#include "stdafx.h"
#include "..\public\Bullet_Tesla.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Monster.h"
#include "Damage.h"
#include "Particle_Effect.h"
#include "Effect.h"
#include "PoleMonarch.h"
#include "Player_Hand.h"

CBullet_Tesla::CBullet_Tesla(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBullet_Tesla::CBullet_Tesla(const CBullet_Tesla & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBullet_Tesla::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBullet_Tesla::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBullet_Tesla::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 40.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 20.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBullet_Tesla::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBullet_Tesla::NativeConstruct");
		return E_FAIL;
	}
	if (nullptr != pArg)
		memcpy(&m_tBulletDesc, pArg, sizeof(BULLETDESC));


	m_vDir = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK);

	CHierarchyNode* pNode = m_tBulletDesc.pModel->Find_HierarchyNode(m_tBulletDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tBulletDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tBulletDesc.pTransform->Get_WorldMatrix();
	_vector vCamPos = g_pGameInstance->Get_CamPosition();

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);         
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(vCamPos + m_vDir * m_tBulletDesc.fDist - TransformMatrix.r[3]));
	m_vDir = vCamPos + m_vDir * m_tBulletDesc.fDist - TransformMatrix.r[3];
	// 외적

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vCamPos))));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pTransformCom->Get_State(CTransform::STATE_RIGHT))));

	m_pTransformCom->Scaled(XMVectorSet(0.5f, 1.f, 1.f, 0.f));


	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_BulletGlow_Tesla"), this)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::NativeConstruct");
		return E_FAIL;
	}

	XMStoreFloat4(&m_vMyPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 32;
	tParticleDesc.fMinSpeed = 0.1f;
	tParticleDesc.fMaxSpeed = 0.2f;
	tParticleDesc.fOriginScale = 0.07f;
	tParticleDesc.fScaleSpeed = 0.f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 300.f;
	tParticleDesc.fMakeRange = 0.1f;
	tParticleDesc.fMaxDistance = 5.48f;
	tParticleDesc.eDir = CParticle_Effect::DIR_ONE;
	tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	tParticleDesc.pParentPosition = &m_vMyPosition;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead = &m_bDead;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::NativeConstruct");
		return E_FAIL;
	}

	return S_OK;
}

_int CBullet_Tesla::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBullet_Tesla::Tick");
		return E_FAIL;
	}
	m_fSprite += (_float)TimeDelta * 8.f;
	if (4.f <= m_fSprite)
		m_fSprite = 0.f;

	if (0 > --m_iLiveTime)
	{
		m_bDead = true;
	}

	m_pTransformCom->Go_Vector(m_vDir, TimeDelta);
	m_fZscale += 20.f * (_float)TimeDelta;
	m_pColliderCom->Set_ColliderTranslation(XMVectorSet(0.f, 0.f, m_fZscale * 0.375f, 1.f));

	_matrix TransformMatrix = m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pColliderCom->Update_Transform(TransformMatrix);

	XMStoreFloat4(&m_vMyPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	list<CCollider*>* pBulletColliderList = g_pGameInstance->Get_ColliderList(TEXT("Monster_Bullet"), CCollision_Manager::TARGET_ENEMY);

	if (nullptr != pBulletColliderList)
	{
		for (auto& pBulletCollider : *pBulletColliderList)
		{
			_float Dist = 0.f;
			if (true == g_pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pBulletCollider, &Dist))
			{
				pBulletCollider->Get_Parent()->Set_Dead(true);
			}
		}
	}

	return _int();
}

_int CBullet_Tesla::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBullet_Tesla::LateTick");
		return E_FAIL;
	}


	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - g_pGameInstance->Get_CamPosition()))));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pTransformCom->Get_State(CTransform::STATE_RIGHT))));
	m_pTransformCom->Scaled(XMVectorSet(0.5f, 1.f, m_fZscale, 0.f));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CCollider*>* pWeakColliderList = pGameInstance->Get_ColliderList(TEXT("Weakness"), CCollision_Manager::TARGET_ENEMY);
	list<CCollider*>* pBodyColliderList = pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);

	if (nullptr != pWeakColliderList && nullptr != pBodyColliderList)
	{
		for (auto& pWeakCollider : *pWeakColliderList)
		{
			if (true == pGameInstance->Collision_Sphere(m_pColliderCom, pWeakCollider))
			{
				g_pGameInstance->StopSound(CSoundMgr::EFFECT);
				g_pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT, 0.5f);
				pGameInstance->StopSound(CSoundMgr::EFFECT1);
				pGameInstance->LetsPlaySound(TEXT("TeslaHit.mp3"), CSoundMgr::EFFECT1, 0.7f);
				CDamage::NUMBERDESC tNumberDesc;
				tNumberDesc.eType = CDamage::TYPE_CRITICAL;
				if (false == static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_UseEnergyOrb())
					tNumberDesc.iDamage = static_cast<CMonster*>(pWeakCollider->Get_Parent())->Hit(200.f, 3.f);
				else
				{
					tNumberDesc.iDamage = static_cast<CMonster*>(pWeakCollider->Get_Parent())->Hit(800.f, 3.f);
					static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_UseEnergyOrb();
				}
				tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				_int iElement = rand() % 5;
				if (0 == iElement)
					static_cast<CMonster*>(pWeakCollider->Get_Parent())->Set_Shock(true);

				if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pWeakCollider->Get_Parent())->Get_Strong()))
					if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
						return DEAD;
					}
				if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_TeslaHit"), static_cast<CTransform*>(pWeakCollider->Get_Parent()->Get_Component(TEXT("Com_Transform"))))))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
					return DEAD;
				}
				if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_TeslaHit"), static_cast<CTransform*>(pWeakCollider->Get_Parent()->Get_Component(TEXT("Com_Transform"))))))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
					return DEAD;
				}
				if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_TeslaHit"), static_cast<CTransform*>(pWeakCollider->Get_Parent()->Get_Component(TEXT("Com_Transform"))))))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
					return DEAD;
				}

				_float4	vHitPos;
				XMStoreFloat4(&vHitPos, tNumberDesc.vPosition);
				// 피격 파티클
				CParticle_Effect::PARTICLEDESC tParticleDesc;
				tParticleDesc.iTexIndex = 32;
				tParticleDesc.fMinSpeed = 8.25f;
				tParticleDesc.fMaxSpeed = 13.35f;
				tParticleDesc.fOriginScale = 0.11f;
				tParticleDesc.fScaleSpeed = 0.f;
				tParticleDesc.fGravityPower = 43.f;
				tParticleDesc.fLiveTime = 0.5f;
				tParticleDesc.fMakeRange = 0.5f;
				tParticleDesc.fMaxDistance = 6.7f;
				tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
				tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
				tParticleDesc.pParentPosition = &vHitPos;
				tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
				tParticleDesc.bUseEasing = false;
				tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
				tParticleDesc.bChase = false;
				tParticleDesc.bRoof = false;
				tParticleDesc.bTrail = false;
				tParticleDesc.fTrailInterval = 0.f;
				tParticleDesc.pDead = nullptr;
				if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
					return DEAD;
				}

				CEffect::EFFECTDESC tEffectDesc;
				tEffectDesc.bNotLookCamera = false;
				tEffectDesc.eMove = CEffect::MOVE_STAND;
				tEffectDesc.eType = CEffect::TYPE_2D;
				tEffectDesc.eWay = CEffect::WAY_SCALEDOWN;
				tEffectDesc.fDownAlphaSpeed = 150.f;
				tEffectDesc.iPass = 24;
				_matrix MyMatrix = m_pTransformCom->Get_WorldMatrix();
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.1f, 0.2f);
				tEffectDesc.pDead = nullptr;
				tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
				tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_TeslaBoom");
				tEffectDesc.vScale = XMVectorSet(15.f, 15.f, 15.f, 0.f);
				if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
					return DEAD;
				}



				m_bDead = true;
				RELEASE_INSTANCE(CGameInstance);
				return S_OK;
			}
		}

		for (auto& pBodyCollider : *pBodyColliderList)
		{
			if (true == pGameInstance->Collision_Sphere(m_pColliderCom, pBodyCollider))
			{
				g_pGameInstance->StopSound(CSoundMgr::EFFECT);
				g_pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT, 0.5f);
				pGameInstance->StopSound(CSoundMgr::EFFECT1);
				pGameInstance->LetsPlaySound(TEXT("TeslaHit.mp3"), CSoundMgr::EFFECT1, 0.7f);
				CDamage::NUMBERDESC tNumberDesc;
				tNumberDesc.eType = CDamage::TYPE_ORIGIN;
				if (false == static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_UseEnergyOrb())
					tNumberDesc.iDamage = static_cast<CMonster*>(pBodyCollider->Get_Parent())->Hit(200.f, 1.f);
				else
				{
					tNumberDesc.iDamage = static_cast<CMonster*>(pBodyCollider->Get_Parent())->Hit(800.f, 1.f);
					static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_UseEnergyOrb();
				}
				tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				_int iElement = rand() % 5;
				if (0 == iElement)
					static_cast<CMonster*>(pBodyCollider->Get_Parent())->Set_Shock(true);

				if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pBodyCollider->Get_Parent())->Get_Strong()))
					if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
						return DEAD;
					}
				if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_TeslaHit"), static_cast<CTransform*>(pBodyCollider->Get_Parent()->Get_Component(TEXT("Com_Transform"))))))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
					return DEAD;
				}
				if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_TeslaHit"), static_cast<CTransform*>(pBodyCollider->Get_Parent()->Get_Component(TEXT("Com_Transform"))))))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
					return DEAD;
				}
				if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_TeslaHit"), static_cast<CTransform*>(pBodyCollider->Get_Parent()->Get_Component(TEXT("Com_Transform"))))))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
					return DEAD;
				}

				_float4	vHitPos;
				XMStoreFloat4(&vHitPos, tNumberDesc.vPosition);
				// 피격 파티클
				CParticle_Effect::PARTICLEDESC tParticleDesc;
				tParticleDesc.iTexIndex = 32;
				tParticleDesc.fMinSpeed = 8.25f;
				tParticleDesc.fMaxSpeed = 13.35f;
				tParticleDesc.fOriginScale = 0.11f;
				tParticleDesc.fScaleSpeed = 0.f;
				tParticleDesc.fGravityPower = 43.f;
				tParticleDesc.fLiveTime = 0.5f;
				tParticleDesc.fMakeRange = 0.5f;
				tParticleDesc.fMaxDistance = 6.7f;
				tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
				tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
				tParticleDesc.pParentPosition = &vHitPos;
				tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
				tParticleDesc.bUseEasing = false;
				tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
				tParticleDesc.bChase = false;
				tParticleDesc.bRoof = false;
				tParticleDesc.bTrail = false;
				tParticleDesc.fTrailInterval = 0.f;
				tParticleDesc.pDead = nullptr;
				if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
					return DEAD;
				}

				CEffect::EFFECTDESC tEffectDesc;
				tEffectDesc.bNotLookCamera = false;
				tEffectDesc.eMove = CEffect::MOVE_STAND;
				tEffectDesc.eType = CEffect::TYPE_2D;
				tEffectDesc.eWay = CEffect::WAY_SCALEDOWN;
				tEffectDesc.fDownAlphaSpeed = 150.f;
				tEffectDesc.iPass = 24;
				_matrix MyMatrix = m_pTransformCom->Get_WorldMatrix();
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.1f, 0.2f);
				tEffectDesc.pDead = nullptr;
				tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
				tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_TeslaBoom");
				tEffectDesc.vScale = XMVectorSet(15.f, 15.f, 15.f, 0.f);
				if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
					return DEAD;
				}

				m_bDead = true;
				RELEASE_INSTANCE(CGameInstance);

				return S_OK;
			}
		}
	}

	if (DISJOINT != pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
//#ifdef _DEBUG
//		m_pRendererCom->Add_Debug(m_pColliderCom);
//#endif
	}
	RELEASE_INSTANCE(CGameInstance);

	return _int();
}




HRESULT CBullet_Tesla::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBullet_Tesla::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 5)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CBullet_Tesla::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBullet_Tesla::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectXZ"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(VIBuffer_RectXZ)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bullet_Tesla"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Texture_Bullet_Tesla)");
		return E_FAIL;
	}
	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(0.25f, 0.25f, 0.25f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.375f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Collider)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBullet_Tesla::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBullet_Tesla::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBullet_Tesla::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_Tesla::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_Tesla::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBullet_Tesla::SetUp_ConstantTable");
		return E_FAIL;
	}

	_int iSprite = (_int)m_fSprite;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteIndex", &iSprite, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_Tesla::SetUp_ConstantTable(g_iSpriteIndex)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CBullet_Tesla * CBullet_Tesla::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBullet_Tesla*	pInstance = new CBullet_Tesla(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBullet_Tesla");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBullet_Tesla::Clone(void * pArg)
{
	CBullet_Tesla*	pInstance = new CBullet_Tesla(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBullet_Tesla");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBullet_Tesla::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
