#include "stdafx.h"
#include "..\public\RockWarning.h"
#include "GameInstance.h"

CRockWarning::CRockWarning(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CRockWarning::CRockWarning(const CRockWarning & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRockWarning::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CRockWarning::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRockWarning::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CRockWarning::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CRockWarning::NativeConstruct");
		return E_FAIL;
	}
	memcpy(&m_tWarningDesc, pArg, sizeof(WARNINGDESC));

	m_pTransformCom->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tWarningDesc.vPosition);
	return S_OK;
}

_int CRockWarning::Tick(_double TimeDelta)
{
	if (true == *m_tWarningDesc.pDead)
		return DEAD;
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CRockWarning::Tick");
		return E_FAIL;
	}

	if (6.f > m_fScale)
		m_fScale += (_float)TimeDelta * 18.f;

	m_pTransformCom->Scaled(XMVectorSet(m_fScale, 1.f, m_fScale, 0.f));


	return _int();
}

_int CRockWarning::LateTick(_double TimeDelta)
{
	if (true == *m_tWarningDesc.pDead)
		return DEAD;

	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CRockWarning::LateTick");
		return E_FAIL;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}

HRESULT CRockWarning::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CRockWarning::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 24)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CRockWarning::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRockWarning::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRockWarning::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRockWarning::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectXZ"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRockWarning::SetUp_Components(VIBuffer_RectXZ)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_WaterBallWarning"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRockWarning::SetUp_Components(Prototype_Component_Texture_WaterBallWarning)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CRockWarning::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CRockWarning::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CRockWarning::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRockWarning::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRockWarning::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CRockWarning::SetUp_ConstantTable");
		return E_FAIL;
	}


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CRockWarning * CRockWarning::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRockWarning*	pInstance = new CRockWarning(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CRockWarning");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRockWarning::Clone(void * pArg)
{
	CRockWarning*	pInstance = new CRockWarning(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CRockWarning");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRockWarning::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
