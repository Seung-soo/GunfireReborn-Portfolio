#include "stdafx.h"
#include "..\public\HpBar.h"
#include "GameInstance.h"
#include "HpNumber.h"

CHpBar::CHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CHpBar::CHpBar(const CHpBar & rhs)
	: CGameObject(rhs)
	, m_iTexIndex(rhs.m_iTexIndex)
{
}

void CHpBar::Set_Type(HP_TYPE eHp_Type)
{
	m_eHp_Type = eHp_Type;
	if (TYPE_HP == m_eHp_Type)
		m_iTexIndex = 28;
	else if (TYPE_SHIELD == m_eHp_Type)
		m_iTexIndex = 29;
}

void CHpBar::Set_Hp(_uint iMax, _uint iCurrent)
{
	m_iMaxHp = iMax;
	m_iCurrentHp = iCurrent;
}

HRESULT CHpBar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpBar::NativeConstruct(void * pArg)
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
	//	MSGBOX("Ready_Layer_UI_HpNumber returned E_FAIL in CHpBar::NativeConstruct");
	//	return E_FAIL;
	//}

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-27.f));

	

	return S_OK;
}

_int CHpBar::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	if (TYPE_SHIELD == m_eHp_Type)
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-28.f));


	return _int();
}

_int CHpBar::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	SetUp_Number();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CHpBar::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	_float fCurrentHp_UV = m_iCurrentHp / (_float)m_iMaxHp;
	m_pShaderCom->Set_RawValue("g_CurrentHP_UV", &fCurrentHp_UV, sizeof(_float));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CHpBar::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 4);
	return S_OK;
}

HRESULT CHpBar::SetUp_Components()
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

HRESULT CHpBar::Ready_Layer_UI_HpNumber()
{


	return S_OK;
}

void CHpBar::SetUp_Number()
{

}

CHpBar * CHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHpBar*	pInstance = new CHpBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CHpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHpBar::Clone(void * pArg)
{
	CHpBar*	pInstance = new CHpBar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CHpBar");
		Safe_Release(pInstance);
	}
	m_pInfoUI = (INFO_UI*)pArg;
	return pInstance;
}

void CHpBar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
