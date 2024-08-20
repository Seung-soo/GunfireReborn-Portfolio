#include "stdafx.h"
#include "..\public\DamageNumber.h"
#include "GameInstance.h"
#include "PipeLine.h"

CDamageNumber::CDamageNumber(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CDamageNumber::CDamageNumber(const CDamageNumber & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDamageNumber::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamageNumber::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_tNumberDesc, pArg, sizeof(NUMBERDESC));

	m_pTransformCom->Scaled(XMVectorSet(0.6f, 0.6f, 1.f, 0.f));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_fSizeX = 32.f;
	m_fSizeY = 32.f;
	m_fDepth = 8.f;
	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));

	return S_OK;
}

_int CDamageNumber::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;
	if (0 > __super::Tick(TimeDelta))
		return -1;

	m_fAlpha -= (_float)TimeDelta * 200.f;
	if (0.f > m_fAlpha)
		m_bDead = true;
	m_fDepth = m_fAlpha * 0.1f;

	return _int();
}

_int CDamageNumber::LateTick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	if (0.f <= m_fZ)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}




HRESULT CDamageNumber::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CDamageNumber::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 17)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CDamageNumber::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDamageNumber::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CDamageNumber::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CDamageNumber::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CDamageNumber::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Number_White"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CDamageNumber::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDamageNumber::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CDamageNumber::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CDamageNumber::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CDamageNumber::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CDamageNumber::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	_int iAlpha = (_int)m_fAlpha;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iHitAlpha", &iAlpha, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CDamageNumber::SetUp_ConstantTable(g_iHitAlpha)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_tNumberDesc.vColor, sizeof(_float3))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CDamageNumber::SetUp_ConstantTable(g_vColor)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_tNumberDesc.iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CDamageNumber::SetUp_ConstantTable");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CDamageNumber * CDamageNumber::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDamageNumber*	pInstance = new CDamageNumber(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CDamageNumber");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDamageNumber::Clone(void * pArg)
{
	CDamageNumber*	pInstance = new CDamageNumber(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CDamageNumber");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDamageNumber::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
