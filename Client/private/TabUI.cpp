#include "stdafx.h"
#include "..\public\TabUI.h"
#include "GameInstance.h"
#include "Player_Hand.h"

CTabUI::CTabUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTabUI::CTabUI(const CTabUI & rhs)
	: CGameObject(rhs)
	, m_iTexIndex(rhs.m_iTexIndex)
{
}

HRESULT CTabUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTabUI::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pInfoUI = (INFO_UI*)pArg;


	m_fSizeX = (*(INFO_UI*)pArg).fScaleX;
	m_fSizeY = (*(INFO_UI*)pArg).fScaleY;
	m_fX = (*(INFO_UI*)pArg).fPositionX;
	m_fY = (*(INFO_UI*)pArg).fPositionY;
	m_iTexIndex = (*(INFO_UI*)pArg).iTextureIndex;
	m_fDepth = (*(INFO_UI*)pArg).fDepth;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	if (FAILED(Ready_Layer_WeaponSlot()))
	{
		MSGBOX("Ready_Layer_WeaponSlot returned E_FAIL in CTabUI::NativeConstruct");
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_TabBar()))
	{
		MSGBOX("Ready_Layer_TabBar returned E_FAIL in CTabUI::NativeConstruct");
		return E_FAIL;
	}


	return S_OK;
}

_int CTabUI::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
	{
		if (52 == m_iTexIndex)
		{
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Talent_Icon"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Skill_Icon"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}
			if (nullptr != m_pGlow)
			{
				m_pGlow->Set_Dead(true);
				m_pGlow = nullptr;
			}
			if (nullptr != g_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_UI_PlayerInfo")) && 0 < g_pGameInstance->Get_Size(LEVEL_STATIC, TEXT("Layer_UI_PlayerInfo")))
				if (FAILED(g_pGameInstance->Release_LayerObject(LEVEL_STATIC, TEXT("Layer_UI_PlayerInfo"))))
				{
					MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CTabUI::Tick");
					return DEAD;
				}
			if (nullptr != g_pGameInstance->Find_Layer(g_eCurrentLevel, TEXT("Layer_Skill_Info")) && 0 < g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_Skill_Info")))
				if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Skill_Info"))))
				{
					MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CTabUI::Tick");
					return DEAD;
				}
			if (nullptr != g_pGameInstance->Find_Layer(g_eCurrentLevel, TEXT("Layer_Talent_Info")) && 0 < g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_Talent_Info")))
				if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Talent_Info"))))
				{
					MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CTabUI::Tick");
					return DEAD;
				}
		}
		else
		{
			if (nullptr != m_pGlow)
			{
				m_pGlow->Set_Dead(true);
				m_pGlow = nullptr;
			}
			if (nullptr != g_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_UI_PlayerInfo")) && 0 < g_pGameInstance->Get_Size(LEVEL_STATIC, TEXT("Layer_UI_PlayerInfo")))
				if (FAILED(g_pGameInstance->Release_LayerObject(LEVEL_STATIC, TEXT("Layer_UI_PlayerInfo"))))
				{
					MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CTabUI::Tick");
					return DEAD;
				}
			if (nullptr != g_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_UI_WeaponInfo")) && 0 < g_pGameInstance->Get_Size(LEVEL_STATIC, TEXT("Layer_UI_WeaponInfo")))
				if (FAILED(g_pGameInstance->Release_LayerObject(LEVEL_STATIC, TEXT("Layer_UI_WeaponInfo"))))
				{
					MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CTabUI::Tick");
					return DEAD;
				}
		}
		return DEAD;
	}
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (768.f - 64.f <= ptMouse.x && 768.f + 64.f >= ptMouse.x && 100.f - 32.f <= ptMouse.y && 100.f + 32.f >= ptMouse.y)
	{
		if (53 == m_iTexIndex && true == g_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
		{
			m_iTexIndex = 52;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (nullptr != g_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_UI_WeaponInfo")) && 0 < g_pGameInstance->Get_Size(LEVEL_STATIC, TEXT("Layer_UI_WeaponInfo")))
				if (FAILED(g_pGameInstance->Release_LayerObject(LEVEL_STATIC, TEXT("Layer_UI_WeaponInfo"))))
				{
					MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CTabUI::Tick");
					return DEAD;
				}

			INFO_UI tInfoUI;
			tInfoUI.fPositionX = 190.f;
			tInfoUI.fPositionY = 360.f;
			tInfoUI.iTextureIndex = 0;
			if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Talent_Icon"), TEXT("Prototype_GameObject_Talent_Icon"), &tInfoUI)))
			{
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}
			tInfoUI.fPositionX = 190.f;
			tInfoUI.fPositionY = 530.f;
			tInfoUI.iTextureIndex = 1;
			if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Talent_Icon"), TEXT("Prototype_GameObject_Talent_Icon"), &tInfoUI)))
			{
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}
			tInfoUI.fPositionX = 570.f;
			tInfoUI.fPositionY = 360.f;
			tInfoUI.iTextureIndex = 2;
			if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Talent_Icon"), TEXT("Prototype_GameObject_Talent_Icon"), &tInfoUI)))
			{
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}
			tInfoUI.fPositionX = 570.f;
			tInfoUI.fPositionY = 530.f;
			tInfoUI.iTextureIndex = 3;
			if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Talent_Icon"), TEXT("Prototype_GameObject_Talent_Icon"), &tInfoUI)))
			{
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}
			tInfoUI.fPositionX = 960.f;
			tInfoUI.fPositionY = 360.f;
			tInfoUI.iTextureIndex = 4;
			if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Talent_Icon"), TEXT("Prototype_GameObject_Talent_Icon"), &tInfoUI)))
			{
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}
			tInfoUI.fPositionX = 960.f;
			tInfoUI.fPositionY = 530.f;
			tInfoUI.iTextureIndex = 5;
			if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Talent_Icon"), TEXT("Prototype_GameObject_Talent_Icon"), &tInfoUI)))
			{
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}

			tInfoUI.fPositionX = 1215.f;
			tInfoUI.fPositionY = 290.f;
			tInfoUI.fScaleX = 150.f;
			tInfoUI.fScaleY = 150.f;
			tInfoUI.iTextureIndex = 92;
			if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Skill_Icon"), TEXT("Prototype_GameObject_Skill_Icon"), &tInfoUI)))
			{
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}

			tInfoUI.fPositionX = 1215.f;
			tInfoUI.fPositionY = 430.f;
			tInfoUI.fScaleX = 75.f;
			tInfoUI.fScaleY = 75.f;
			tInfoUI.iTextureIndex = 27;
			if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Skill_Icon"), TEXT("Prototype_GameObject_Skill_Icon"), &tInfoUI)))
			{
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}

			if (nullptr != m_pGlow)
			{
				m_pGlow->Set_Dead(true);
				m_pGlow = nullptr;
			}

			tInfoUI.fPositionX = 768.f;
			tInfoUI.fPositionY = 100.f;
			tInfoUI.fScaleX = 128.f;
			tInfoUI.fScaleY = 128.f;
			tInfoUI.fDepth = -0.1f;
			tInfoUI.iTextureIndex = 82;

			if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}
			m_pGlow = pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_UI"));

			RELEASE_INSTANCE(CGameInstance);

		}

	}
	else if (512.f - 64.f <= ptMouse.x && 512.f + 64.f >= ptMouse.x && 100.f - 32.f <= ptMouse.y && 100.f + 32.f >= ptMouse.y)
	{
		if (52 == m_iTexIndex && g_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
		{
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Talent_Icon"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Skill_Icon"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}

			if (nullptr != m_pGlow)
			{
				m_pGlow->Set_Dead(true);
				m_pGlow = nullptr;
			}

			INFO_UI tInfoUI;

			tInfoUI.fPositionX = 512.f;
			tInfoUI.fPositionY = 100.f;
			tInfoUI.fScaleX = 128.f;
			tInfoUI.fScaleY = 128.f;
			tInfoUI.fDepth = -0.1f;
			tInfoUI.iTextureIndex = 82;

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
				return DEAD;
			}
			m_pGlow = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_UI"));
			m_iTexIndex = 53;

			if (FAILED(Ready_Layer_WeaponSlot()))
			{
				MSGBOX("Ready_Layer_WeaponSlot returned E_FAIL in CTabUI::Tick");
				return E_FAIL;
			}

		}
	}


	return _int();
}

_int CTabUI::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CTabUI::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CTabUI::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 3);
	return S_OK;
}

HRESULT CTabUI::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTabUI::Ready_Layer_WeaponSlot()
{
	INFO_UI tInfoUI1;
	ZeroMemory(&tInfoUI1, sizeof(INFO_UI));

	tInfoUI1.fScaleX = 370.f;
	tInfoUI1.fScaleY = 270.f;
	tInfoUI1.fPositionX = 246.f;
	tInfoUI1.fPositionY = 360.f;
	tInfoUI1.fDepth = -0.1f;
	_uint iWeapon1 = static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_Slot1();
	switch (iWeapon1)
	{
	case CPlayer_Hand::WEAPON_TESLA:
		tInfoUI1.iTextureIndex = 61;
		break;
	case CPlayer_Hand::WEAPON_TIGERCANNON:
		tInfoUI1.fScaleY = 195.f;
		tInfoUI1.fPositionY = 320.f;
		tInfoUI1.iTextureIndex = 63;
		break;
	case CPlayer_Hand::WEAPON_THUNDERCLAPGLOVES:
		tInfoUI1.iTextureIndex = 65;
		break;
	case CPlayer_Hand::WEAPON_FIREDRAGON:
		tInfoUI1.iTextureIndex = 67;
		break;
	case CPlayer_Hand::WEAPON_PIERCINGFRAME:
		tInfoUI1.iTextureIndex = 69;
		break;
	case CPlayer_Hand::WEAPON_SWORD:
		tInfoUI1.iTextureIndex = 71;
		break;
	}
	if (CPlayer_Hand::WEAPON_END != iWeapon1)
	{
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_WeaponInfo"), TEXT("Prototype_GameObject_UI"), &tInfoUI1)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Ready_Layer_WeaponSlot(Weapon1)");
			return E_FAIL;
		}
	}


	INFO_UI tInfoUI2;
	ZeroMemory(&tInfoUI2, sizeof(INFO_UI));

	tInfoUI2.fScaleX = 370.f;
	tInfoUI2.fScaleY = 270.f;
	tInfoUI2.fPositionX = 630.f;
	tInfoUI2.fPositionY = 360.f;
	tInfoUI2.fDepth = -0.1f;
	_uint iWeapon2 = static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_Slot2();

	switch (iWeapon2)
	{
	case CPlayer_Hand::WEAPON_TESLA:
		tInfoUI2.iTextureIndex = 62;
		break;
	case CPlayer_Hand::WEAPON_TIGERCANNON:
		tInfoUI2.fScaleY = 195.f;
		tInfoUI2.fPositionY = 320.f;
		tInfoUI2.iTextureIndex = 64;
		break;
	case CPlayer_Hand::WEAPON_THUNDERCLAPGLOVES:
		tInfoUI2.iTextureIndex = 66;
		break;
	case CPlayer_Hand::WEAPON_FIREDRAGON:
		tInfoUI2.iTextureIndex = 68;
		break;
	case CPlayer_Hand::WEAPON_PIERCINGFRAME:
		tInfoUI2.iTextureIndex = 70;
		break;
	case CPlayer_Hand::WEAPON_SWORD:
		tInfoUI2.iTextureIndex = 72;
		break;
	}
	if (CPlayer_Hand::WEAPON_END != iWeapon2)
	{
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_WeaponInfo"), TEXT("Prototype_GameObject_UI"), &tInfoUI2)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Ready_Layer_WeaponSlot(Weapon2)");
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CTabUI::Ready_Layer_TabBar()
{
	INFO_UI tInfoUI;
	tInfoUI.fPositionX = 512.f;
	tInfoUI.fPositionY = 100.f;
	tInfoUI.fScaleX = 128.f;
	tInfoUI.fScaleY = 128.f;
	tInfoUI.fDepth = -0.5f;
	tInfoUI.iTextureIndex = 83;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_PlayerInfo"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Ready_Layer_TabBar(¹«±â)");
		return E_FAIL;
	}

	tInfoUI.fPositionX = 768.f;
	tInfoUI.iTextureIndex = 84;
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_PlayerInfo"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Ready_Layer_TabBar(¿µ¿õ)");
		return E_FAIL;
	}

	tInfoUI.fPositionX = 512.f;
	tInfoUI.fPositionY = 100.f;
	tInfoUI.fScaleX = 128.f;
	tInfoUI.fScaleY = 128.f;
	tInfoUI.fDepth = -0.1f;
	tInfoUI.iTextureIndex = 82;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTabUI::Tick");
		return DEAD;
	}
	m_pGlow = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_UI"));

	return S_OK;
}

CTabUI * CTabUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTabUI*	pInstance = new CTabUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTabUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTabUI::Clone(void * pArg)
{
	CTabUI*	pInstance = new CTabUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTabUI");
		Safe_Release(pInstance);
	}
	m_pInfoUI = (INFO_UI*)pArg;
	return pInstance;
}

void CTabUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
