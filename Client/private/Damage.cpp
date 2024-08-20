#include "stdafx.h"
#include "..\public\Damage.h"
#include "GameInstance.h"
#include "DamageNumber.h"

CDamage::CDamage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDamage::CDamage(const CDamage & rhs)
	: CGameObject(rhs)
{
}


HRESULT CDamage::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamage::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	memcpy(&m_tNumberDesc, pArg, sizeof(NUMBERDESC));
	m_Numbers.reserve(5);
	if (TYPE_CRITICAL == m_tNumberDesc.eType)
		m_vColor = _float3(238.f / 255.f, 217.f / 255.f, 79.f / 255.f);
	else if (TYPE_COMB == m_tNumberDesc.eType)
		m_vColor = _float3(1.f, 0.f, 0.f);
	else if (TYPE_MIAS == m_tNumberDesc.eType)
		m_vColor = _float3(0.f, 1.f, 0.f);
	else
		m_vColor = _float3(0.8f, 0.8f, 0.8f);

	if (FAILED(Make_Number()))
	{
		MSGBOX("Make_Number returned E_FAIL in CDamage::NativeConstruct");
		return E_FAIL;
	}

	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tNumberDesc.vPosition + m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * _float(rand() % 100 - 50) / 100.f + m_pTransformCom->Get_State(CTransform::STATE_UP) * _float(rand() % 100 - 50) / 100.f);

	m_vMyPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	return S_OK;
}

_int CDamage::Tick(_double dTimeDelta)
{
	if (true == m_Numbers[0]->Get_Dead())
		return DEAD;
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	return _int();
}

_int CDamage::LateTick(_double dTimeDelta)
{
	if (true == m_Numbers[0]->Get_Dead())
		return DEAD;

	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	m_vMyRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	m_vViewPortPosition = XMVector3TransformCoord(m_vMyPosition, ViewMatrix);
	m_fZ = XMVectorGetZ(m_vViewPortPosition);

	_matrix	ProjMatrix;
	ProjMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	m_vViewPortPosition = XMVector3TransformCoord(m_vViewPortPosition, ProjMatrix);

	m_vViewPortPosition = XMVectorSet((XMVectorGetX(m_vViewPortPosition) + 1) * (g_iWinCX * 0.5f)
		, (XMVectorGetY(m_vViewPortPosition) - 1) * -(g_iWinCY * 0.5f)
		, 0.f, 1.f);

	if (m_tNumberDesc.iDamage >= 10000.f)
		Damege_10000();
	else if (m_tNumberDesc.iDamage >= 1000.f)
		Damege_1000();
	else if (m_tNumberDesc.iDamage >= 100.f)
		Damege_100();
	else if (m_tNumberDesc.iDamage >= 10.f)
		Damege_10();
	else if (m_tNumberDesc.iDamage >= 1.f)
		Damege_1();

	return _int();
}

HRESULT CDamage::Render()
{
	return S_OK;
}

HRESULT CDamage::Make_Number()
{
	CDamageNumber::NUMBERDESC tNumberDesc;
	tNumberDesc.vColor = m_vColor;

	tNumberDesc.fY = XMVectorGetY(m_vViewPortPosition);
	if (m_tNumberDesc.iDamage >= 10000)
	{
		tNumberDesc.iTexIndex = m_tNumberDesc.iDamage / 10000;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_DamageNumber"), TEXT("Prototype_GameObject_DamageNumber"), &tNumberDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CDamage::Make_Number");
			return E_FAIL;
		}
		m_Numbers.push_back(static_cast<CDamageNumber*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_DamageNumber"))));
	}
	if (m_tNumberDesc.iDamage >= 1000)
	{
		tNumberDesc.iTexIndex = (m_tNumberDesc.iDamage % 10000) / 1000;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_DamageNumber"), TEXT("Prototype_GameObject_DamageNumber"), &tNumberDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CDamage::Make_Number");
			return E_FAIL;
		}
		m_Numbers.push_back(static_cast<CDamageNumber*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_DamageNumber"))));
	}
	if (m_tNumberDesc.iDamage >= 100)
	{
		tNumberDesc.iTexIndex = (m_tNumberDesc.iDamage % 1000) / 100;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_DamageNumber"), TEXT("Prototype_GameObject_DamageNumber"), &tNumberDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CDamage::Make_Number");
			return E_FAIL;
		}
		m_Numbers.push_back(static_cast<CDamageNumber*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_DamageNumber"))));
	}
	if (m_tNumberDesc.iDamage >= 10)
	{
		tNumberDesc.iTexIndex = (m_tNumberDesc.iDamage % 100) / 10;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_DamageNumber"), TEXT("Prototype_GameObject_DamageNumber"), &tNumberDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CDamage::Make_Number");
			return E_FAIL;
		}
		m_Numbers.push_back(static_cast<CDamageNumber*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_DamageNumber"))));
	}
	if (m_tNumberDesc.iDamage >= 0)
	{
		tNumberDesc.iTexIndex = (m_tNumberDesc.iDamage % 10);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_DamageNumber"), TEXT("Prototype_GameObject_DamageNumber"), &tNumberDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CDamage::Make_Number");
			return E_FAIL;
		}
		m_Numbers.push_back(static_cast<CDamageNumber*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_DamageNumber"))));
	}
	

	return S_OK;
}

void CDamage::Damege_10000()
{
	m_Numbers[0]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) - 36.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
	m_Numbers[1]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) - 18.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
	m_Numbers[2]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition), XMVectorGetY(m_vViewPortPosition), m_fZ);
	m_Numbers[3]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) + 18.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
	m_Numbers[4]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) + 36.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
}

void CDamage::Damege_1000()
{

	m_Numbers[0]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) - 27.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
	m_Numbers[1]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) - 9.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
	m_Numbers[2]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) + 9.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
	m_Numbers[3]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) + 27.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
}

void CDamage::Damege_100()
{
	m_Numbers[0]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) - 18.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
	m_Numbers[1]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition), XMVectorGetY(m_vViewPortPosition), m_fZ);
	m_Numbers[2]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) + 18.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
}

void CDamage::Damege_10()
{
	m_Numbers[0]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) - 9.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
	m_Numbers[1]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition) + 9.f, XMVectorGetY(m_vViewPortPosition), m_fZ);
}

void CDamage::Damege_1()
{
	m_Numbers[0]->Set_UIPosition(XMVectorGetX(m_vViewPortPosition), XMVectorGetY(m_vViewPortPosition), m_fZ);
}


CDamage * CDamage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDamage*	pInstance = new CDamage(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CDamage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDamage::Clone(void * pArg)
{
	CDamage*	pInstance = new CDamage(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CDamage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDamage::Free()
{
	__super::Free();
}
