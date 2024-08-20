#include "stdafx.h"
#include "..\public\BossHp.h"
#include "GameInstance.h"

CBossHp::CBossHp(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBossHp::CBossHp(const CBossHp & rhs)
	: CGameObject(rhs)
	, m_iTexIndex(rhs.m_iTexIndex)
{
}

void CBossHp::Set_Type(HP_TYPE eHp_Type)
{
	m_eHp_Type = eHp_Type;
}

void CBossHp::Set_Hp(_uint iMax, _uint iCurrent)
{
	m_iMaxHp = iMax;
	m_iCurrentHp = iCurrent;
}

HRESULT CBossHp::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossHp::NativeConstruct(void * pArg)
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

	//if (FAILED(Ready_Layer_UI_HpNumber()))
	//{
	//	MSGBOX("Ready_Layer_UI_HpNumber returned E_FAIL in CBossHp::NativeConstruct");
	//	return E_FAIL;
	//}

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));



	return S_OK;
}

_int CBossHp::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	return _int();
}

_int CBossHp::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CBossHp::Render()
{
	if (nullptr == m_pVIBufferCom)
	{
		MSGBOX("nullptr == m_pVIBufferCom in CBossHp::Render");
		return E_FAIL;
	}

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	_float fCurrentHp_UV = m_iCurrentHp / (_float)m_iMaxHp;
	m_pShaderCom->Set_RawValue("g_CurrentHP_UV", &fCurrentHp_UV, sizeof(_float));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBossHp::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, m_iPass);
	return S_OK;
}

HRESULT CBossHp::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("Add_Component returned E_FAIL in CBossHp::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("Add_Component returned E_FAIL in CBossHp::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("Add_Component returned E_FAIL in CBossHp::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("Add_Component returned E_FAIL in CBossHp::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}
//
//HRESULT CBossHp::Ready_Layer_UI_HpNumber()
//{
//	// 100
//	INFO_UI tInfoUI_100;
//	ZeroMemory(&tInfoUI_100, sizeof(INFO_UI));
//
//	tInfoUI_100.fScaleX = 16.f;
//	tInfoUI_100.fScaleY = 16.f;
//	tInfoUI_100.fPositionX = m_fX + 140.f;
//	tInfoUI_100.fPositionY = m_fY;
//	tInfoUI_100.fDepth = 3.f;
//	tInfoUI_100.iTextureIndex = 16;
//	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_HpNumber"), TEXT("Prototype_GameObject_UI_HpNumber"), &tInfoUI_100)))
//	{
//		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CHpBar::Ready_Layer_UI_HpNumber(tInfoUI_100)");
//		return E_FAIL;
//	}
//	m_pHpNumber[0] = static_cast<CHpNumber*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI_HpNumber")));
//
//	// 10
//	INFO_UI tInfoUI_10;
//	ZeroMemory(&tInfoUI_10, sizeof(INFO_UI));
//
//	tInfoUI_10.fScaleX = 16.f;
//	tInfoUI_10.fScaleY = 16.f;
//	tInfoUI_10.fPositionX = m_fX + 156.f;
//	tInfoUI_10.fPositionY = m_fY;
//	tInfoUI_10.fDepth = 3.f;
//	tInfoUI_10.iTextureIndex = 16;
//	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_HpNumber"), TEXT("Prototype_GameObject_UI_HpNumber"), &tInfoUI_10)))
//	{
//		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CHpBar::Ready_Layer_UI_HpNumber(tInfoUI_10)");
//		return E_FAIL;
//	}
//	m_pHpNumber[1] = static_cast<CHpNumber*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI_HpNumber")));
//
//	// 1
//	INFO_UI tInfoUI_1;
//	ZeroMemory(&tInfoUI_1, sizeof(INFO_UI));
//
//	tInfoUI_1.fScaleX = 16.f;
//	tInfoUI_1.fScaleY = 16.f;
//	tInfoUI_1.fPositionX = m_fX + 172.f;
//	tInfoUI_1.fPositionY = m_fY;
//	tInfoUI_1.fDepth = 3.f;
//	tInfoUI_1.iTextureIndex = 16;
//	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_HpNumber"), TEXT("Prototype_GameObject_UI_HpNumber"), &tInfoUI_1)))
//	{
//		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CHpBar::Ready_Layer_UI_HpNumber(tInfoUI_1)");
//		return E_FAIL;
//	}
//	m_pHpNumber[2] = static_cast<CHpNumber*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI_HpNumber")));
//
//	// "/"
//	INFO_UI tInfoUI__;
//	ZeroMemory(&tInfoUI__, sizeof(INFO_UI));
//
//	tInfoUI__.fScaleX = 16.f;
//	tInfoUI__.fScaleY = 16.f;
//	tInfoUI__.fPositionX = m_fX + 188.f;
//	tInfoUI__.fPositionY = m_fY;
//	tInfoUI__.fDepth = 3.f;
//	tInfoUI__.iTextureIndex = 16;
//	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_HpNumber"), TEXT("Prototype_GameObject_UI_HpNumber"), &tInfoUI__)))
//	{
//		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CHpBar::Ready_Layer_UI_HpNumber(tInfoUI__)");
//		return E_FAIL;
//	}
//	m_pHpNumber[3] = static_cast<CHpNumber*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI_HpNumber")));
//
//	// _100
//	INFO_UI tInfoUI__100;
//	ZeroMemory(&tInfoUI__100, sizeof(INFO_UI));
//
//	tInfoUI__100.fScaleX = 16.f;
//	tInfoUI__100.fScaleY = 16.f;
//	tInfoUI__100.fPositionX = m_fX + 204.f;
//	tInfoUI__100.fPositionY = m_fY;
//	tInfoUI__100.fDepth = 3.f;
//	tInfoUI__100.iTextureIndex = 16;
//	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_HpNumber"), TEXT("Prototype_GameObject_UI_HpNumber"), &tInfoUI__100)))
//	{
//		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CHpBar::Ready_Layer_UI_HpNumber(tInfoUI__100)");
//		return E_FAIL;
//	}
//	m_pHpNumber[4] = static_cast<CHpNumber*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI_HpNumber")));
//
//	// _10
//	INFO_UI tInfoUI__10;
//	ZeroMemory(&tInfoUI__10, sizeof(INFO_UI));
//
//	tInfoUI__10.fScaleX = 16.f;
//	tInfoUI__10.fScaleY = 16.f;
//	tInfoUI__10.fPositionX = m_fX + 220.f;
//	tInfoUI__10.fPositionY = m_fY;
//	tInfoUI__10.fDepth = 3.f;
//	tInfoUI__10.iTextureIndex = 16;
//	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_HpNumber"), TEXT("Prototype_GameObject_UI_HpNumber"), &tInfoUI__10)))
//	{
//		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CHpBar::Ready_Layer_UI_HpNumber(tInfoUI__10)");
//		return E_FAIL;
//	}
//	m_pHpNumber[5] = static_cast<CHpNumber*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI_HpNumber")));
//
//	// _1
//	INFO_UI tInfoUI__1;
//	ZeroMemory(&tInfoUI__1, sizeof(INFO_UI));
//
//	tInfoUI__1.fScaleX = 16.f;
//	tInfoUI__1.fScaleY = 16.f;
//	tInfoUI__1.fPositionX = m_fX + 236.f;
//	tInfoUI__1.fPositionY = m_fY;
//	tInfoUI__1.fDepth = 3.f;
//	tInfoUI__1.iTextureIndex = 16;
//	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_HpNumber"), TEXT("Prototype_GameObject_UI_HpNumber"), &tInfoUI__1)))
//	{
//		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CHpBar::Ready_Layer_UI_HpNumber(tInfoUI__1)");
//		return E_FAIL;
//	}
//	m_pHpNumber[6] = static_cast<CHpNumber*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI_HpNumber")));
//
//	return S_OK;
//}
//
//void CHpBar::SetUp_Number()
//{
//	_int i100 = m_iCurrentHp / 100;
//	_int i10 = (m_iCurrentHp - i100 * 100) / 10;
//	_int i1 = m_iCurrentHp % 10;
//	if (i100 > 0 && i10 == 0)
//		i10 = 11;
//
//	_int i_100 = m_iMaxHp / 100;
//	_int i_10 = (m_iMaxHp - i_100 * 100) / 10;
//	_int i_1 = m_iMaxHp % 10;
//	if (i_100 > 0 && i_10 == 0)
//		i_10 = 11;
//
//	m_pHpNumber[0]->Set_TexIndex(i100);
//	m_pHpNumber[1]->Set_TexIndex(i10);
//	m_pHpNumber[2]->Set_TexIndex(i1);
//	m_pHpNumber[3]->Set_TexIndex(10);
//	m_pHpNumber[4]->Set_TexIndex(i_100);
//	m_pHpNumber[5]->Set_TexIndex(i_10);
//	m_pHpNumber[6]->Set_TexIndex(i_1);
//}

CBossHp * CBossHp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossHp*	pInstance = new CBossHp(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBossHp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBossHp::Clone(void * pArg)
{
	CBossHp*	pInstance = new CBossHp(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBossHp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossHp::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
