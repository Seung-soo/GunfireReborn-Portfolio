#include "..\public\MeshContainer.h"
#include "HierarchyNode.h"
CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMeshContainer::NativeConstruct_Prototype(aiMesh * pAIMesh)
{
	m_pAIMesh = pAIMesh;	// 매쉬정보
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;	// 매쉬 인덱스
	m_iNumBones = pAIMesh->mNumBones;			// 뼈 갯수

	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::Ready_VertexIndexBuffer(CModel::MODELTYPE eMeshType, _fmatrix TransformMatrix)
{
#pragma region VERTICES
	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertices = m_pAIMesh->mNumVertices;
	m_iNumVertexBuffers = 1;

	HRESULT		hr = 0;

	if (CModel::TYPE_NONANIM == eMeshType)
		hr = Ready_NonAnimMeshContainer(m_pAIMesh, TransformMatrix);
	else
		hr = Ready_AnimMeshContainer(m_pAIMesh);
#pragma endregion

#pragma region INDICES
	m_iNumPrimitive = m_pAIMesh->mNumFaces;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; i++)
	{
		pIndices[i]._0 = m_pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = m_pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = m_pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
	{
		MSGBOX("Create_IndexBuffer returned E_FAIL in CMeshContainer::NativeConstruct_Prototype");
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::Add_HierarChyNode(_uint iNodeIndex)
{
	m_HierarchyNodeIndices.push_back(iNodeIndex);

	return S_OK;
}

HRESULT CMeshContainer::SetUp_Matrices(_float4x4 * pBoneMatrices, const vector<CHierarchyNode*>* pNodes, _fmatrix TransformMatrix)
{
	_uint	iIndex = 0;

	if (0 == m_iNumBones)		// 뼈의 갯수가 0개이면
	{
		auto iter = find_if(pNodes->begin(), pNodes->end(), [&](CHierarchyNode* pHierarchyNode) 
		{
			return !strcmp(m_szName, pHierarchyNode->Get_Name());	// 매쉬의 이름과 하이어라키 노드의 이름이 같다면 그 부분을 iter에 저장
		});

		if (iter == pNodes->end())
			XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());	// 본 매트릭스를 항등으로
		else
		{
			_matrix	OffsetMatrix = (*iter)->Get_OffsetMatrix();
			_matrix CombinedTransformationMatrix = (*iter)->Get_CombinedTransformationMatrix();
			_matrix BoneMatrix = OffsetMatrix * CombinedTransformationMatrix * TransformMatrix;

			XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(BoneMatrix));
		}

	}

	for (auto& iNodeIndex : m_HierarchyNodeIndices)	// 현재 매쉬에 영향을 주는 모든 노드를 순회
	{
		_matrix		OffsetMatrix = (*pNodes)[iNodeIndex]->Get_OffsetMatrix();
		_matrix		CombinedTransformationMatrix = (*pNodes)[iNodeIndex]->Get_CombinedTransformationMatrix();

		_matrix		BoneMatrix = OffsetMatrix * CombinedTransformationMatrix * TransformMatrix;	// 오프셋 매트릭스와 컴바인 매트릭스를 곱해서 뼈 매트릭스를 만듬

		XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(BoneMatrix));	// 셰이더에 던질 것이기 때문에 본 매트릭스 행렬을 전치해서 던짐
	}

	return S_OK;
}

HRESULT CMeshContainer::Ready_NonAnimMeshContainer(aiMesh * pAIMesh, _fmatrix TransformMatrix)
{
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = sizeof(VTXMODEL) * m_iNumVertices;
	m_VBDesc.StructureByteStride = sizeof(VTXMODEL);
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	VTXMODEL*	pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(
			&pVertices[i].vPosition), TransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformCoord(XMLoadFloat3(
			&pVertices[i].vNormal), TransformMatrix));

		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
	{
		MSGBOX("Create_VertexBuffer returned E_FAIL in CMeshContainer::Ready_NonAnimMeshContainer");
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
	

	return S_OK;
}

HRESULT CMeshContainer::Ready_AnimMeshContainer(aiMesh * pAIMesh)
{
		ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = sizeof(VTXANIMMODEL) * m_iNumVertices;
	m_VBDesc.StructureByteStride = sizeof(VTXANIMMODEL);
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	VTXANIMMODEL*	pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	if (FAILED(Ready_SkinnedInfo(pVertices)))
	{
		MSGBOX("Ready_SkinnedInfo returned E_FAIL in CMeshContainer::Ready_AnimMeshContainer");
		return E_FAIL;
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
	{
		MSGBOX("Create_VertexBuffer returned E_FAIL in CMeshContainer::Ready_NonAnimMeshContainer");
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
	

	return S_OK;
}

HRESULT CMeshContainer::Ready_SkinnedInfo(VTXANIMMODEL * pVertices)
{
	if (nullptr == m_pAIMesh)
	{
		MSGBOX("nullptr == m_pAIMesh in CMeshContainer::Ready_SkinnedInfo");
		return E_FAIL;
	}
	
	m_iNumBones = m_pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		aiBone*		pAIBone = m_pAIMesh->mBones[i];

		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y = pAIBone->mWeights[j].mWeight;

			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z = pAIBone->mWeights[j].mWeight;

			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.w = pAIBone->mWeights[j].mWeight;

			}
		}
	}
	return S_OK;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, aiMesh* pAIMesh)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pAIMesh)))
	{
		MSGBOX("Failed to Created CMeshContainer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*	pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMeshContainer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

	m_HierarchyNodeIndices.clear();
}
