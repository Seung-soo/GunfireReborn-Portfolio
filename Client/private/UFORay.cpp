#include "stdafx.h"
#include "..\public\UFORay.h"
#include "GameInstance.h"
#include "Monster.h"

CUFORay::CUFORay(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CUFORay::CUFORay(const CUFORay & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUFORay::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CUFORay::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUFORay::NativeConstruct(void * pArg)
{


	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CUFORay::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CUFORay::NativeConstruct");
		return E_FAIL;
	}

	m_pBuffTransform = static_cast<CTransform*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pBuffTransform->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Scaled(XMVectorSet(4.f, 2.f, 4.f, 0.f));
	return S_OK;
}

_int CUFORay::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CUFORay::Tick");
		return -1;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pBuffTransform->Get_State(CTransform::STATE_POSITION));

	return _int();
}

_int CUFORay::LateTick(_double dTimeDelta)
{
	if (m_bDead)
		return -1;

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CUFORay::LateTick");
		return -1;
	}

	// ����ü
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}




HRESULT CUFORay::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pVIBufferCom in CUFORay::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CUFORay::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 13, i);
	}

	return S_OK;
}

HRESULT CUFORay::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFORay::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFORay::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_UFORay"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFORay::SetUp_Components(Model)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUFORay::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CUFORay::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CUFORay::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CUFORay::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CUFORay::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CUFORay * CUFORay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUFORay*	pInstance = new CUFORay(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUFORay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUFORay::Clone(void * pArg)
{
	CUFORay*	pInstance = new CUFORay(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUFORay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUFORay::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
