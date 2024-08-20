#include "stdafx.h"
#include "..\public\ImGui_Animation.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"

CImGui_Animation::CImGui_Animation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CImGui_Animation::CImGui_Animation(const CImGui_Animation & rhs)
	: CGameObject(rhs)
{
}

CModel * CImGui_Animation::Get_Model()
{
	return m_pModelCom;
}

HRESULT CImGui_Animation::Set_Model(_tchar * pPrototypeTag)
{
	if (nullptr != m_pModelCom)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);

	}
	m_Components.erase(TEXT("Com_Model"));
#ifdef USE_IMGUI
	if (FAILED(__super::Add_Component(LEVEL_STATIC, pPrototypeTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CImGui_Animation::Set_Model");
		return E_FAIL;
	}
	m_pModelCom->SetUp_AnimIndex(0);
	memcpy(m_szBoneName, m_pModelCom->Get_FrontHierarchyNodeName(), MAX_PATH);
#else
	if (FAILED(__super::Add_Component(g_eCurrentLevel, pPrototypeTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CImGui_Animation::Set_Model");
		return E_FAIL;
	}
#endif
	return S_OK;
}

void CImGui_Animation::Set_ColliderRadius(_float fRadius)
{
	m_fColliderRadius = fRadius;

	m_tColliderDesc.vScale = _float3(m_fColliderRadius, m_fColliderRadius, m_fColliderRadius);

	m_pColliderCom->Set_ColliderDesc(m_tColliderDesc);
}

HRESULT CImGui_Animation::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CImGui_Animation::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CImGui_Animation::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CImGui_Animation::NativeConstruct");
		return E_FAIL;
	}

	m_tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	m_tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	m_tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CMonster::NativeConstruct");
		return E_FAIL;
	}


	m_pModelCom->SetUp_AnimIndex(0);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

_int CImGui_Animation::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CImGui_Animation::Tick");
		return -1;
	}

	CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode(m_szBoneName);

	_matrix LocalMatrix = XMMatrixIdentity();
	LocalMatrix.r[3] = m_vLocalPos;

	_matrix			TransformMatrix = LocalMatrix * XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix())  * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pColliderCom->Update_Transform(TransformMatrix);

	return _int();
}

_int CImGui_Animation::LateTick(_double dTimeDelta)
{

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CImGui_Animation::LateTick");
		return -1;
	}

	m_pModelCom->Update_CombinedTransformationMatrices(ANIM_ROOF, dTimeDelta);

	CHierarchyNode* pBone = m_pModelCom->Find_HierarchyNode(m_szBoneName);

	// ÀýµÎÃ¼
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 30.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return _int();
}




HRESULT CImGui_Animation::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pVIBufferCom in CImGui_Animation::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CImGui_Animation::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 0, i, "g_BoneMatrices");
	}
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CImGui_Animation::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CImGui_Animation::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CImGui_Animation::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_2164_model"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CImGui_Animation::SetUp_Components(Model)");
		return E_FAIL;
	}
	//CHierarchyNode* pBone = m_pModelCom->Find_HierarchyNode(m_pModelCom->Get_FrontHierarchyNodeName());

	/* For.Com_Collder */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &m_tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CImGui_Animation::SetUp_Components(Collider)");
		return E_FAIL;
	}
	m_pColliderCom->Set_Parent(this);

	return S_OK;
}

HRESULT CImGui_Animation::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CImGui_Animation::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CImGui_Animation::SetUp_ConstantTable");
		return E_FAIL;
	}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_vector))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CImGui_Animation::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CImGui_Animation::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CImGui_Animation * CImGui_Animation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CImGui_Animation*	pInstance = new CImGui_Animation(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CImGui_Animation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CImGui_Animation::Clone(void * pArg)
{
	CImGui_Animation*	pInstance = new CImGui_Animation(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CImGui_Animation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_Animation::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
