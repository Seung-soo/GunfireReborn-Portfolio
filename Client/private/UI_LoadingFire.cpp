#include "stdafx.h"
#include "..\public\UI_LoadingFire.h"
#include "GameInstance.h"

CUI_LoadingFire::CUI_LoadingFire(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUI_LoadingFire::CUI_LoadingFire(const CUI_LoadingFire & rhs)
	: CGameObject(rhs)
	, m_fTexIndex(rhs.m_fTexIndex)
{
}

HRESULT CUI_LoadingFire::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CUI_LoadingFire::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_LoadingFire::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CUI_LoadingFire::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CUI_LoadingFire::NativeConstruct");
		return E_FAIL;
	}


	m_fSizeX = 150.f;
	m_fSizeY = 150.f;
	m_fX = 1200;
	m_fY = 600;
	m_fDepth = -10.f;
	m_fTexIndex = 6.f;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	return S_OK;
}

_int CUI_LoadingFire::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick(dTimeDelta) in CUI_LoadingFire::Tick");
		return -1;
	}


	return _int();
}

_int CUI_LoadingFire::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick(dTimeDelta) in CUI_LoadingFire::LateTick");
		return -1;
	}
	m_fTexIndex += (_float)m_pTextureCom->Get_Size() * (_float)dTimeDelta;
	if (m_fTexIndex >= (_float)m_pTextureCom->Get_Size() - 1)
		m_fTexIndex = 6.f;


	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CUI_LoadingFire::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CUI_LoadingFire::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 3);
	return S_OK;
}

HRESULT CUI_LoadingFire::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUI_LoadingFire::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUI_LoadingFire::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUI_LoadingFire::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Texture_UI_LoadingFire"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUI_LoadingFire::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CUI_LoadingFire * CUI_LoadingFire::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_LoadingFire*	pInstance = new CUI_LoadingFire(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_LoadingFire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_LoadingFire::Clone(void * pArg)
{
	CUI_LoadingFire*	pInstance = new CUI_LoadingFire(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_LoadingFire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LoadingFire::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
