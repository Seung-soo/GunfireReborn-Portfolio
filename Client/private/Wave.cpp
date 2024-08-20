#include "stdafx.h"
#include "..\public\Wave.h"
#include "GameInstance.h"
#include "Player_Hand.h"
#include "Particle_Effect.h"
#include "Camera_Dynamic.h"

CWave::CWave(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CWave::CWave(const CWave & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWave::Set_Collider()
{

	if (nullptr == m_pColliderCom)
	{
		MSGBOX("nullptr == m_pColliderCom in CWave::Set_Collider");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CWave::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CWave::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWave::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4((_float4*)pArg));
	m_pTransformCom->Scaled(XMVectorSet(0.5f, 1.f, 0.3f, 0.f));

	// 물튀기기 파티클
	XMStoreFloat4(&m_vMyPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 3.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
	XMStoreFloat4(&m_vMyPosition1, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 3.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 12.f);
	XMStoreFloat4(&m_vMyPosition2, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 3.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 24.f);
	XMStoreFloat4(&m_vMyPosition3, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 3.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f) - m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 12.f);
	XMStoreFloat4(&m_vMyPosition4, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 3.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f) - m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 24.f);

	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 9;
	tParticleDesc.fMinSpeed = 10.f;
	tParticleDesc.fMaxSpeed = 20.f;
	tParticleDesc.fOriginScale = 0.3f;
	tParticleDesc.fScaleSpeed = 0.19f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 100.f;
	tParticleDesc.fMakeRange = 5.f;
	tParticleDesc.fMaxDistance = 20.f;
	tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
	tParticleDesc.pParentPosition = &m_vMyPosition;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_30;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead = &m_bDead;
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
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}
	tParticleDesc.iTexIndex = 10;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}

	tParticleDesc.pParentPosition = &m_vMyPosition1;
	tParticleDesc.iTexIndex = 9;
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
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}
	tParticleDesc.iTexIndex = 10;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}

	tParticleDesc.pParentPosition = &m_vMyPosition2;
	tParticleDesc.iTexIndex = 9;
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
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}
	tParticleDesc.iTexIndex = 10;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}

	tParticleDesc.pParentPosition = &m_vMyPosition3;
	tParticleDesc.iTexIndex = 9;
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
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}
	tParticleDesc.iTexIndex = 10;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}

	tParticleDesc.pParentPosition = &m_vMyPosition4;
	tParticleDesc.iTexIndex = 9;
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
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}
	tParticleDesc.iTexIndex = 10;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}

	static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Set_HorizonShake(0.8f, 0.5f);

	return S_OK;
}

_int CWave::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CWave::Tick");
		return -1;
	}
	if (0.8 < m_dLiveTime)
		m_bDead = true;

	m_pTransformCom->Go_Vector(XMVectorSet(-1.f, 0.f, 0.f, 0.f), dTimeDelta * 2.f);
	m_dLiveTime += dTimeDelta;

	CCollider* pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));
	if (nullptr != m_pColliderCom)
	{
		if (false == m_bCollision && true == g_pGameInstance->Collision_AABBToSphere(m_pColliderCom, pPlayerCollider))
		{
			static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(60.f);
			m_bCollision = true;

			g_pGameInstance->StopSound(CSoundMgr::MONSTER2);
			g_pGameInstance->LetsPlaySound(TEXT("HitWave.mp3"), CSoundMgr::MONSTER2, 0.5f);

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_HitWave"), TEXT("Prototype_GameObject_HitWave"))))
			{
				__debugbreak();
				return E_FAIL;
			}
		}
	}
	XMStoreFloat4(&m_vMyPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 3.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
	XMStoreFloat4(&m_vMyPosition1, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 3.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 12.f);
	XMStoreFloat4(&m_vMyPosition2, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 3.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 24.f);
	XMStoreFloat4(&m_vMyPosition3, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 3.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f) - m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 12.f);
	XMStoreFloat4(&m_vMyPosition4, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 3.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f) - m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 24.f);

	m_fAddNoiseUV -= (_float)dTimeDelta * 5.f;

	return _int();
}

_int CWave::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CWave::LateTick");
		return -1;
	}
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	// 절두체
	//if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRadius))
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
	m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	return _int();
}




HRESULT CWave::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CWave::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CWave::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 17, i);
	}

	return S_OK;
}

HRESULT CWave::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWave::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWave::SetUp_Components(Shader)");
		return E_FAIL;
	}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_SourceTexture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CNoiseRect::SetUp_Components(Texture");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_WaveSphere"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWave::SetUp_Components(Model)");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(3.f, 3.f, 80.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWave::SetUp_Components(Collider)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CWave::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CWave::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CWave::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWave::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWave::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 36)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CWave::SetUp_ConstantTable(DiffuseTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_AddNoiseUV)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_Texture("g_WindowTexture", m_pRendererCom->Get_SRV(TEXT("Target_Caculation")))))
	{
		MSGBOX("m_pShaderCom->Set_Texture returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_WindowTexture)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CWave * CWave::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWave*	pInstance = new CWave(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWave::Clone(void * pArg)
{
	CWave*	pInstance = new CWave(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWave::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
