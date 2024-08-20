#include "stdafx.h"
#include "..\public\Talent_Icon.h"
#include "GameInstance.h"
#include "Player_Hand.h"

CTalent_Icon::CTalent_Icon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTalent_Icon::CTalent_Icon(const CTalent_Icon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTalent_Icon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTalent_Icon::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_tInfoUI, pArg, sizeof(INFO_UI));

	m_fX = m_tInfoUI.fPositionX;
	m_fY = m_tInfoUI.fPositionY;
	m_fSizeX = 256.f;
	m_fSizeY = 256.f;
	m_fDepth = -0.5f;
	m_iTexIndex = m_tInfoUI.iTextureIndex;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	if (2 == m_iTexIndex || 3 == m_iTexIndex)
		m_iPass = 15;
	else
		m_iPass = 14;
	m_pTalents = static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_Talents();

	if (true == m_pTalents[m_iTexIndex])
		m_iPass = 3;

	return S_OK;
}

_int CTalent_Icon::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(dTimeDelta))
		return -1;
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (m_fX - m_fSizeX * 0.23f <= ptMouse.x && m_fX + m_fSizeX * 0.23f >= ptMouse.x && m_fY - m_fSizeY * 0.23f <= ptMouse.y && m_fY + m_fSizeY * 0.23f >= ptMouse.y)
	{
		m_fSizeX = 384.f;
		m_fSizeY = 384.f;
		if (true == m_bStart)
		{
			// 각성 설명 생성
			INFO_UI tInfoUI;
			if (0 <= m_iTexIndex && 4 > m_iTexIndex)
				tInfoUI.fPositionX = m_fX + 350.f;
			else
				tInfoUI.fPositionX = m_fX - 350.f;
			tInfoUI.fPositionY = m_fY + 50.f;
			tInfoUI.iTextureIndex = m_iTexIndex;

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Talent_Info"), TEXT("Prototype_GameObject_Talent_Info"), &tInfoUI)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTalent_Icon::Tick");
				return DEAD;
			}

			m_bStart = false;
		}

	}
	else if (false == m_bStart)
	{
		CGameObject* pGameObject = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Talent_Info"));
		if (nullptr != pGameObject)
			pGameObject->Set_Dead(true);

		m_fSizeX = 256.f;
		m_fSizeY = 256.f;
		// 각성 설명 싹다 릴리즈
		m_bStart = true;
	}
	

	return _int();
}

_int CTalent_Icon::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CTalent_Icon::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CTalent_Icon::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTalent_Icon::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTalent_Icon::Render(g_ProjMatrix)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CTalent_Icon::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, m_iPass);
	return S_OK;
}

HRESULT CTalent_Icon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalent_Icon::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalent_Icon::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalent_Icon::SetUp_Components(ViBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Talent_Icon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalent_Icon::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CTalent_Icon * CTalent_Icon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTalent_Icon*	pInstance = new CTalent_Icon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTalent_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTalent_Icon::Clone(void * pArg)
{
	CTalent_Icon*	pInstance = new CTalent_Icon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTalent_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalent_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
