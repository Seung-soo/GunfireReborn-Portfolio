#include "stdafx.h"
#include "..\public\VictoryRing.h"
#include "GameInstance.h"

CVictoryRing::CVictoryRing(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CVictoryRing::CVictoryRing(const CVictoryRing & rhs)
	: CGameObject(rhs)
{
}

HRESULT CVictoryRing::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVictoryRing::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fX = 640.f;
	m_fY = 360.f;
	m_fSizeX = 400.f;
	m_fSizeY = 400.f;
	m_fDepth = 2.5f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	return S_OK;
}

_int CVictoryRing::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(dTimeDelta))
		return -1;

	m_dTime += dTimeDelta;
	m_fSpriteIndex += 16.f * (_float)dTimeDelta;
	if (4.f <= m_fSpriteIndex)
	{
		m_fSpriteIndex = 0.f;
		m_iSpriteYIndex++;
		if (3 < m_iSpriteYIndex)
			return DEAD;
	}
	m_fSizeX += 2000.f * (_float)dTimeDelta;
	m_fSizeY += 2000.f * (_float)dTimeDelta;

	return _int();
}

_int CVictoryRing::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	m_dTime += dTimeDelta;

	return _int();
}

HRESULT CVictoryRing::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CVictoryRing::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictoryRing::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictoryRing::Render(g_ProjMatrix)");
		return E_FAIL;
	}

	_int iSpriteIndex = (_int)m_fSpriteIndex;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteIndex", &iSpriteIndex, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictoryRing::Render(g_iSpriteIndex)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteYIndex", &m_iSpriteYIndex, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictoryRing::Render(g_iSpriteYIndex)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CVictoryRing::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 7);
	return S_OK;
}

HRESULT CVictoryRing::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictoryRing::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictoryRing::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictoryRing::SetUp_Components(ViBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_VictoryRing"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictoryRing::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CVictoryRing * CVictoryRing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVictoryRing*	pInstance = new CVictoryRing(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CVictoryRing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVictoryRing::Clone(void * pArg)
{
	CVictoryRing*	pInstance = new CVictoryRing(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVictoryRing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVictoryRing::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
