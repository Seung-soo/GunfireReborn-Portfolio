#include "stdafx.h"
#include "..\public\StageName.h"
#include "GameInstance.h"

CStageName::CStageName(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CStageName::CStageName(const CStageName & rhs)
	: CGameObject(rhs)
	, m_bStage(rhs.m_bStage)
{
}

HRESULT CStageName::NativeConstruct_Prototype(_bool bStage)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	m_bStage = bStage;

	return S_OK;
}

HRESULT CStageName::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (false == m_bStage)
	{
		memcpy(&m_tInfoUI, pArg, sizeof(INFO_UI));

		m_fX = 220.f;
		m_fY = 50.f;
		m_fSizeX = 200.f;
		m_fSizeY = 200.f;
		m_fDepth = 0.1f;
		m_iTexIndex = m_tInfoUI.iTextureIndex;

		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));
		m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));
	}
	else
	{
		m_iTexIndex = (*(STAGENAMEDESC*)pArg).iTexIndex;
		m_pTransformCom->Set_WorldMatrix((*(STAGENAMEDESC*)pArg).WorldMatrix);
	}


	return S_OK;
}

_int CStageName::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(dTimeDelta))
		return -1;

	if (true == m_bStage)
	{
		m_fAlpha -= (_float)dTimeDelta * 64.f;
		if (0.f > m_fAlpha)
			return DEAD;
	}

	return _int();
}

_int CStageName::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;



	if (false == m_bStage)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	return _int();
}

HRESULT CStageName::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CStageName::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CStageName::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CStageName::Render(g_ProjMatrix)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CStageName::Render");
		return E_FAIL;
	}

	if (false == m_bStage)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
		{
			MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CStageName::Render(g_ViewMatrix)");
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		{
			MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CStageName::Render(g_ProjMatrix)");
			return E_FAIL;
		}
		m_pVIBufferCom->Render(m_pShaderCom, 3);
	}
	else
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &g_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		{
			MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CStageName::Render(g_ViewMatrix)");
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &g_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		{
			MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CStageName::Render(g_ProjMatrix)");
			return E_FAIL;
		}
		_int iAlpha = (_int)m_fAlpha;
		if (FAILED(m_pShaderCom->Set_RawValue("g_iHitAlpha", &iAlpha, sizeof(_int))))
		{
			MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CStageName::Render(g_iHitAlpha)");
			return E_FAIL;
		}
		m_pVIBufferCom->Render(m_pShaderCom, 18);
	}

	return S_OK;
}

HRESULT CStageName::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CStageName::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CStageName::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CStageName::SetUp_Components(ViBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StageName"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CStageName::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CStageName * CStageName::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _bool bStage)
{
	CStageName*	pInstance = new CStageName(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(bStage)))
	{
		MSGBOX("Failed to Created CStageName");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CStageName::Clone(void * pArg)
{
	CStageName*	pInstance = new CStageName(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CStageName");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStageName::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
