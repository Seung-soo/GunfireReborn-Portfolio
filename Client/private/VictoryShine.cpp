#include "stdafx.h"
#include "..\public\VictoryShine.h"
#include "GameInstance.h"

CVictoryShine::CVictoryShine(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CVictoryShine::CVictoryShine(const CVictoryShine & rhs)
	: CGameObject(rhs)
{
}

HRESULT CVictoryShine::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVictoryShine::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_fX = (*(INFO_UI*)pArg).fPositionX;
		m_fY = (*(INFO_UI*)pArg).fPositionY;
		m_fSizeX = (*(INFO_UI*)pArg).fScaleX;
		m_fSizeY = (*(INFO_UI*)pArg).fScaleY;
		m_fDepth = (*(INFO_UI*)pArg).fDepth;
	}
	else
	{
		m_fX = 640.f;
		m_fY = 360.f;
		m_fSizeX = 500.f;
		m_fSizeY = 1000.f;
		m_fDepth = 2.f;
	}



	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	return S_OK;
}

_int CVictoryShine::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(dTimeDelta))
		return -1;


	m_dTime += dTimeDelta;
	m_fSpriteIndex += 32.f * (_float)dTimeDelta;
	if (4.f <= m_fSpriteIndex)
	{
		m_fSpriteIndex = 0.f;
		m_iSpriteYIndex++;
		if (1 < m_iSpriteYIndex)
			return DEAD;
	}

	return _int();
}

_int CVictoryShine::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);


	return _int();
}

HRESULT CVictoryShine::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CVictoryShine::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictoryShine::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictoryShine::Render(g_ProjMatrix)");
		return E_FAIL;
	}
	
	_int iSpriteIndex = (_int)m_fSpriteIndex;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteIndex", &iSpriteIndex, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictoryShine::Render(g_iSpriteIndex)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteYIndex", &m_iSpriteYIndex, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictoryShine::Render(g_iSpriteYIndex)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CVictoryShine::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 13);
	return S_OK;
}

HRESULT CVictoryShine::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictoryShine::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictoryShine::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictoryShine::SetUp_Components(ViBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_VictoryShine"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictoryShine::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CVictoryShine * CVictoryShine::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVictoryShine*	pInstance = new CVictoryShine(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CVictoryShine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVictoryShine::Clone(void * pArg)
{
	CVictoryShine*	pInstance = new CVictoryShine(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVictoryShine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVictoryShine::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
