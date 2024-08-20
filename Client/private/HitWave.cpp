#include "stdafx.h"
#include "..\public\HitWave.h"
#include "GameInstance.h"
#include "Easing_Function.h"
#include "PipeLine.h"
#include "Player_Hand.h"

CHitWave::CHitWave(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CHitWave::CHitWave(const CHitWave & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHitWave::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHitWave::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CHitWave::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CHitWave::NativeConstruct");
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_pTransformCom->Scaled(XMVectorSet(1280.f, 720.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(640.f - g_iWinCX * 0.5f, -320.f + g_iWinCY * 0.5f, 0.f, 1.f));

	m_iTexIndex = 6;
	return S_OK;
}

_int CHitWave::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CHitWave::Tick");
		return DEAD;
	}

	m_fAddNoiseUV -= (_float)TimeDelta;
	m_fDistortionTime += (_float)TimeDelta;

	m_fDistortionPower = 0.05f - EaseInQuint(0.05f, m_fDistortionTime, 1.5f);
	if (1.5f < m_fDistortionTime)
		m_bDead = true;
	return _int();
}

_int CHitWave::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CHitWave::LateTick");
		return DEAD;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}

HRESULT CHitWave::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CHitWave::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 32)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CHitWave::Render");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CHitWave::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHitWave::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHitWave::SetUp_Components(Shader");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHitWave::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture1 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_SourceTexture"), (CComponent**)&m_pTextureCom[1])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CHitWave::SetUp_Components(Texture");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHitWave::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CHitWave::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CHitWave::SetUp_ConstantTable");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CHitWave::SetUp_ConstantTable(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CHitWave::SetUp_ConstantTable(g_ProjMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom[1]->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CHitWave::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_AddNoiseUV", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CHitWave::SetUp_ConstantTable(g_AddNoiseUV)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_DistortionPower", &m_fDistortionPower, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CHitWave::SetUp_ConstantTable(g_DistortionPower)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_Texture("g_WindowTexture", m_pRendererCom->Get_SRV(TEXT("Target_Caculation")))))
	{
		MSGBOX("m_pShaderCom->Set_Texture returned E_FAIL in CHitWave::SetUp_ConstantTable(g_WindowTexture)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CHitWave * CHitWave::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHitWave*	pInstance = new CHitWave(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CHitWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHitWave::Clone(void * pArg)
{
	CHitWave*	pInstance = new CHitWave(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CHitWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHitWave::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	for (auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);
	Safe_Release(m_pShaderCom);
}
