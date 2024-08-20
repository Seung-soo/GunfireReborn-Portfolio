#pragma once

#include "Component.h"

BEGIN(Engine)
class CHierarchyNode;

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODELTYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END };
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;
public:
	_uint							Get_NumMaterials() const { return m_iNumMaterials; }
	const aiScene*					Get_Scene() const { return m_pScene; }
	const char*						Get_FrontHierarchyNodeName();
	vector<CHierarchyNode*>*		Get_Nodes();
	_bool							Get_EndAnimation(_uint iAnimIndex);
	_uint							Get_CurrentAnimIndex() { return m_iCurrentAnim; }
	_double							Get_PlayAcc();
	_float4x4						Get_TransformMatrix() { return m_TransformMatrix; }
public:
	void							Set_PlayAcc(_float fPlayAcc);
	void							Set_EndPlayAcc();
	void							Set_ModelType(MODELTYPE eType) { m_eModelType = eType; }
public:
	virtual HRESULT		NativeConstruct_Prototype(MODELTYPE eModelType, const char* pModelFilePath, const char* pModelFileName, _fmatrix TransformMatrix);
	virtual HRESULT		NativeConstruct(void* pArg);
public:
	HRESULT				SetUp_AnimIndex(_uint iAnimIndex);	// 현재 애니메이션 갱신 해주는 함수
	HRESULT				SetUp_AnimIndexChange(_uint iAnimIndex);	// 현재 애니메이션 갱신 해주는 함수(중간에 바꿀 수도 있는)
	HRESULT				Change_Anim(_double TimeDelta);
	HRESULT				Update_CombinedTransformationMatrices(_uint iType, _double TimeDelta);	// 누적행렬을 갱신해주는 함수
	HRESULT				Bind_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pValueName);
	HRESULT				Render(class CShader* pShader, _uint iPassIndex, _uint iMaterialIndex, const char* pBoneValueName = nullptr, _bool bOurArm = false);
public:
	CHierarchyNode*		Find_HierarchyNode(const char* pName, _uint* pOut = nullptr);
private:
	const aiScene*		m_pScene = nullptr;
	Importer			m_Importer;
private:
	_uint									m_iNumMeshContainers = 0;
	vector<class CMeshContainer*>*			m_pMeshContainers = nullptr;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;
	MODELTYPE								m_eModelType = TYPE_END;
	_float4x4								m_TransformMatrix;
private:
	_uint									m_iNumMaterials = 0;
	vector<MESHMATERIALDESC*>				m_Materials;
	typedef vector<MESHMATERIALDESC*>		MATERIALS;
private:
	vector<CHierarchyNode*>					m_HierarchyNodes;
	typedef vector<CHierarchyNode*>			HIERARCHYNODES;
private:
	_uint									m_iCurrentAnim = 0;	// 현재 애니메이션 인덱스
	_uint									m_iOldAnim = 0;
	_double									m_ChangeTime = 0.f;
	vector<_uint>							m_vOldKeyFrame;

	_uint									m_iNumAnimations;	// 애니메이션의 갯수
	vector<class CAnimation*>				m_Animations;		// 애니메이션들을 담은 벡터 컨테이너
	typedef vector<class CAnimation*>		ANIMATIONS;
private:
	HRESULT				Ready_MeshContainers();
	HRESULT				Ready_Materials(const char* pModelFilePath);
	HRESULT				Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth);
	HRESULT				Ready_OffsetMatrices();
	HRESULT				Ready_Animation();
	HRESULT				Link_ChannelToNode();
public:
	static CModel*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELTYPE eModelType, const char* pModelFilePath, const char* pModelFileName, _fmatrix TransformMatrix);
	virtual CComponent*	Clone(void* pArg) override;
	virtual void		Free() override;
};

END