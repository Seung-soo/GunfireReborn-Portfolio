#include "stdafx.h"
#include "..\public\CoolTime.h"
#include "GameInstance.h"
#include "NumberUI.h"

CCoolTime::CCoolTime(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CCoolTime::CCoolTime(const CCoolTime & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCoolTime::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoolTime::NativeConstruct(void * pArg)
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
		memcpy(&m_tCoolTimeDesc, pArg, sizeof(COOLTIMEDESC));
	m_iCurrentTime = _int(*m_tCoolTimeDesc.pCurrentCoolTime) + 1;

	CNumberUI::NUMBERDESC tNumberDesc;
	tNumberDesc.eNumberType = CNumberUI::TYPE_COOLTIME;
	tNumberDesc.fX = m_tCoolTimeDesc.tInfoUI.fPositionX;
	tNumberDesc.fY = m_tCoolTimeDesc.tInfoUI.fPositionY;
	tNumberDesc.pCurrent = &m_iCurrentTime;
	
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_NumberUI"), TEXT("Prototype_GameObject_NumberUI"), &tNumberDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CCoolTime::NativeConstruct");
		return E_FAIL;
	}
	m_pNumberUI = g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_NumberUI"));

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_pTransformCom->Scaled(XMVectorSet(m_tCoolTimeDesc.tInfoUI.fScaleX, m_tCoolTimeDesc.tInfoUI.fScaleY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tCoolTimeDesc.tInfoUI.fPositionX - g_iWinCX * 0.5f, -m_tCoolTimeDesc.tInfoUI.fPositionY + g_iWinCY * 0.5f, 0.f, 1.f));

	return S_OK;
}

_int CCoolTime::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
	{
		m_pNumberUI->Set_Dead(true);
		m_pNumberUI = nullptr;

		INFO_UI tInfoUI;
		tInfoUI.fPositionX = m_tCoolTimeDesc.tInfoUI.fPositionX + 3.f;
		tInfoUI.fPositionY = m_tCoolTimeDesc.tInfoUI.fPositionY - 27.f;
		tInfoUI.fScaleX = 200.f;
		tInfoUI.fScaleY = 400.f;
		tInfoUI.fDepth = -0.7f;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_CoolTime"), TEXT("Prototype_GameObject_VictoryShine"), &tInfoUI)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CCoolTime::Tick");
			return DEAD;
		}

		return DEAD;
	}
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	m_iCurrentTime = _int(*m_tCoolTimeDesc.pCurrentCoolTime) + 1;
	if (0.f > *m_tCoolTimeDesc.pCurrentCoolTime)
	{
		m_bDead = true;
	}

	return _int();
}

_int CCoolTime::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CCoolTime::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CCoolTime::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CCoolTime::Render(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CCoolTime::Render(Proj)");
		return E_FAIL;
	}

	_float fCurrentCoolTime_UV = *m_tCoolTimeDesc.pCurrentCoolTime / *m_tCoolTimeDesc.pMaxCoolTime;
	if (FAILED(m_pShaderCom->Set_RawValue("g_CurrentHP_UV", &fCurrentCoolTime_UV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CCoolTime::Render(g_CurrentHP_UV)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_tCoolTimeDesc.tInfoUI.iTextureIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CCoolTime::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 19);
	return S_OK;
}

HRESULT CCoolTime::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CCoolTime::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CCoolTime::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CCoolTime::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CoolTime"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CCoolTime::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CCoolTime * CCoolTime::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCoolTime*	pInstance = new CCoolTime(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CCoolTime");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCoolTime::Clone(void * pArg)
{
	CCoolTime*	pInstance = new CCoolTime(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CCoolTime");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCoolTime::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}

