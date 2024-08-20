#include "stdafx.h"
#include "..\public\Skill_Icon.h"
#include "GameInstance.h"
#include "Player_Hand.h"

CSkill_Icon::CSkill_Icon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSkill_Icon::CSkill_Icon(const CSkill_Icon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkill_Icon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Icon::NativeConstruct(void * pArg)
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
	m_fSizeX = m_tInfoUI.fScaleX;
	m_fSizeY = m_tInfoUI.fScaleY;
	m_fDepth = -0.7f;
	m_iTexIndex = m_tInfoUI.iTextureIndex;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));
	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	m_pPlayer = static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")));
	return S_OK;
}

_int CSkill_Icon::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(dTimeDelta))
		return -1;

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (m_fX - 37.5f <= ptMouse.x && m_fX + 37.5f >= ptMouse.x && m_fY - 37.5f <= ptMouse.y && m_fY + 37.5f >= ptMouse.y)
	{
		if (true == m_bStart)
		{
			// 스킬 설명 생성
			INFO_UI tInfoUI;

			tInfoUI.fPositionX = m_fX - 300.f;
			tInfoUI.fPositionY = m_fY + 30.f;
			if (15 == m_iTexIndex)
			{
				if (true == m_pPlayer->Get_Talents()[4])
					tInfoUI.iTextureIndex = 1;
				else
					tInfoUI.iTextureIndex = 0;
			}
			else if (27 == m_iTexIndex)
			{
				if (true == m_pPlayer->Get_Talents()[2])
					tInfoUI.iTextureIndex = 3;
				else
					tInfoUI.iTextureIndex = 2;
			}

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Skill_Info"), TEXT("Prototype_GameObject_Skill_Info"), &tInfoUI)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CSkill_Icon::Tick");
				return DEAD;
			}

			tInfoUI.fPositionX = m_fX;
			tInfoUI.fPositionY = m_fY;
			tInfoUI.fScaleX = 150.f;
			tInfoUI.fScaleY = 150.f;
			tInfoUI.fDepth = -0.1f;
			tInfoUI.iTextureIndex = 82;

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CSkill_Icon::Tick");
				return DEAD;
			}
			m_pGlow = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_UI"));

			m_bStart = false;
		}

	}
	else if (false == m_bStart)
	{
		CGameObject* pGameObject = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Skill_Info"));
		if (nullptr != pGameObject)
			pGameObject->Set_Dead(true);

		if (nullptr != m_pGlow)
			m_pGlow->Set_Dead(true);

		m_bStart = true;
	}

	return _int();
}

_int CSkill_Icon::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;




	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CSkill_Icon::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CSkill_Icon::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSkill_Icon::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSkill_Icon::Render(g_ProjMatrix)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CSkill_Icon::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 16);
	return S_OK;
}

HRESULT CSkill_Icon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSkill_Icon::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSkill_Icon::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSkill_Icon::SetUp_Components(ViBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSkill_Icon::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CSkill_Icon * CSkill_Icon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSkill_Icon*	pInstance = new CSkill_Icon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CSkill_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkill_Icon::Clone(void * pArg)
{
	CSkill_Icon*	pInstance = new CSkill_Icon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CSkill_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Icon::Free()
{
	__super::Free();
	if (nullptr != m_pGlow)
		m_pGlow->Set_Dead(true);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
