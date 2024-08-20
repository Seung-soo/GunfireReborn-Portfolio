#include "stdafx.h"
#include "..\public\DashEffect.h"
#include "GameInstance.h"

CDashEffect::CDashEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDashEffect::CDashEffect(const CDashEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDashEffect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CDashEffect::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDashEffect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CDashEffect::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CDashEffect::NativeConstruct");
		return E_FAIL;
	}


	m_fX = 640.f;
	m_fY = 360.f;
	m_fSizeX = 1280;
	m_fSizeY = 720;
	m_fDepth = 5.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	for (_uint i = 0; i < 50; i++)
	{
		_int iIndex1 = rand() % 8;
		_int iIndex2 = rand() % 8;
		_int iTemp = m_iIndex[iIndex1];
		m_iIndex[iIndex1] = m_iIndex[iIndex2];
		m_iIndex[iIndex2] = iTemp;
	}

	return S_OK;
}

_int CDashEffect::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return DEAD;

	m_dTime += dTimeDelta * 48.f;
	if (8.0 < m_dTime)
		return DEAD;

	return NOPROBLEM;
}

_int CDashEffect::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return DEAD;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);


	return NOPROBLEM;
}

HRESULT CDashEffect::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CDashEffect::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CDashEffect::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CDashEffect::Render(g_ProjMatrix)");
		return E_FAIL;
	}
	_int iTexIndex = (_int)m_dTime;
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iIndex[iTexIndex])))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CDashEffect::Render");
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 3)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CDashEffect::Render");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDashEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CDashEffect::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CDashEffect::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CDashEffect::SetUp_Components(ViBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DashEffect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CDashEffect::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CDashEffect * CDashEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDashEffect*	pInstance = new CDashEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CDashEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDashEffect::Clone(void * pArg)
{
	CDashEffect*	pInstance = new CDashEffect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CDashEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDashEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
