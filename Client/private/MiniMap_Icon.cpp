#include "stdafx.h"
#include "..\public\MiniMap_Icon.h"
#include "GameInstance.h"

CMiniMap_Icon::CMiniMap_Icon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMiniMap_Icon::CMiniMap_Icon(const CMiniMap_Icon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniMap_Icon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniMap_Icon::NativeConstruct(void * pArg)
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
	m_iTexIndex = m_pInfoUI->iTextureIndex;
	m_fDepth = (*(INFO_UI*)pArg).fDepth;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));
	m_pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")));
	return S_OK;
}

_int CMiniMap_Icon::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;
	if (true == m_bDead)
		return DEAD;



	return _int();
}

_int CMiniMap_Icon::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	_vector vTargetPos;
	if (TARGET_ARIA == m_iTexIndex)
		vTargetPos = XMLoadFloat4(m_pAriaPos);
	else
		vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPos = vTargetPos - vPlayerPos;
	vPos = XMVectorSetY(vPos, 0.f);




	if (60.f > XMVectorGetX(XMVector4Length(vPos)))
	{
		_matrix PlayerMatrix = m_pPlayerTransform->Get_WorldMatrixInverse();
		vTargetPos = XMVector3TransformCoord(vTargetPos, PlayerMatrix);
		m_fX = 110.f + XMVectorGetX(vTargetPos);
		m_fY = 110.f - XMVectorGetZ(vTargetPos);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}

	

	return _int();
}

HRESULT CMiniMap_Icon::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CMiniMap_Icon::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 0);
	return S_OK;
}

HRESULT CMiniMap_Icon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMiniMap_Icon::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMiniMap_Icon::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMiniMap_Icon::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMap_Icon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMiniMap_Icon::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CMiniMap_Icon * CMiniMap_Icon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMiniMap_Icon*	pInstance = new CMiniMap_Icon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMiniMap_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMiniMap_Icon::Clone(void * pArg)
{
	CMiniMap_Icon*	pInstance = new CMiniMap_Icon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMiniMap_Icon");
		Safe_Release(pInstance);
	}
	m_pInfoUI = (INFO_UI*)pArg;
	return pInstance;
}

void CMiniMap_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
