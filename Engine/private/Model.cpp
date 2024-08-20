#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Shader.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Channel.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
	, m_iNumMeshContainers(rhs.m_iNumMeshContainers)
	, m_pMeshContainers(rhs.m_pMeshContainers)
	, m_eModelType(rhs.m_eModelType)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	//, m_iCurrentAnim(rhs.m_iCurrentAnim)
	, m_TransformMatrix(rhs.m_TransformMatrix)
{
	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(pMaterial->pTexture[i]);
	}
	if (nullptr != m_pMeshContainers)
		for (_uint i = 0; i < m_iNumMaterials; i++)
		{
			for (auto& pMeshContainer : m_pMeshContainers[i])
				Safe_AddRef(pMeshContainer);
		}
}

const char * CModel::Get_FrontHierarchyNodeName()
{
	return m_HierarchyNodes.front()->Get_Name();
}

vector<CHierarchyNode*>* CModel::Get_Nodes()
{
	return &m_HierarchyNodes;
}

_bool CModel::Get_EndAnimation(_uint iAnimIndex)
{
	return m_Animations[iAnimIndex]->Get_EndAnimation();
}

_double CModel::Get_PlayAcc()
{
	return m_Animations[m_iCurrentAnim]->Get_PlayAcc();
}

void CModel::Set_PlayAcc(_float fPlayAcc)
{
	m_Animations[m_iCurrentAnim]->Set_PlayAcc(fPlayAcc);
}

void CModel::Set_EndPlayAcc()
{
	m_Animations[m_iCurrentAnim]->Set_EndPlayAcc();
}

HRESULT CModel::NativeConstruct_Prototype(MODELTYPE eModelType, const char * pModelFilePath, const char * pModelFileName, _fmatrix TransformMatrix)
{
	char	szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint	iFrag = 0;

	if (TYPE_NONANIM == eModelType)
		iFrag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	else
		iFrag = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	m_pScene = m_Importer.ReadFile(szFullPath, iFrag);

	if (nullptr == m_pScene)
	{
		MSGBOX("m_pScene is nullptr in CModel::NativeConstruct");
		return E_FAIL;
	}

	m_eModelType = eModelType;
	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	if (FAILED(Ready_Materials(pModelFilePath)))
	{
		MSGBOX("Ready_Materials returned E_FAIL in CModel::NativeConstruct");
		return E_FAIL;
	}

	if (TYPE_ANIM == m_eModelType)
	{
		if (FAILED(Ready_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
		{
			MSGBOX("Ready_HierarchyNodes returned E_FAIL in CModel::NativeConstruct");
			return E_FAIL;
		}

		sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});
	}
	if (FAILED(Ready_MeshContainers()))
	{
		MSGBOX("Ready_MeshContainers returned E_FAIL in CModel::NativeConstruct");
		return E_FAIL;
	}

	//if (TYPE_NONANIM == m_eModelType)
	//{
	//	for (_uint i = 0; i < m_iNumMaterials; i++)
	//	{
	//		for (auto& pMeshContainer : m_pMeshContainers[i])
	//			pMeshContainer->Ready_VertexIndexBuffer(eModelType, TransformMatrix);
	//	}
	//	return S_OK;
	//}

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		for (auto& pMeshContainer : m_pMeshContainers[i])
			pMeshContainer->Ready_VertexIndexBuffer(eModelType, TransformMatrix);
	}


	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{

	if (FAILED(Ready_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
	{
		MSGBOX("Ready_HierarchyNodes returned E_FAIL in CModel::NativeConstruct");
		return E_FAIL;
	}
	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});


	if (FAILED(Ready_OffsetMatrices()))
	{
		MSGBOX("Ready_OffsetMatrices returned E_FAIL in CModel::NativeConstruct");
		return E_FAIL;
	}
	if (FAILED(Ready_Animation()))
	{
		MSGBOX("Ready_Animation returned E_FAIL in CModel::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Link_ChannelToNode()))
	{
		MSGBOX("Link_ChannelToNode returned E_FAIL in CModel::NativeConstruct");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CModel::SetUp_AnimIndex(_uint iAnimIndex)
{
	if (iAnimIndex >= m_iNumAnimations)				
	{
		MSGBOX("iAnimIndex >= m_iNumAnimations in CModel::SetUp_AnimIndex");
		return E_FAIL;
	}

	// 애니메이션 교체 보간중에 애니메이션을 바꿔라 라는 명령이 들어왔을 때
	if (0 < m_ChangeTime)
	{
		m_Animations[m_iOldAnim]->Set_PlayAcc();	// 과거 애니메이션 누적 시간을 0으로 초기화

		const vector<CChannel*>* OldChannels = m_Animations[m_iOldAnim]->Get_Channels();
		for (size_t i = 0; i < OldChannels->size(); i++)
		{
			(*OldChannels)[i]->Set_CurrentKeyFrame(0);
		}
	}

	m_iOldAnim = m_iCurrentAnim;
	m_ChangeTime = 0.0;
	m_iCurrentAnim = iAnimIndex;

	return S_OK;
}

HRESULT CModel::SetUp_AnimIndexChange(_uint iAnimIndex)
{
	if (iAnimIndex >= m_iNumAnimations)
	{
		MSGBOX("iAnimIndex >= m_iNumAnimations in CModel::SetUp_AnimIndex");
		return E_FAIL;
	}

	m_Animations[m_iOldAnim]->Set_PlayAcc();	// 과거 애니메이션 누적 시간을 0으로 초기화

	const vector<CChannel*>* OldChannels = m_Animations[m_iOldAnim]->Get_Channels();
	for (size_t i = 0; i < OldChannels->size(); i++)
		(*OldChannels)[i]->Set_CurrentKeyFrame(0);

	m_iOldAnim = m_iCurrentAnim;
	m_ChangeTime = 0.0;
	m_iCurrentAnim = iAnimIndex;

	return S_OK;
}

HRESULT CModel::Change_Anim(_double TimeDelta)
{
	// 애니메이션 누적 시간을 갱신
	m_ChangeTime += 5 * TimeDelta;


	const vector<CChannel*>* OldChannels = m_Animations[m_iOldAnim]->Get_Channels();
	const vector<CChannel*>* CurrentChannels = m_Animations[m_iCurrentAnim]->Get_Channels();

	if (m_ChangeTime > 1)
	{
		m_Animations[m_iOldAnim]->Set_PlayAcc();
		for (size_t i = 0; i < OldChannels->size(); i++)
		{
			(*OldChannels)[i]->Set_CurrentKeyFrame(0);
		}
		m_iOldAnim = m_iCurrentAnim;
		m_ChangeTime = 0.0;
		return S_OK;
	}


	_vector		vScale, vRotation, vPosition;

	size_t RoofSize = OldChannels->size() <= CurrentChannels->size() ? OldChannels->size() : CurrentChannels->size();

	// 현재 내 애니메이션 상태에서 재생시간에 따른 모든 뼈의 상태를 갱신한다.
	for (size_t i = 0; i < OldChannels->size(); i++)
	{
		_int iCurrentIndex = (_int)i;
		if (OldChannels->size() != CurrentChannels->size())
		{
			_bool bSearch = false;
			for (size_t j = 0; j < CurrentChannels->size(); j++)
			{
				if (!strcmp((*OldChannels)[i]->Get_Name(), (*CurrentChannels)[j]->Get_Name()))
				{
					iCurrentIndex = (_int)j;
					bSearch = true;
					break;
				}
			}
			if (false == bSearch)
				continue;
		}

		KEYFRAME*	OldKeyFrame = (*OldChannels)[i]->Get_CurrentKeyFrameInfo();
		KEYFRAME*	CurrentKeyFrame = (*CurrentChannels)[iCurrentIndex]->Get_CurrentKeyFrameInfo();

		_vector	vSourScale, vDestScale;
		_vector	vSourRotation, vDestRotation;
		_vector	vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&OldKeyFrame->vScale);
		vSourRotation = XMLoadFloat4(&OldKeyFrame->vRotation);
		vSourPosition = XMLoadFloat3(&OldKeyFrame->vPosition);

		vDestScale = XMLoadFloat3(&CurrentKeyFrame->vScale);
		vDestRotation = XMLoadFloat4(&CurrentKeyFrame->vRotation);
		vDestPosition = XMLoadFloat3(&CurrentKeyFrame->vPosition);

		// 보간 함수. 두 개의 벡터값을 세 번째 인자만큼의 퍼센트 수치로 보간해서 리턴해줌.
		vScale = XMVectorLerp(vSourScale, vDestScale, _float(m_ChangeTime));
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(m_ChangeTime));
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(m_ChangeTime));

		vPosition = XMVectorSetW(vPosition, 1.f);

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		(*CurrentChannels)[iCurrentIndex]->Set_TransformationMatrix(TransformationMatrix);
	}
	return S_OK;
}

HRESULT CModel::Update_CombinedTransformationMatrices(_uint iType, _double TimeDelta)
{
	// 모든 뼈들에게 현재 시간에 맞는 상태를 저장시킨다.
	m_Animations[m_iCurrentAnim]->Update_TransformMatrices(iType, TimeDelta);
	if (m_iOldAnim != m_iCurrentAnim)
	{
		Change_Anim(TimeDelta);
	}
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		// 부모로부터 자식까지의 행렬 상태를 누적시키며 셋팅해준다.
		pHierarchyNode->Update_CombinedTransformationMatrix();
	}
	return S_OK;
}

HRESULT CModel::Bind_OnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char * pValueName)
{
	if (iMaterialIndex >= m_iNumMaterials)
	{
		MSGBOX("iMaterialIndex >= m_iNumMaterials in CModel::Bind_OnShader");
		return E_FAIL;
	}

	CTexture*	pTexture = m_Materials[iMaterialIndex]->pTexture[eTextureType];
	if (nullptr == pTexture)
	{
		//MSGBOX("nullptr == pTexture in CModel::Bind_OnShader");
		return S_OK;
	}

	return pTexture->SetUp_OnShader(pShader, pValueName);
}

HRESULT CModel::Render(CShader* pShader, _uint iPassIndex, _uint iMaterialIndex, const char* pBoneValueName, _bool bOurArm)
{
	if (iMaterialIndex >= m_iNumMaterials ||
		nullptr == m_pMeshContainers)
	{
		MSGBOX("iMaterialIndex >= m_iNumMaterials || nullptr == m_pMeshContainers in CModel::Render");
		return E_FAIL;
	}

	_float4x4		BoneMatrices[256];	// 셰이더에 넘겨줄 배열 선언

	for (auto& pMeshContainer : m_pMeshContainers[iMaterialIndex])	// 이 머테리얼을 사용하는 매쉬 컨테이너를 순회하며
	{
		if (TYPE_ANIM == m_eModelType)	// 애니메이션 타입일 때
		{
			if (true == bOurArm&& (!strcmp(pMeshContainer->Get_AIMesh()->mName.data, "hero_fpp_101_A_L") || !strcmp(pMeshContainer->Get_AIMesh()->mName.data, "hero_fpp_101_A_R")))
				continue;
			ZeroMemory(&BoneMatrices, sizeof(_float4x4) * 256);

			pMeshContainer->SetUp_Matrices(BoneMatrices, &m_HierarchyNodes, XMLoadFloat4x4(&m_TransformMatrix));	// 본 매트릭스를 채워줌

			pShader->Set_RawValue(pBoneValueName, BoneMatrices, sizeof(_float4x4) * 256);	// 셰이더에 본 매트릭스를 던져줌
		}

		if (nullptr != pMeshContainer)
			pMeshContainer->Render(pShader, iPassIndex);
	}
	return S_OK;
}

HRESULT CModel::Ready_MeshContainers()
{
	if (nullptr == m_pScene)
	{
		MSGBOX("m_pScene is nullptr in CModel::Ready_MeshContainers");
		return E_FAIL;
	}
	m_iNumMeshContainers = m_pScene->mNumMeshes;

	m_pMeshContainers = new MESHCONTAINERS[m_iNumMaterials];

	for (_uint i = 0; i < m_iNumMeshContainers; i++)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, m_pScene->mMeshes[i]);
		if (nullptr == pMeshContainer)
		{
			MSGBOX("pMeshContainer is nullptr in CModel::Ready_MeshContainers");
			return E_FAIL;
		}
		for (_uint j = 0; j < m_pScene->mMeshes[i]->mNumBones; j++)	// 해당 매쉬의 뼈의 갯수만큼 루프
		{
			_uint	iIndex = 0;

			// 해당 뼈의 이름으로 노드의 객체와 인덱스값을 받아온다.
			CHierarchyNode* pHierarchyNode = Find_HierarchyNode(m_pScene->mMeshes[i]->mBones[j]->mName.data, &iIndex);

			if (nullptr == pHierarchyNode)
			{
				MSGBOX("pHierarchyNode is nullptr in CModel::Ready_MeshContainers");
				return E_FAIL;
			}

			pMeshContainer->Add_HierarChyNode(iIndex);	// 받아온 인덱스를 노드 컨테이너에 추가
		}
		m_pMeshContainers[m_pScene->mMeshes[i]->mMaterialIndex].push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char * pModelFilePath)
{
	if (nullptr == m_pScene)
	{
		MSGBOX("m_pScene is nullptr in CModel::Ready_Materials");
		return E_FAIL;
	}

	m_iNumMaterials = m_pScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		MESHMATERIALDESC*	pMeshMaterialDesc = new MESHMATERIALDESC;
		ZeroMemory(pMeshMaterialDesc, sizeof(MESHMATERIALDESC));

		for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString	TexturePath;

			if (FAILED(m_pScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &TexturePath)))
				continue;

			char	szFullPath[MAX_PATH] = "";
			char	szFileName[MAX_PATH] = "";
			char	szExt[MAX_PATH] = "";

			_splitpath_s(TexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szTextureFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szTextureFullPath, MAX_PATH);

			pMeshMaterialDesc->pTexture[j] = CTexture::Create(m_pDevice, m_pDeviceContext, szTextureFullPath);
			if (nullptr == pMeshMaterialDesc->pTexture[j])
			{
				MSGBOX("nullptr == pMeshMaterialDesc->pTexture[j] in CModel::Ready_Materials");
				return E_FAIL;
			}
		}

		m_Materials.push_back(pMeshMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(aiNode * pNode, CHierarchyNode * pParent, _uint iDepth)
{
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CModel::Ready_HierarchyNodes");
		return E_FAIL;
	}

	_float4x4	TransformationMatrix;
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_float4x4));

	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, TransformationMatrix, iDepth, pParent);
	if (nullptr == pHierarchyNode)
	{
		MSGBOX("nullptr == pHierarchyNode in CModel::Ready_HierarchyNodes");
		return E_FAIL;
	}

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; i++)
	{
		Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth + 1);
	}

	return S_OK;
}

HRESULT CModel::Ready_OffsetMatrices()
{
	for (_uint i = 0; i < m_iNumMaterials; i++)		// 머테리얼 갯수 만큼 루프
	{
		for (auto& pMeshContainer : m_pMeshContainers[i])	// i번째 머테리얼을 쓰는 매쉬 컨테이너를 순회
		{
			_uint	iNumBones = pMeshContainer->Get_NumBones();

			aiMesh*	pAIMesh = pMeshContainer->Get_AIMesh();

			for (_uint j = 0; j < iNumBones; j++)			// 뼈의 갯수만큼 순회
			{
				aiBone*		pBone = pAIMesh->mBones[j];		// j번째 뼈를 받아옴

				CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pBone->mName.data);	// 해당 이름을 가진 노드를 받아옴.

				if (nullptr == pHierarchyNode)
				{
					MSGBOX("pHierarchyNode is nullptr in CModel::Ready_OffsetMatrices");
					return E_FAIL;
				}

				_float4x4			OffsetMatrix;
				memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_float4x4));	// 뼈의 오프셋 매트릭스를 가져옴

				pHierarchyNode->Set_OffsetMatrix(OffsetMatrix);						// 오프셋 매트릭스를 채워줌
			}
		}
	}
	
	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	if (nullptr == m_pScene)
	{
		MSGBOX("nullptr == m_pScene in CModel::Ready_Animation");
		return E_FAIL;
	}

	m_iNumAnimations = m_pScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; i++)	// 애니메이션 갯수만큼 루프
	{
		aiAnimation*	pAIAnimation = m_pScene->mAnimations[i];	// i번째 인덱스 애니메이션을 받아옴

		// 가져온 애니메이션 정보로 애니메이션 객체 생성
		CAnimation*		pAnimation = CAnimation::Create(pAIAnimation->mName.data, pAIAnimation->mDuration, pAIAnimation->mTicksPerSecond);
		if (nullptr == pAnimation)
		{
			MSGBOX("pAnimation is nullptr in CModel::Ready_Animation");
			return E_FAIL;
		}

		pAnimation->Reserve(pAIAnimation->mNumChannels);		// 현재 해당 애니메이션의 뼈의 갯수만큼 공간 할당

		for (_uint j = 0; j < pAIAnimation->mNumChannels; j++)	// 뼈의 갯수만큼 루프
		{
			aiNodeAnim*	pAIChannel = pAIAnimation->mChannels[j];// j번째 인덱스에 있는 뼈를 받아온다

			CHierarchyNode*	pNode = Find_HierarchyNode(pAIChannel->mNodeName.data);	// 현재 뼈의 이름과 같은 하이어라키노드를 찾아서 넣어준다.
			if (nullptr == pNode)
			{
				MSGBOX("pNode == nullptr in CModel::Ready_Animation");
				return E_FAIL;
			}

			CChannel*	pChannel = CChannel::Create(pAIChannel->mNodeName.data);	// 뼈의 정보를 토대로 뼈 객체 생성
			if (nullptr == pChannel)
			{
				MSGBOX("pChannel is nullptr in CModel::Ready_Animation");
				return E_FAIL;
			}

			// 키 프레임의 총 갯수는 스케일, 로테이션, 포지션중에 가장 많은 프레임 수이다.
			// 보통 로테이션, 포지션은 거의 가지고 있고 스케일은 맨 앞에 하나정도 올 때도 많다.
			_uint		iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, pAIChannel->mNumPositionKeys);

			pChannel->Reserve(iNumKeyFrames);	// 키 프레임의 총 갯수만큼 저장공간 할당

			_float3	vScale;
			_float4 vRotation;
			_float3 vPosition;
			_double Time;

			for (_uint k = 0; k < iNumKeyFrames; k++)	// 키 프레임 수만큼 루프
			{
				KEYFRAME*		pKeyFrame = new KEYFRAME;	// 키 프레임 정보 구조체 동적 할당
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				if (pAIChannel->mNumScalingKeys > k)	// k번째에 스케일 키 프레임이 존재하면(스케일 키 프레임 갯수보다 현재 인덱스가 작으면)
				{
					memcpy(&vScale, &pAIChannel->mScalingKeys[k].mValue, sizeof(_float3));	// 스케일 키 프레임 값 채움
					Time = pAIChannel->mScalingKeys[k].mTime;	// 이 키 프레임이 재생되어야할 시간
				}

				if (pAIChannel->mNumRotationKeys > k)	// k번째에 로테이션 키 프레임이 존재하면(로테이션 키 프레임 갯수보다 현재 인덱스가 작으면)
				{
					vRotation.x = pAIChannel->mRotationKeys[k].mValue.x;	// 로테이션은 쿼터니언 단위이다.
					vRotation.y = pAIChannel->mRotationKeys[k].mValue.y;	// 쿼터니언은 wxyz순이기 때문에 memcpy를 하면 잘못 들어간다.
					vRotation.z = pAIChannel->mRotationKeys[k].mValue.z;
					vRotation.w = pAIChannel->mRotationKeys[k].mValue.w;
					Time = pAIChannel->mRotationKeys[k].mTime;				// 이 키 프레임이 재생되어야할 시간
				}

				if (pAIChannel->mNumPositionKeys > k)	// k번째에 포지션 키 프레임이 존재하면(포지션 키 프레임 갯수보다 현재 인덱스가 작으면)
				{
					memcpy(&vPosition, &pAIChannel->mPositionKeys[k].mValue, sizeof(_float3));	// 포지션 키 프레임 값 채움
					Time = pAIChannel->mPositionKeys[k].mTime;	// 이 키 프레임이 재생되어야할 시간
				}

				pKeyFrame->vScale = vScale;				// 키 프레임 구조체에 값 채워줌
				pKeyFrame->vRotation = vRotation;
				pKeyFrame->vPosition = vPosition;
				pKeyFrame->Time = Time;

				pChannel->Add_KeyFrame(pKeyFrame);		// 키 프레임 컨테이너에 추가
			}

			pAnimation->Add_Channel(pChannel);			// 뼈 컨테이너에 추가
		}

		m_Animations.push_back(pAnimation);				// 애니메이션 컨테이너에 추가
	}
	return S_OK;
}

HRESULT CModel::Link_ChannelToNode()
{
	for (auto& pAnimation : m_Animations)	// 모든 애니메이션을 루프
	{
		const vector<CChannel*>* pChannels = pAnimation->Get_Channels();	// 해당 애니메이션의 뼈 목록을 가져옴

		for (auto& pChannels : *pChannels)									// 그 뼈 목록을 루프
		{
			CHierarchyNode* pNode = Find_HierarchyNode(pChannels->Get_Name());	// 해당 뼈의 이름과 같은 노드가 있는지 검사해서 가져옴.
			if (nullptr == pNode)
			{
				MSGBOX("pNode is nullptr in CModel::Link_ChannelToNode");
				return E_FAIL;
			}

			pChannels->Set_HierarchyNodePtr(pNode);								// 가져온 노드를 해당 뼈가 가지고 있게 보내줌.
		}
	}
	return S_OK;
}

CHierarchyNode * CModel::Find_HierarchyNode(const char * pName, _uint* pOut)
{
	_uint	iIndex = 0;
	CHierarchyNode* pNode = nullptr;

	for (auto& pHierarchyNode : m_HierarchyNodes)		// 모든 하이어라키 노드를 순회
	{
		if (!strcmp(pHierarchyNode->Get_Name(), pName))	// 해당 노드의 이름과 찾는 이름이 같으면
		{
			pNode = pHierarchyNode;
			if (nullptr != pOut)
				*pOut = iIndex;

			break;
		}

		iIndex++;
	}

	return pNode;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, MODELTYPE eModelType, const char * pModelFilePath, const char * pModelFileName, _fmatrix TransformMatrix)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eModelType, pModelFilePath, pModelFileName, TransformMatrix)))
	{
		MSGBOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pMaterials : m_Materials)
	{
		for (_int i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release(pMaterials->pTexture[i]);
		
		if (false == m_isCloned)
			Safe_Delete(pMaterials);
	}
	m_Materials.clear();

	if (nullptr != m_pMeshContainers)
	{
		for (_uint i = 0; i < m_iNumMaterials; i++)
		{

			for (auto& pMeshContainer : m_pMeshContainers[i])
				Safe_Release(pMeshContainer);
			
			if (false == m_isCloned)
				m_pMeshContainers[i].clear();
		}

	}

	for (auto& pNode : m_HierarchyNodes)
		Safe_Release(pNode);

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pMeshContainers);
		m_Importer.FreeScene();
	}
}
