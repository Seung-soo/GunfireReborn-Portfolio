#include "stdafx.h"
#include "..\public\HpNumber.h"
#include "GameInstance.h"

CHpNumber::CHpNumber(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CHpNumber::CHpNumber(const CHpNumber & rhs)
	: CGameObject(rhs)
	, m_iTexIndex(rhs.m_iTexIndex)
{
}

HRESULT CHpNumber::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpNumber::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_tNumInfo, pArg, sizeof(NUMINFO));

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_pTransformCom->Scaled(XMVectorSet(m_tNumInfo.fSizeX, m_tNumInfo.fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(*m_tNumInfo.pParentX + m_tNumInfo.fX - g_iWinCX * 0.5f, -(*m_tNumInfo.pParentY + m_tNumInfo.fY) + g_iWinCY * 0.5f, 0.f, 1.f));

	return S_OK;
}

_int CHpNumber::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	return _int();
}

_int CHpNumber::LateTick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pTransformCom->Scaled(XMVectorSet(m_tNumInfo.fSizeX, m_tNumInfo.fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(*m_tNumInfo.pParentX + m_tNumInfo.fX - g_iWinCX * 0.5f, -(*m_tNumInfo.pParentY + m_tNumInfo.fY) + g_iWinCY * 0.5f, 0.f, 1.f));

	if (true == m_bRender)
	{
		if (false == m_tNumInfo.bStopUI)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STOPUI, this);
	}

	return _int();
}

HRESULT CHpNumber::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	if (FAILED(m_pTextureCom[m_tNumInfo.eNumColor]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CHpNumber::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 3);
	return S_OK;
}

HRESULT CHpNumber::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHpNumber::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHpNumber::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHpNumber::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Number_White"), TEXT("Com_Texture_White"), (CComponent**)&m_pTextureCom[COLOR_WHITE])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHpNumber::SetUp_Components(Texture_White)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Number_Green"), TEXT("Com_Texture_Green"), (CComponent**)&m_pTextureCom[COLOR_GREEN])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHpNumber::SetUp_Components(Texture_Green)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Number_Blue"), TEXT("Com_Texture_Blue"), (CComponent**)&m_pTextureCom[COLOR_BLUE])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHpNumber::SetUp_Components(Texture_Blue");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Number_Yellow"), TEXT("Com_Texture_Yellow"), (CComponent**)&m_pTextureCom[COLOR_YELLOW])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHpNumber::SetUp_Components(Texture_Yellow)");
		return E_FAIL;
	}

	return S_OK;
}

CHpNumber * CHpNumber::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHpNumber*	pInstance = new CHpNumber(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CHpNumber");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHpNumber::Clone(void * pArg)
{
	CHpNumber*	pInstance = new CHpNumber(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CHpNumber");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHpNumber::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
