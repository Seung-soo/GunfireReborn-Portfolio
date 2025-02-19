#include "stdafx.h"
#include "..\public\WaterEffect.h"
#include "GameInstance.h"
#include "Effect.h"
#include "Easing_Function.h"
#include "Particle_Effect.h"

CWaterEffect::CWaterEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CWaterEffect::CWaterEffect(const CWaterEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWaterEffect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CWaterEffect::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWaterEffect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 30.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 30.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CWaterEffect::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CWaterEffect::NativeConstruct");
		return E_FAIL;
	}

	_vector	vPosition = XMLoadFloat4((_float4*)pArg);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, 0.5f));

	_float4 vParticlePos;
	XMStoreFloat4(&vParticlePos, vPosition + static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK) * -6.f + XMVectorSet(0.f, 5.f, 0.f, 0.f));
	// 물튀기기 파티클
	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 9;
	tParticleDesc.fMinSpeed = 20.f;
	tParticleDesc.fMaxSpeed = 30.f;
	tParticleDesc.fOriginScale = 0.3f;
	tParticleDesc.fScaleSpeed = 0.05f;
	tParticleDesc.fGravityPower = 90.f;
	tParticleDesc.fLiveTime = 0.7f;
	tParticleDesc.fMakeRange = 20.f;
	tParticleDesc.fMaxDistance = 20.f;
	tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
	tParticleDesc.pParentPosition = &vParticlePos;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CParticle_Effect::OUTCUBIC;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead = nullptr;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}

	g_pGameInstance->StopSound(CSoundMgr::EFFECT9);
	g_pGameInstance->LetsPlaySound(TEXT("Boss1Up.mp3"), CSoundMgr::EFFECT9, 0.3f);
	
	return S_OK;
}

_int CWaterEffect::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		return DEAD;
	}
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CWaterEffect::Tick");
		return E_FAIL;
	}

	if (0.3f > m_fTime)
	{
		m_fScale = EaseOutExpo(1.f, m_fTime, 0.3f) * 7.f + 1.f;
		m_iPass = 20;
		m_pTransformCom->Scaled(XMVectorSet(m_fScale, m_fScale * 1.5f, m_fScale, 0.f));
	}
	else if (255.f > m_fAlpha)
	{
		m_fScale += (_float)TimeDelta * 10.f;
		m_fAlpha += 800.f * (_float)TimeDelta;
		m_pTransformCom->Go_Down(TimeDelta * 0.12f);
		m_pTransformCom->Scaled(XMVectorSet(m_fScale, 12.f, m_fScale, 0.f));
	}
	else
		m_bDead = true;
	//m_fAddNoiseUV += _float(TimeDelta);
	//m_fMoveUV -= _float(TimeDelta) * 2.f;

	m_fTime += (_float)TimeDelta;
	return _int();
}

_int CWaterEffect::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CWaterEffect::LateTick");
		return E_FAIL;
	}


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);


	return _int();
}




HRESULT CWaterEffect::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom returned E_FAIL in CWaterEffect::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CWaterEffect::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i);
	}

	return S_OK;
}

HRESULT CWaterEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWaterEffect::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_ModelShader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWaterEffect::SetUp_Components(Shader)");
		return E_FAIL;
	}
	/* For.Com_NoiseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_NoiseTexture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWaterEffect::SetUp_Components(Prototype_Component_Texture_NoiseFire)");
		return E_FAIL;
	}
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_WaterEffect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWaterEffect::SetUp_Components(Prototype_Component_Model_WaterEffect)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWaterEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CWaterEffect::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CWaterEffect::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWaterEffect::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWaterEffect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (20 == m_iPass)
	{
		_float fAlpha = m_fAlpha / 255.f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		{
			MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWaterEffect::SetUp_ConstantTable(g_iHitAlpha)");
			return E_FAIL;
		}
	}


	//if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 6)))
	//{
	//	MSGBOX("m_pNoiseTextureCom->SetUp_OnShader returned E_FAIL in CWaterEffect::SetUp_ConstantTable(DiffuseTexture)");
	//	return E_FAIL;
	//}


	//_float	fMoveUVX = 0.f;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &fMoveUVX, sizeof(_float))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWaterEffect::SetUp_ConstantTable(g_fMoveUV_X)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &m_fMoveUV, sizeof(_float))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWaterEffect::SetUp_ConstantTable(g_fMoveUV_Y)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &fMoveUVX, sizeof(_float))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWaterEffect::SetUp_ConstantTable(g_fMoveNoise_X)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &m_fAddNoiseUV, sizeof(_float))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWaterEffect::SetUp_ConstantTable(g_fMoveNoise_Y)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_Texture("g_WindowTexture", m_pRendererCom->Get_SRV(TEXT("Target_Caculation")))))
	//{
	//	MSGBOX("m_pShaderCom->Set_Texture returned E_FAIL in CWaterEffect::SetUp_ConstantTable(g_WindowTexture)");
	//	return E_FAIL;
	//}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CWaterEffect * CWaterEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWaterEffect*	pInstance = new CWaterEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CWaterEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWaterEffect::Clone(void * pArg)
{
	CWaterEffect*	pInstance = new CWaterEffect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CWaterEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWaterEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseTextureCom);
}
