#include "stdafx.h"
#include "..\public\Victory_UI.h"
#include "GameInstance.h"

CVictory_UI::CVictory_UI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CVictory_UI::CVictory_UI(const CVictory_UI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CVictory_UI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVictory_UI::NativeConstruct(void * pArg)
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
	m_fSizeX = 1500.f;
	m_fSizeY = 1500.f;
	m_fDepth = 3.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	g_pGameInstance->StopSound(CSoundMgr::UI);
	g_pGameInstance->LetsPlaySound(TEXT("Victory.mp3"), CSoundMgr::UI, 0.7f);

	return S_OK;
}

_int CVictory_UI::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(dTimeDelta))
		return -1;

	if (0.2 > m_dTime)
	{
		m_fAlpha += (_float)dTimeDelta * 127.5f * 5.f;
		m_fSizeX -= (_float)dTimeDelta * 5500.f;
		m_fSizeY -= (_float)dTimeDelta * 5500.f;
	}
	else if (1.7 > m_dTime)
	{
		if (true == m_bStart)
		{
			// 샤인, 돌, 링 생성
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Victory"), TEXT("Prototype_GameObject_VictoryShine"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CVictory_UI::Tick(VictoryShine)");
				return DEAD;
			}
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Victory"), TEXT("Prototype_GameObject_VictoryRock"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CVictory_UI::Tick(VictoryRock)");
				return DEAD;
			}
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Victory"), TEXT("Prototype_GameObject_VictoryRing"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CVictory_UI::Tick(VictoryRing)");
				return DEAD;
			}
			m_bStart = false;
		}

		m_fSizeX += (_float)dTimeDelta * 50.f;
		m_fSizeY += (_float)dTimeDelta * 50.f;
	}
	else
	{
		m_fSizeX -= (_float)dTimeDelta * 2000.f;
		m_fSizeY -= (_float)dTimeDelta * 2000.f;
	}

	if (10.f > m_fSizeX)
		m_bDead = true;

	return _int();
}

_int CVictory_UI::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	m_dTime += dTimeDelta;

	return _int();
}

HRESULT CVictory_UI::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CVictory_UI::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictory_UI::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictory_UI::Render(g_ProjMatrix)");
		return E_FAIL;
	}

	_int iAlpha = (_int)m_fAlpha;
	if (iAlpha > 255)
		iAlpha = 255;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iHitAlpha", &iAlpha, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CVictory_UI::Render(g_iHitAlpha)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBackGround::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 12);
	return S_OK;
}

HRESULT CVictory_UI::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictory_UI::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictory_UI::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictory_UI::SetUp_Components(ViBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Victory"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CVictory_UI::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CVictory_UI * CVictory_UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVictory_UI*	pInstance = new CVictory_UI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CVictory_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVictory_UI::Clone(void * pArg)
{
	CVictory_UI*	pInstance = new CVictory_UI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVictory_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVictory_UI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
