#include "stdafx.h"
#include "..\public\Talent.h"
#include "GameInstance.h"
#include "Player_Hand.h"

CTalent::CTalent(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTalent::CTalent(const CTalent & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTalent::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTalent::NativeConstruct(void * pArg)
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
	m_fSizeX = 300.f;
	m_fSizeY = 300.f;
	m_fDepth = 3.f;
	m_iTexIndex = m_tInfoUI.iTextureIndex;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	return S_OK;
}

_int CTalent::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(dTimeDelta))
		return -1;

	if (true == m_bSelect)
	{
		m_fPickDelay += (_float)dTimeDelta;
		if (0.5f < m_fPickDelay)
		{
			if (true == m_bStart)
			{
				g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_TalentBack"))->Set_Dead(true);
				m_bPick = true;
				m_bStart = false;
				if (2 == m_iTexIndex)
					static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_MaxSmoke();
			}
			if (500.f > m_fSizeX)
			{
				m_fSizeX += 10.f;
				m_fSizeY += 10.f;
			}
			if (500.f <= m_fSizeX)
			{
				m_fSizeX = 500.f;
				m_fSizeY = 500.f;
				m_bDead = true;
				m_pRendererCom->Set_DontRenderUI(false);
				g_bCameraAction = false;
				g_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT2);
				g_pGameInstance->LetsPlaySound(TEXT("Talent.mp3"), CSoundMgr::SYSTEM_EFFECT2, 0.7f);
			}
		}
	}
	else
	{
		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		if (m_fX - m_fSizeX * 0.33f <= ptMouse.x && m_fX + m_fSizeX * 0.33f >= ptMouse.x && m_fY - m_fSizeY * 0.5f <= ptMouse.y && m_fY + m_fSizeY * 0.5f >= ptMouse.y)
		{
			m_fSizeX = 400.f;
			m_fSizeY = 400.f;
			if (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
			{
				if (300.f < m_fSizeX)
				{
					m_fSizeX -= 20.f;
					m_fSizeY -= 20.f;
				}
				if (300.f > m_fSizeX)
				{
					m_fSizeX = 300.f;
					m_fSizeY = 300.f;
				}

			}
			else if (true == g_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
			{
				m_bSelect = true;
			}
		}
		else
		{
			m_fSizeX = 320.f;
			m_fSizeY = 320.f;
		}
	}
	


	return _int();
}

_int CTalent::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STOPUI, this);

	return _int();
}

HRESULT CTalent::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CTalent::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTalent::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTalent::Render(g_ProjMatrix)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CTalent::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 3);
	return S_OK;
}

HRESULT CTalent::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalent::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalent::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalent::SetUp_Components(ViBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Talent"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalent::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CTalent * CTalent::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTalent*	pInstance = new CTalent(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTalent");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTalent::Clone(void * pArg)
{
	CTalent*	pInstance = new CTalent(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTalent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalent::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
