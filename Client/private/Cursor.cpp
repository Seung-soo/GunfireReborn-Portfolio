#include "stdafx.h"
#include "..\public\Cursor.h"
#include "GameInstance.h"

CCursor::CCursor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CCursor::CCursor(const CCursor & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCursor::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCursor::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;
	
	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 30.f;
	m_fSizeY = 30.f;

	m_fDepth = -1.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	return S_OK;
}

_int CCursor::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	//RECT a;
	//GetClientRect(g_hWnd, &a);
	m_fX = (_float)ptMouse.x + m_fSizeX * 0.5f;
	m_fY = (_float)ptMouse.y + m_fSizeY * 0.5f;

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STOPUI, this);

	return _int();
}

_int CCursor::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;



	return _int();
}

HRESULT CCursor::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CCursor::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 3);

	return S_OK;
}

HRESULT CCursor::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CCursor * CCursor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCursor*	pInstance = new CCursor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CCursor");
		Safe_Release(pInstance);
	}
	if (FAILED(pInstance->NativeConstruct(nullptr)))
	{
		MSGBOX("Failed to Created CCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCursor::Clone(void * pArg)
{
	CCursor*	pInstance = new CCursor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCursor::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
