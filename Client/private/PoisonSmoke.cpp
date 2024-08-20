#include "stdafx.h"
#include "..\public\PoisonSmoke.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "Effect.h"
#include "MonsterWave.h"
#include "Monster.h"
#include "Damage.h"
#include "Particle_Effect.h"

CPoisonSmoke::CPoisonSmoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CPoisonSmoke::CPoisonSmoke(const CPoisonSmoke & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPoisonSmoke::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPoisonSmoke::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 10.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CTransform* pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")));
	m_vStartPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION) + pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vStartPos);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK) + XMVectorSet(0.f, 0.2f, 0.f, 0.f)));
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), pPlayerTransform->Get_State(CTransform::STATE_LOOK)));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Cross(pPlayerTransform->Get_State(CTransform::STATE_LOOK), pPlayerTransform->Get_State(CTransform::STATE_RIGHT)));

	g_pGameInstance->StopSound(CSoundMgr::PLAYER);
	g_pGameInstance->LetsPlaySound(TEXT("ThrowSmoke.mp3"), CSoundMgr::PLAYER, 0.5f);

	return S_OK;
}

_int CPoisonSmoke::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		Smoke_Effect();

		return DEAD;
	}

	if (0 > __super::Tick(TimeDelta))
		return -1;

	Throw(TimeDelta);

	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CPoisonSmoke::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	if (FAILED(Collision_Terrain()))
	{
		MSGBOX("Collision_Terrain returned E_FAIL in CPoisonSmoke::LateTick");
		return E_FAIL;
	}

	if (LEVEL_STAGE3_2 == g_eCurrentLevel && 3.f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{
		m_bDead = true;

		auto ColliderList = g_pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
		vector<CGameObject*> ParentContainer;

		if (nullptr != ColliderList)
			for (auto& pCollider : *ColliderList)
			{
				_bool bEqual = false;
				for (auto& pParent : ParentContainer)
				{
					if (bEqual = (pParent == pCollider->Get_Parent()))
						break;
				}
				if (true == bEqual)
					continue;
				if (g_pGameInstance->Collision_Sphere(m_pColliderCom, pCollider))
				{
					CDamage::NUMBERDESC tNumberDesc;
					tNumberDesc.eType = CDamage::TYPE_MIAS;
					tNumberDesc.iDamage = static_cast<CMonster*>(pCollider->Get_Parent())->Hit(800.f, 1.f, true);
					tNumberDesc.vPosition = static_cast<CTransform*>(pCollider->Get_Parent()->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f);

					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoisonSmoke::Collision_Terrain");
						return E_FAIL;
					}

					static_cast<CMonster*>(pCollider->Get_Parent())->Set_Decay(true);
					ParentContainer.push_back(pCollider->Get_Parent());
				}
			}
		ParentContainer.clear();
	}
	else if (LEVEL_STAGE4_2 == g_eCurrentLevel && 41.5f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{
		m_bDead = true;

		auto ColliderList = g_pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
		vector<CGameObject*> ParentContainer;

		if (nullptr != ColliderList)
			for (auto& pCollider : *ColliderList)
			{
				_bool bEqual = false;
				for (auto& pParent : ParentContainer)
				{
					if (bEqual = (pParent == pCollider->Get_Parent()))
						break;
				}
				if (true == bEqual)
					continue;
				if (g_pGameInstance->Collision_Sphere(m_pColliderCom, pCollider))
				{
					CDamage::NUMBERDESC tNumberDesc;
					tNumberDesc.eType = CDamage::TYPE_MIAS;
					tNumberDesc.iDamage = static_cast<CMonster*>(pCollider->Get_Parent())->Hit(800.f, 1.f, true);
					tNumberDesc.vPosition = static_cast<CTransform*>(pCollider->Get_Parent()->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f);

					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoisonSmoke::Collision_Terrain");
						return E_FAIL;
					}

					static_cast<CMonster*>(pCollider->Get_Parent())->Set_Decay(true);
					ParentContainer.push_back(pCollider->Get_Parent());
				}
			}
		ParentContainer.clear();
	}
	
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}


	return _int();
}




HRESULT CPoisonSmoke::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("m_pModelCom is nullptr in CPoisonSmoke");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CPoisonSmoke::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 0, i);
	}

	return S_OK;
}

HRESULT CPoisonSmoke::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_PoisonSmoke"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(10.f, 10.f, 10.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoisonSmoke::SetUp_Components(Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPoisonSmoke::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CPoisonSmoke::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CPoisonSmoke::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPoisonSmoke::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPoisonSmoke::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CPoisonSmoke::Collision_Terrain()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CPoisonSmoke::Collision_Terrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CPoisonSmoke::Collision_Terrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	// 로컬 포지션 구하기
	_matrix	WorldMatrixInverse = pTerrainTransform->Get_WorldMatrixInverse();

	_vector	vLocalPos = XMVector3TransformCoord(vPosition, WorldMatrixInverse);
	_float3 vLocalPosFloat3;
	XMStoreFloat3(&vLocalPosFloat3, vLocalPos);

	vPosition = XMVector3TransformCoord(vLocalPos, pTerrainTransform->Get_WorldMatrix());

	////////////////////////// 점프 멈춰!!!!!!!!////////////////////////
	_float fTerrainY = pVIBuffer_Terrain->Compute_Y(vLocalPosFloat3);
	_float fMyHalfScaleY = XMVectorGetY(m_pTransformCom->Get_Scale(CTransform::STATE_UP)) * 0.5f;
	if (fTerrainY > vLocalPosFloat3.y - fMyHalfScaleY)
	{
		m_bDead = true;

		auto ColliderList = g_pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
		vector<CGameObject*> ParentContainer;

		if (nullptr != ColliderList)
			for (auto& pCollider : *ColliderList)
			{
				_bool bEqual = false;
				for (auto& pParent : ParentContainer)
				{
					if (bEqual = (pParent == pCollider->Get_Parent()))
						break;
				}
				if (true == bEqual)
					continue;
				if (g_pGameInstance->Collision_Sphere(m_pColliderCom, pCollider))
				{
					CDamage::NUMBERDESC tNumberDesc;
					tNumberDesc.eType = CDamage::TYPE_MIAS;
					tNumberDesc.iDamage = static_cast<CMonster*>(pCollider->Get_Parent())->Hit(800.f, 1.f, true);
					tNumberDesc.vPosition = static_cast<CTransform*>(pCollider->Get_Parent()->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f);
					
					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoisonSmoke::Collision_Terrain");
						return E_FAIL;
					}
					
					static_cast<CMonster*>(pCollider->Get_Parent())->Set_Decay(true);
					ParentContainer.push_back(pCollider->Get_Parent());
				}
			}
		ParentContainer.clear();
	}

	/////////////////////////////////////////////////////////////////////
	return S_OK;
}

HRESULT CPoisonSmoke::Throw(_double dTimeDelta)
{
	m_pTransformCom->Go_Straight(dTimeDelta * m_fThrowPower);
	m_pTransformCom->Go_Vector(XMVectorSet(0.f, -1.f, 0.f, 0.f) , dTimeDelta * m_fJumpTime * m_fGravity);
	m_fJumpTime += (_float)dTimeDelta;


	return S_OK;
}

void CPoisonSmoke::Smoke_Effect()
{
	g_pGameInstance->StopSound(CSoundMgr::EFFECT8);
	g_pGameInstance->LetsPlaySound(TEXT("Smoke.mp3"), CSoundMgr::EFFECT8, 0.6f);

	CEffect::EFFECTDESC tEffectDesc;
	//tEffectDesc.eMove = CEffect::MOVE_STAND;
	//tEffectDesc.eType = CEffect::TYPE_MULTITEX;
	//tEffectDesc.eWay = CEffect::WAY_FINAL;dddddd
	//tEffectDesc.fSpriteSpeed = 31.3f;
	//tEffectDesc.iNumTexIndex = 2;
	//tEffectDesc.iPass = 22;
	//tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
	//tEffectDesc.pDead = nullptr;
	//tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
	//tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_SmokeGlow");
	//tEffectDesc.vScale = XMVectorSet(10.f, 10.f, 10.f, 0.f);

	//if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
	//{
	//	MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoisonSmoke::Smoke_Effect");
	//	return;
	//}

	tEffectDesc.eMove = CEffect::MOVE_STAND;
	tEffectDesc.eType = CEffect::TYPE_SPRITE;
	tEffectDesc.eWay = CEffect::WAY_FINAL;
	tEffectDesc.fSpriteSpeed = 1 / 0.032f * 1.5f;
	tEffectDesc.iSpriteNumX = 4;
	tEffectDesc.iSpriteNumY = 4;
	tEffectDesc.iSpriteStartX = 0;
	tEffectDesc.iSpriteStartY = 1;
	tEffectDesc.iNumTexIndex = 0;
	tEffectDesc.iPass = 22;
	tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 3.f, 0.f);
	tEffectDesc.pDead = nullptr;
	tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
	tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_PoisonSmoke");
	tEffectDesc.vScale = XMVectorSet(10.f, 10.f, 1.f, 0.f);
	tEffectDesc.fDownAlphaSpeed = 500.f;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoisonSmoke::Smoke_Effect(Smoke)");
		return;
	}

	tEffectDesc.iSpriteStartX = 0;
	tEffectDesc.iSpriteStartY = 1;
	tEffectDesc.MyWorldMatrix = XMMatrixTranslation(-2.f, 2.5f, -0.5f);
	tEffectDesc.vScale = XMVectorSet(5.f, 5.f, 1.f, 0.f);
	tEffectDesc.vDir = XMVectorSet(-1.f, 0.5f, -1.f, 0.f);
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoisonSmoke::Smoke_Effect(Smoke)");
		return;
	}

	tEffectDesc.iSpriteStartX = 1;
	tEffectDesc.iSpriteStartY = 1;
	tEffectDesc.MyWorldMatrix = XMMatrixTranslation(2.f, 2.f, -0.5f);
	tEffectDesc.vScale = XMVectorSet(7.f, 7.f, 1.f, 0.f);
	tEffectDesc.vDir = XMVectorSet(1.f, 0.5f, -1.f, 0.f);
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoisonSmoke::Smoke_Effect(Smoke)");
		return;
	}

	tEffectDesc.iSpriteStartX = 2;
	tEffectDesc.iSpriteStartY = 1;
	tEffectDesc.MyWorldMatrix = XMMatrixTranslation(-1.5f, 5.f, -0.4f);
	tEffectDesc.vScale = XMVectorSet(2.f, 2.f, 1.f, 0.f);
	tEffectDesc.vDir = XMVectorSet(-1.f, 1.f, -1.f, 0.f);
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoisonSmoke::Smoke_Effect(Smoke)");
		return;
	}

	tEffectDesc.iSpriteStartX = 3;
	tEffectDesc.iSpriteStartY = 1;
	tEffectDesc.MyWorldMatrix = XMMatrixTranslation(1.7f, 4.5f, -0.39f);
	tEffectDesc.vScale = XMVectorSet(3.f, 3.f, 1.f, 0.f);
	tEffectDesc.vDir = XMVectorSet(1.f, 1.f, -1.f, 0.f);
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoisonSmoke::Smoke_Effect(Smoke)");
		return;
	}


	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	// 터지는 파티클
	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 44;
	tParticleDesc.fMinSpeed = 15.f;
	tParticleDesc.fMaxSpeed = 20.f;
	tParticleDesc.fOriginScale = 0.09f;
	tParticleDesc.fScaleSpeed = 0.13f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 0.35f;
	tParticleDesc.fMakeRange = 0.f;
	tParticleDesc.fMaxDistance = 7.5f;
	tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
	tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	tParticleDesc.pParentPosition = &vPosition;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead = nullptr;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoisonSmoke::Smoke_Effect(Smoke)");
		return;
	}
}



CPoisonSmoke * CPoisonSmoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPoisonSmoke*	pInstance = new CPoisonSmoke(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPoisonSmoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPoisonSmoke::Clone(void * pArg)
{
	CPoisonSmoke*	pInstance = new CPoisonSmoke(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPoisonSmoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPoisonSmoke::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
