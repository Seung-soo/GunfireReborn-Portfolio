#include "stdafx.h"
#include "..\public\UFOBuff.h"
#include "GameInstance.h"
#include "Monster.h"

CUFOBuff::CUFOBuff(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CUFOBuff::CUFOBuff(const CUFOBuff & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUFOBuff::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CUFOBuff::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUFOBuff::NativeConstruct(void * pArg)
{


	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CUFOBuff::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CUFOBuff::NativeConstruct");
		return E_FAIL;
	}
	m_pMonster = static_cast<CMonster*>(pArg);
	m_pBuffTransform = static_cast<CTransform*>(m_pMonster->Get_Component(TEXT("Com_Transform")));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pBuffTransform->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Scaled(XMVectorSet(2.f, 2.f, 2.f, 0.f));

	return S_OK;
}

_int CUFOBuff::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CUFOBuff::Tick");
		return -1;
	}

	m_BuffTime -= dTimeDelta;
	if (m_BuffTime < 0.0)
	{
		m_pMonster->Set_Buff(false);
		return DEAD;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pBuffTransform->Get_State(CTransform::STATE_POSITION));

	return _int();
}

_int CUFOBuff::LateTick(_double dTimeDelta)
{
	if (m_bDead)
		return -1;

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CUFOBuff::LateTick");
		return -1;
	}

	// ÀýµÎÃ¼
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}




HRESULT CUFOBuff::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pVIBufferCom in CUFOBuff::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CUFOBuff::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Render(m_pShaderCom, 3, i);
	}

	return S_OK;
}

HRESULT CUFOBuff::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFOBuff::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFOBuff::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Aura"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFOBuff::SetUp_Components(Model)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUFOBuff::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CUFOBuff::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CUFOBuff::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CUFOBuff::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CUFOBuff::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CUFOBuff * CUFOBuff::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUFOBuff*	pInstance = new CUFOBuff(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUFOBuff");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUFOBuff::Clone(void * pArg)
{
	CUFOBuff*	pInstance = new CUFOBuff(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUFOBuff");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUFOBuff::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
