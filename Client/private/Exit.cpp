#include "stdafx.h"
#include "..\public\Exit.h"
#include "GameInstance.h"

CExit::CExit(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CExit::CExit(const CExit & rhs)
	: CGameObject(rhs)
{
}

HRESULT CExit::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExit::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fX = 256.f;
	m_fY = 630.f;
	m_fSizeX = 256.f;
	m_fSizeY = 256.f;
	m_fDepth = -0.5f;
	m_iTexIndex = 0;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));


	return S_OK;
}

_int CExit::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(dTimeDelta))
		return -1;
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (m_fX - m_fSizeX * 0.5f <= ptMouse.x && m_fX + m_fSizeX * 0.5f >= ptMouse.x && m_fY - m_fSizeY * 0.1f <= ptMouse.y && m_fY + m_fSizeY * 0.1f >= ptMouse.y)
	{
		m_fSizeX = 300.f;
		m_fSizeY = 300.f;
	}
	else
	{
		m_fSizeX = 256.f;
		m_fSizeY = 256.f;
	}


	return _int();
}

_int CExit::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CExit::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CExit::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CExit::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CExit::Render(g_ProjMatrix)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CExit::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 3);
	return S_OK;
}

HRESULT CExit::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CExit::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CExit::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CExit::SetUp_Components(ViBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_MAINMANU, TEXT("Prototype_Component_Texture_Exit"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CExit::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CExit * CExit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CExit*	pInstance = new CExit(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CExit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CExit::Clone(void * pArg)
{
	CExit*	pInstance = new CExit(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CExit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExit::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
