#include "stdafx.h"
#include "..\public\NumberUI.h"
#include "GameInstance.h"

CNumberUI::CNumberUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CNumberUI::CNumberUI(const CNumberUI & rhs)
	: CGameObject(rhs)
{
}

void CNumberUI::Set_BulletNumberDesc(NUMBERDESC tNumberDesc)
{
	m_tNumberDesc = tNumberDesc;

	if (FAILED(g_pGameInstance->Release_LayerObject(LEVEL_STATIC, TEXT("Layer_BulletNumber"))))
	{
		MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CNumberUI::Set_NumberDesc");
		return;
	}

	if (FAILED(Make_Number()))
	{
		MSGBOX("Make_Number returned E_FAIL in CNumberUI::Set_NumberDesc");
		return;
	}

	m_iOldCur = 100;
	m_iOldMax = 100;
}

HRESULT CNumberUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNumberUI::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	memcpy(&m_tNumberDesc, pArg, sizeof(NUMBERDESC));
	m_iOldCur = 100;
	m_iOldMax = 100;

	m_Numbers.reserve(7);

	if (FAILED(Make_Number()))
	{
		MSGBOX("Make_Number returned E_FAIL in CNumberUI::NativeConstruct");
		return E_FAIL;
	}


	LateTick(0.016);

	return S_OK;
}

_int CNumberUI::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
	{
		for (auto& pNumber : m_Numbers)
			pNumber->Set_Dead(true);
		m_Numbers.clear();
		return DEAD;
	}

	if (0 > __super::Tick(dTimeDelta))
		return -1;

	return _int();
}

_int CNumberUI::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	switch (m_tNumberDesc.eNumberType)
	{
	case TYPE_SHIELD:
		Number_Shield();
		break;
	case TYPE_HP:
		Number_Hp();
		break;
	case TYPE_GREENBULLET:
		Number_GreenBullet();
		break;
	case TYPE_BLUEBULLET:
		Number_BlueBullet();
		break;
	case TYPE_YELLOWBULLET:
		Number_YellowBullet();
		break;
	case TYPE_WHITEBULLET:
		Number_WhiteBullet();
		break;
	case TYPE_COIN:
		Number_Coin();
		break;
	case TYPE_SKILL:
		Number_Skill();
		break;
	case TYPE_COOLTIME:
		Number_CoolTime();
		break;
	}

	m_iOldCur = *m_tNumberDesc.pCurrent;
	if (nullptr != m_tNumberDesc.pMax)
		m_iOldMax = *m_tNumberDesc.pMax;

	return _int();
}

HRESULT CNumberUI::Render()
{
	return S_OK;
}

HRESULT CNumberUI::Push_Number(_float fX, _float fY, _float fSizeX, _float fSizeY, CHpNumber::NUMCOLOR eNumColor, _tchar* pLayerTag)
{
	CHpNumber::NUMINFO tNumInfo;
	tNumInfo.fX = fX;
	tNumInfo.fY = fY;
	tNumInfo.fSizeX = fSizeX;
	tNumInfo.fSizeY = fSizeY;
	tNumInfo.eNumColor = eNumColor;
	tNumInfo.pParentX = &m_tNumberDesc.fX;
	tNumInfo.pParentY = &m_tNumberDesc.fY;
	tNumInfo.bStopUI = m_tNumberDesc.bStopUI;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI_HpNumber"), &tNumInfo)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CNumberUI::Push_Number");
		return E_FAIL;
	}

	m_Numbers.push_back(static_cast<CHpNumber*>(g_pGameInstance->Get_Back(LEVEL_STATIC, pLayerTag)));

	return S_OK;
}

HRESULT CNumberUI::Make_Number()
{

	switch (m_tNumberDesc.eNumberType)
	{
	case TYPE_SHIELD:
		if (FAILED(Push_Number(-40.5f, 0.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_ShieldNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-27.f, 0.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_ShieldNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-13.5f, 0.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_ShieldNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(0.f, 0.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_ShieldNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(13.5f, 0.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_ShieldNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(27.f, 0.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_ShieldNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(40.5f, 0.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_ShieldNumber"))))
			return E_FAIL;
		m_Numbers[3]->Set_TexIndex(10);
		break;
	case TYPE_HP:
		if (FAILED(Push_Number(-40.5f, 0.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_HpNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-27.f, 0.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_HpNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-13.5f, 0.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_HpNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(0.f, 0.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_HpNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(13.5f, 0.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_HpNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(27.f, 0.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_HpNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(40.5f, 0.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_HpNumber"))))
			return E_FAIL;
		m_Numbers[3]->Set_TexIndex(10);
		break;
	case TYPE_GREENBULLET:
		if (FAILED(Push_Number(-51.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_GREEN, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-33.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_GREEN, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-15.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_GREEN, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(0.f, 10.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(13.5f, 10.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(27.f, 10.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(40.5f, 10.f, 24.f, 26.f, CHpNumber::COLOR_GREEN, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		m_Numbers[3]->Set_TexIndex(10);
		break;
	case TYPE_BLUEBULLET:
		if (FAILED(Push_Number(-51.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_BLUE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-33.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_BLUE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-15.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_BLUE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(0.f, 10.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(13.5f, 10.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(27.f, 10.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(40.5f, 10.f, 24.f, 26.f, CHpNumber::COLOR_BLUE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		m_Numbers[3]->Set_TexIndex(10);
		break;
	case TYPE_YELLOWBULLET:
		if (FAILED(Push_Number(-51.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_YELLOW, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-33.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_YELLOW, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-15.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_YELLOW, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(0.f, 10.f, 24.f, 26.f, CHpNumber::COLOR_YELLOW, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(13.5f, 10.f, 24.f, 26.f, CHpNumber::COLOR_YELLOW, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(27.f, 10.f, 24.f, 26.f, CHpNumber::COLOR_YELLOW, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(40.5f, 10.f, 24.f, 26.f, CHpNumber::COLOR_YELLOW, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		m_Numbers[3]->Set_TexIndex(10);
		break;
	case TYPE_WHITEBULLET:
		if (FAILED(Push_Number(-33.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_WHITE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-15.5f, 0.f, 32.f, 35.f, CHpNumber::COLOR_WHITE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(0.f, 10.f, 24.f, 26.f, CHpNumber::COLOR_WHITE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(16.f, 10.f, 24.f, 26.f, CHpNumber::COLOR_WHITE, TEXT("Layer_BulletNumber"))))
			return E_FAIL;
		m_Numbers[2]->Set_TexIndex(10);
		m_Numbers[3]->Set_TexIndex(11);
		break;
	case TYPE_COIN:
		if (FAILED(Push_Number(-36.f, 0.f, 32.f, 35.f, CHpNumber::COLOR_WHITE, TEXT("Layer_CoinNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(-18.f, 0.f, 32.f, 35.f, CHpNumber::COLOR_WHITE, TEXT("Layer_CoinNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(0.f, 0.f, 32.f, 35.f, CHpNumber::COLOR_WHITE, TEXT("Layer_CoinNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(18.f, 0.f, 32.f, 35.f, CHpNumber::COLOR_WHITE, TEXT("Layer_CoinNumber"))))
			return E_FAIL;
		m_iOldCur = 1000;
		break;
	case TYPE_SKILL:
		if (FAILED(Push_Number(0.f, 0.f, 24.f, 26.f, CHpNumber::COLOR_WHITE, TEXT("Layer_SkillNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(13.5f, 0.f, 24.f, 26.f, CHpNumber::COLOR_WHITE, TEXT("Layer_SkillNumber"))))
			return E_FAIL;
		break;
	case TYPE_COOLTIME:
		if (FAILED(Push_Number(-9.f, 0.f, 32.f, 35.f, CHpNumber::COLOR_WHITE, TEXT("Layer_CoolTimeNumber"))))
			return E_FAIL;
		if (FAILED(Push_Number(9.f, 0.f, 32.f, 35.f, CHpNumber::COLOR_WHITE, TEXT("Layer_CoolTimeNumber"))))
			return E_FAIL;
		break;
	}

	return S_OK;
}

void CNumberUI::Number_Shield()
{
	if (m_iOldCur / 100 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
			m_Numbers[2]->Set_Render(false);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(-27.f, 0.f);
		}
		else if (*m_tNumberDesc.pCurrent / 100 == 0)
		{
			m_Numbers[2]->Set_Render(false);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(-13.5f, 0.f);
		}
	}
	else if (m_iOldCur / 10 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(-13.5f, 0.f);
		}
		else if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[2]->Set_Render(true);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
	}
	else if (m_iOldCur / 10 == 0)
	{
		if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
			m_Numbers[2]->Set_Render(true);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(27.f, 0.f);
		}
		else if (*m_tNumberDesc.pCurrent / 10 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
	}

	if (m_iOldMax / 100 >= 1)
	{
		if (*m_tNumberDesc.pMax / 10 == 0)
		{
			m_Numbers[5]->Set_Render(false);
			m_Numbers[6]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pMax / 100 == 0)
		{
			m_Numbers[6]->Set_Render(false);
		}
	}
	else if (m_iOldMax / 10 >= 1)
	{
		if (*m_tNumberDesc.pMax / 10 == 0)
		{
			m_Numbers[5]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pMax / 100 >= 1)
		{
			m_Numbers[6]->Set_Render(true);
		}
	}
	else if (m_iOldMax / 10 == 0)
	{
		if (*m_tNumberDesc.pMax / 100 >= 1)
		{
			m_Numbers[5]->Set_Render(true);
			m_Numbers[6]->Set_Render(true);
		}
		else if (*m_tNumberDesc.pMax / 10 >= 1)
		{
			m_Numbers[5]->Set_Render(true);
		}
	}


	if (*m_tNumberDesc.pCurrent / 100 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 100);
		m_Numbers[1]->Set_TexIndex((*m_tNumberDesc.pCurrent % 100) / 10);
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 10);
		m_Numbers[1]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 == 0)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}

	if (*m_tNumberDesc.pMax / 100 >= 1)
	{
		m_Numbers[4]->Set_TexIndex(*m_tNumberDesc.pMax / 100);
		m_Numbers[5]->Set_TexIndex((*m_tNumberDesc.pMax % 100) / 10);
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
	else if (*m_tNumberDesc.pMax / 10 >= 1)
	{
		m_Numbers[4]->Set_TexIndex(*m_tNumberDesc.pMax / 10);
		m_Numbers[5]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
	else if (*m_tNumberDesc.pMax / 10 == 0)
	{
		m_Numbers[4]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
}

void CNumberUI::Number_Hp()
{
	if (m_iOldCur / 100 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
			m_Numbers[2]->Set_Render(false);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(-27.f, 0.f);
		}
		else if (*m_tNumberDesc.pCurrent / 100 == 0)
		{
			m_Numbers[2]->Set_Render(false);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(-13.5f, 0.f);
		}
	}
	else if (m_iOldCur / 10 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(-13.5f, 0.f);
		}
		else if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[2]->Set_Render(true);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
	}
	else if (m_iOldCur / 10 == 0)
	{
		if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
			m_Numbers[2]->Set_Render(true);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(27.f, 0.f);
		}
		else if (*m_tNumberDesc.pCurrent / 10 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
			for (size_t i = 3; i < m_Numbers.size(); i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
	}

	if (m_iOldMax / 100 >= 1)
	{
		if (*m_tNumberDesc.pMax / 10 == 0)
		{
			m_Numbers[5]->Set_Render(false);
			m_Numbers[6]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pMax / 100 == 0)
		{
			m_Numbers[6]->Set_Render(false);
		}
	}
	else if (m_iOldMax / 10 >= 1)
	{
		if (*m_tNumberDesc.pMax / 10 == 0)
		{
			m_Numbers[5]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pMax / 100 >= 1)
		{
			m_Numbers[6]->Set_Render(true);
		}
	}
	else if (m_iOldMax / 10 == 0)
	{
		if (*m_tNumberDesc.pMax / 100 >= 1)
		{
			m_Numbers[5]->Set_Render(true);
			m_Numbers[6]->Set_Render(true);
		}
		else if (*m_tNumberDesc.pMax / 10 >= 1)
		{
			m_Numbers[5]->Set_Render(true);
		}
	}


	if (*m_tNumberDesc.pCurrent / 100 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 100);
		m_Numbers[1]->Set_TexIndex((*m_tNumberDesc.pCurrent % 100) / 10);
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 10);
		m_Numbers[1]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 == 0)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}

	if (*m_tNumberDesc.pMax / 100 >= 1)
	{
		m_Numbers[4]->Set_TexIndex(*m_tNumberDesc.pMax / 100);
		m_Numbers[5]->Set_TexIndex((*m_tNumberDesc.pMax % 100) / 10);
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
	else if (*m_tNumberDesc.pMax / 10 >= 1)
	{
		m_Numbers[4]->Set_TexIndex(*m_tNumberDesc.pMax / 10);
		m_Numbers[5]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
	else if (*m_tNumberDesc.pMax / 10 == 0)
	{
		m_Numbers[4]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
}

void CNumberUI::Number_WhiteBullet()
{
	if (m_iOldCur / 10 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[0]->Set_Render(false);
		}
	}
	else if (m_iOldCur / 10 == 0)
	{
		if (*m_tNumberDesc.pCurrent / 10 >= 1)
		{
			m_Numbers[0]->Set_Render(true);
		}
	}

	if (*m_tNumberDesc.pCurrent / 10 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 10);
		m_Numbers[1]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 == 0)
	{
		m_Numbers[1]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
}

void CNumberUI::Number_GreenBullet()
{
	if (m_iOldCur / 100 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[0]->Set_Render(false);
			m_Numbers[1]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 100 == 0)
		{
			m_Numbers[0]->Set_Render(false);
		}
	}
	else if (m_iOldCur / 10 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[0]->Set_Render(true);
		}
	}
	else if (m_iOldCur / 10 == 0)
	{
		if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[0]->Set_Render(true);
			m_Numbers[1]->Set_Render(true);
		}
		else if (*m_tNumberDesc.pCurrent / 10 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
		}
	}

	if (m_iOldMax / 100 >= 1)
	{
		if (*m_tNumberDesc.pMax / 10 == 0)
		{
			m_Numbers[4]->Set_Render(false);
			m_Numbers[5]->Set_Render(false);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(27.f, 0.f);
		}
		else if (*m_tNumberDesc.pMax / 100 == 0)
		{
			m_Numbers[4]->Set_Render(false);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
	}
	else if (m_iOldMax / 10 >= 1)
	{
		if (*m_tNumberDesc.pMax / 10 == 0)
		{
			m_Numbers[5]->Set_Render(false);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
		else if (*m_tNumberDesc.pMax / 100 >= 1)
		{
			m_Numbers[4]->Set_Render(true);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(-13.5f, 0.f);
		}
	}
	else if (m_iOldMax / 10 == 0)
	{
		if (*m_tNumberDesc.pMax / 100 >= 1)
		{
			m_Numbers[4]->Set_Render(true);
			m_Numbers[5]->Set_Render(true);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(27.f, 0.f);
		}
		else if (*m_tNumberDesc.pMax / 10 >= 1)
		{
			m_Numbers[5]->Set_Render(true);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
	}


	if (*m_tNumberDesc.pCurrent / 100 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 100);
		m_Numbers[1]->Set_TexIndex((*m_tNumberDesc.pCurrent % 100) / 10);
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 >= 1)
	{
		m_Numbers[1]->Set_TexIndex(*m_tNumberDesc.pCurrent / 10);
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 == 0)
	{
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}

	if (*m_tNumberDesc.pMax / 100 >= 1)
	{
		m_Numbers[4]->Set_TexIndex(*m_tNumberDesc.pMax / 100);
		m_Numbers[5]->Set_TexIndex((*m_tNumberDesc.pMax % 100) / 10);
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
	else if (*m_tNumberDesc.pMax / 10 >= 1)
	{
		m_Numbers[5]->Set_TexIndex(*m_tNumberDesc.pMax / 10);
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
	else if (*m_tNumberDesc.pMax / 10 == 0)
	{
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
}

void CNumberUI::Number_BlueBullet()
{
	if (m_iOldCur / 100 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[0]->Set_Render(false);
			m_Numbers[1]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 100 == 0)
		{
			m_Numbers[0]->Set_Render(false);
		}
	}
	else if (m_iOldCur / 10 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[0]->Set_Render(true);
		}
	}
	else if (m_iOldCur / 10 == 0)
	{
		if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[0]->Set_Render(true);
			m_Numbers[1]->Set_Render(true);
		}
		else if (*m_tNumberDesc.pCurrent / 10 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
		}
	}

	if (m_iOldMax / 100 >= 1)
	{
		if (*m_tNumberDesc.pMax / 10 == 0)
		{
			m_Numbers[4]->Set_Render(false);
			m_Numbers[5]->Set_Render(false);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(27.f, 0.f);
		}
		else if (*m_tNumberDesc.pMax / 100 == 0)
		{
			m_Numbers[4]->Set_Render(false);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
	}
	else if (m_iOldMax / 10 >= 1)
	{
		if (*m_tNumberDesc.pMax / 10 == 0)
		{
			m_Numbers[5]->Set_Render(false);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
		else if (*m_tNumberDesc.pMax / 100 >= 1)
		{
			m_Numbers[4]->Set_Render(true);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(-13.5f, 0.f);
		}
	}
	else if (m_iOldMax / 10 == 0)
	{
		if (*m_tNumberDesc.pMax / 100 >= 1)
		{
			m_Numbers[4]->Set_Render(true);
			m_Numbers[5]->Set_Render(true);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(27.f, 0.f);
		}
		else if (*m_tNumberDesc.pMax / 10 >= 1)
		{
			m_Numbers[5]->Set_Render(true);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
	}


	if (*m_tNumberDesc.pCurrent / 100 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 100);
		m_Numbers[1]->Set_TexIndex((*m_tNumberDesc.pCurrent % 100) / 10);
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 >= 1)
	{
		m_Numbers[1]->Set_TexIndex(*m_tNumberDesc.pCurrent / 10);
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 == 0)
	{
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}

	if (*m_tNumberDesc.pMax / 100 >= 1)
	{
		m_Numbers[4]->Set_TexIndex(*m_tNumberDesc.pMax / 100);
		m_Numbers[5]->Set_TexIndex((*m_tNumberDesc.pMax % 100) / 10);
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
	else if (*m_tNumberDesc.pMax / 10 >= 1)
	{
		m_Numbers[5]->Set_TexIndex(*m_tNumberDesc.pMax / 10);
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
	else if (*m_tNumberDesc.pMax / 10 == 0)
	{
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
}

void CNumberUI::Number_YellowBullet()
{
	if (m_iOldCur / 100 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[0]->Set_Render(false);
			m_Numbers[1]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 100 == 0)
		{
			m_Numbers[0]->Set_Render(false);
		}
	}
	else if (m_iOldCur / 10 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[0]->Set_Render(true);
		}
	}
	else if (m_iOldCur / 10 == 0)
	{
		if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[0]->Set_Render(true);
			m_Numbers[1]->Set_Render(true);
		}
		else if (*m_tNumberDesc.pCurrent / 10 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
		}
	}

	if (m_iOldMax / 100 >= 1)
	{
		if (*m_tNumberDesc.pMax / 10 == 0)
		{
			m_Numbers[4]->Set_Render(false);
			m_Numbers[5]->Set_Render(false);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(27.f, 0.f);
		}
		else if (*m_tNumberDesc.pMax / 100 == 0)
		{
			m_Numbers[4]->Set_Render(false);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
	}
	else if (m_iOldMax / 10 >= 1)
	{
		if (*m_tNumberDesc.pMax / 10 == 0)
		{
			m_Numbers[5]->Set_Render(false);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
		else if (*m_tNumberDesc.pMax / 100 >= 1)
		{
			m_Numbers[4]->Set_Render(true);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(-13.5f, 0.f);
		}
	}
	else if (m_iOldMax / 10 == 0)
	{
		if (*m_tNumberDesc.pMax / 100 >= 1)
		{
			m_Numbers[4]->Set_Render(true);
			m_Numbers[5]->Set_Render(true);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(27.f, 0.f);
		}
		else if (*m_tNumberDesc.pMax / 10 >= 1)
		{
			m_Numbers[5]->Set_Render(true);
			for (size_t i = 0; i < 4; i++)
				m_Numbers[i]->Set_Position(13.5f, 0.f);
		}
	}


	if (*m_tNumberDesc.pCurrent / 100 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 100);
		m_Numbers[1]->Set_TexIndex((*m_tNumberDesc.pCurrent % 100) / 10);
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 >= 1)
	{
		m_Numbers[1]->Set_TexIndex(*m_tNumberDesc.pCurrent / 10);
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 == 0)
	{
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}

	if (*m_tNumberDesc.pMax / 100 >= 1)
	{
		m_Numbers[4]->Set_TexIndex(*m_tNumberDesc.pMax / 100);
		m_Numbers[5]->Set_TexIndex((*m_tNumberDesc.pMax % 100) / 10);
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
	else if (*m_tNumberDesc.pMax / 10 >= 1)
	{
		m_Numbers[5]->Set_TexIndex(*m_tNumberDesc.pMax / 10);
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
	else if (*m_tNumberDesc.pMax / 10 == 0)
	{
		m_Numbers[6]->Set_TexIndex(*m_tNumberDesc.pMax % 10);
	}
}

void CNumberUI::Number_Coin()
{
	if (m_iOldCur / 1000 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
			m_Numbers[2]->Set_Render(false);
			m_Numbers[3]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 100 == 0)
		{
			m_Numbers[2]->Set_Render(false);
			m_Numbers[3]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 1000 == 0)
		{
			m_Numbers[3]->Set_Render(false);
		}
	}
	else if (m_iOldCur / 100 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
			m_Numbers[2]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 100 == 0)
		{
			m_Numbers[2]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 1000 >= 1)
		{
			m_Numbers[3]->Set_Render(true);
		}
	}
	else if (m_iOldCur / 10 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
		}
		else if (*m_tNumberDesc.pCurrent / 1000 >= 1)
		{
			m_Numbers[2]->Set_Render(true);
			m_Numbers[3]->Set_Render(true);
		}
		else if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[2]->Set_Render(true);
		}
	}
	else if (m_iOldCur / 10 == 0)
	{
		if (*m_tNumberDesc.pCurrent / 1000 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
			m_Numbers[2]->Set_Render(true);
			m_Numbers[3]->Set_Render(true);
		}
		else if (*m_tNumberDesc.pCurrent / 100 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
			m_Numbers[2]->Set_Render(true);
		}
		else if (*m_tNumberDesc.pCurrent / 10 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
		}
	}

	if (*m_tNumberDesc.pCurrent / 1000 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 1000);
		m_Numbers[1]->Set_TexIndex((*m_tNumberDesc.pCurrent % 1000) / 100);
		m_Numbers[2]->Set_TexIndex((*m_tNumberDesc.pCurrent % 100) / 10);
		m_Numbers[3]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 100 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 100);
		m_Numbers[1]->Set_TexIndex((*m_tNumberDesc.pCurrent % 100) / 10);
		m_Numbers[2]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 10);
		m_Numbers[1]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 == 0)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
}

void CNumberUI::Number_Skill()
{
	if (m_iOldCur / 10 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
		}
	}
	else if (m_iOldCur / 10 == 0)
	{
		if (*m_tNumberDesc.pCurrent / 10 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
		}
	}

	if (*m_tNumberDesc.pCurrent / 10 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 10);
		m_Numbers[1]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 == 0)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
}

void CNumberUI::Number_CoolTime()
{
	if (m_iOldCur / 10 >= 1)
	{
		if (*m_tNumberDesc.pCurrent / 10 == 0)
		{
			m_Numbers[1]->Set_Render(false);
			m_Numbers[0]->Set_Position(9.f, 0.f);
		}
	}
	else if (m_iOldCur / 10 == 0)
	{
		if (*m_tNumberDesc.pCurrent / 10 >= 1)
		{
			m_Numbers[1]->Set_Render(true);
			m_Numbers[0]->Set_Position(-9.f, 0.f);
		}
	}

	if (*m_tNumberDesc.pCurrent / 10 >= 1)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent / 10);
		m_Numbers[1]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}
	else if (*m_tNumberDesc.pCurrent / 10 == 0)
	{
		m_Numbers[0]->Set_TexIndex(*m_tNumberDesc.pCurrent % 10);
	}

	if (0.f >= *m_tNumberDesc.pCurrent)
		m_bDead = true;
}

CNumberUI * CNumberUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNumberUI*	pInstance = new CNumberUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CNumberUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CNumberUI::Clone(void * pArg)
{
	CNumberUI*	pInstance = new CNumberUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CNumberUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNumberUI::Free()
{
	__super::Free();
}
