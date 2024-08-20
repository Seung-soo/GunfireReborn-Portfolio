#include "stdafx.h"
#include "..\public\Bullet_TigerCannon.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Monster.h"
#include "Damage.h"
#include "Effect.h"
#include "Particle_Effect.h"
#include "PoleMonarch.h"
#include "Player_Hand.h"

CBullet_TigerCannon::CBullet_TigerCannon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBullet_TigerCannon::CBullet_TigerCannon(const CBullet_TigerCannon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBullet_TigerCannon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBullet_TigerCannon::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBullet_TigerCannon::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 30.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBullet_TigerCannon::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBullet_TigerCannon::NativeConstruct");
		return E_FAIL;
	}
	if (nullptr != pArg)
		memcpy(&m_tBulletDesc, pArg, sizeof(BULLETDESC));



	CHierarchyNode* pNode = m_tBulletDesc.pModel->Find_HierarchyNode(m_tBulletDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tBulletDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tBulletDesc.pTransform->Get_WorldMatrix();
	_vector vCamPos = g_pGameInstance->Get_CamPosition();

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);
	m_vDir = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK);
	m_vDir = XMVector4Normalize(vCamPos + m_vDir * m_tBulletDesc.fDist - TransformMatrix.r[3]);

	m_pTransformCom->Scaled(XMVectorSet(2.f, 2.f, 2.f, 0.f));

	CEffect::EFFECTDESC tEffectDesc;
	tEffectDesc.eMove = CEffect::MOVE_CHASE;
	tEffectDesc.eType = CEffect::TYPE_2D;
	tEffectDesc.iPass = 24;
	tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, -0.01f);
	tEffectDesc.pDead = &m_bDead;
	tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
	tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_BulletGlow");
	tEffectDesc.vScale = XMVectorSet(4.f, 4.f, 1.f, 0.f);
	tEffectDesc.iNumTexIndex = 0;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_TigerCannon::NativeConstruct");
		return E_FAIL;
	}



	// 파티클 트레일

	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 1;
	tParticleDesc.fMinSpeed = 0.29f;
	tParticleDesc.fMaxSpeed = 0.41f;
	tParticleDesc.fOriginScale = 0.15f;
	tParticleDesc.fScaleSpeed = 0.13f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 6.f;
	tParticleDesc.fMakeRange = 0.33f;
	tParticleDesc.fMaxDistance = 7.5f;
	tParticleDesc.eDir = CParticle_Effect::DIR_ONE;
	tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	tParticleDesc.pParentPosition = &m_vPosition;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.fTrailInterval = 0.22f;
	tParticleDesc.pDead = &m_bDead;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
		return E_FAIL;
	}

	return S_OK;
}

_int CBullet_TigerCannon::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		_float4 vPos = {};
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_TigerCannon_Explosion"), TEXT("Prototype_GameObject_TigerCannon_Explosion"), &vPos);

		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.eMove = CEffect::MOVE_STAND;
		tEffectDesc.eType = CEffect::TYPE_SPRITE;
		tEffectDesc.eWay = CEffect::WAY_FINAL;
		tEffectDesc.fSpriteSpeed = 30.f;
		tEffectDesc.iPass = 20;
		tEffectDesc.iSpriteNumX = 5;
		tEffectDesc.iSpriteNumY = 2;
		tEffectDesc.iSpriteStartX = 0;
		tEffectDesc.iSpriteStartY = 0;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, -0.01f);
		tEffectDesc.pDead = nullptr;
		tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_WeaponExplotion");
		tEffectDesc.vScale = XMVectorSet(25.f, 25.f, 1.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_TigerCannon::Tick");
		}


		// 터지는 파티클
		CParticle_Effect::PARTICLEDESC tParticleDesc;
		tParticleDesc.iTexIndex = 1;
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
		tParticleDesc.pParentPosition = &m_vPosition;
		tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
		tParticleDesc.bUseEasing = true;
		tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
		tParticleDesc.bChase = false;
		tParticleDesc.bRoof = false;
		tParticleDesc.bTrail = false;
		tParticleDesc.fTrailInterval = 0.f;
		tParticleDesc.pDead = nullptr;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_Tesla::LateTick");
			return DEAD;
		}
		return DEAD;
	}
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBullet_TigerCannon::Tick");
		return E_FAIL;
	}
	m_fSprite += (_float)TimeDelta * 8.f;
	if (2.f <= m_fSprite)
	{
		m_fSprite = 0.f;
		m_iSpriteY++;
		if (2 <= m_iSpriteY)
			m_iSpriteY = 0;
	}

	if (0 > --m_iLiveTime)
	{
		m_bDead = true;
	}

	m_pTransformCom->Go_Vector(m_vDir, TimeDelta);

	_matrix TransformMatrix = m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pColliderCom->Update_Transform(TransformMatrix);

	m_fCurrentTime -= (_float)TimeDelta;
	if (0.f >= m_fCurrentTime)
	{
		m_fCurrentTime = m_fMakeSmokeTime;
		Make_Smoke();
	}

	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return _int();
}

_int CBullet_TigerCannon::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBullet_TigerCannon::LateTick");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CCollider*>* pWeakColliderList = pGameInstance->Get_ColliderList(TEXT("Weakness"), CCollision_Manager::TARGET_ENEMY);
	list<CCollider*>* pBodyColliderList = pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
	list<CCollider*>* pBulletColliderList = pGameInstance->Get_ColliderList(TEXT("Monster_Bullet"), CCollision_Manager::TARGET_ENEMY);

	if (nullptr != pBulletColliderList)
	{
		for (auto& pBulletCollider : *pBulletColliderList)
		{
			_float Dist = 0.f;
			if (true == pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pBulletCollider, &Dist))
			{
				pBulletCollider->Get_Parent()->Set_Dead(true);
			}
		}
	}
	if (nullptr != pWeakColliderList && nullptr != pBodyColliderList)
	{
		_bool bColl = false;
		for (auto& pWeakCollider : *pWeakColliderList)
		{
			if (true == pGameInstance->Collision_Sphere(m_pColliderCom, pWeakCollider))
			{
				
				bColl = true;
				m_bDead = true;
				CDamage::NUMBERDESC tNumberDesc;
				tNumberDesc.eType = CDamage::TYPE_CRITICAL;
				if (false == static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_UseEnergyOrb())
					tNumberDesc.iDamage = static_cast<CMonster*>(pWeakCollider->Get_Parent())->Hit(500.f, 3.f);
				else
				{
					tNumberDesc.iDamage = static_cast<CMonster*>(pWeakCollider->Get_Parent())->Hit(2000.f, 3.f);
					static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_UseEnergyOrb();
				}
				tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pWeakCollider->Get_Parent())->Get_Strong()))
					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_TigerCannon::LateTick");
						return DEAD;
					}
				break;
			}
		}
		for (auto& pBodyCollider : *pBodyColliderList)
		{
			if (true == bColl)
				break;
			if (true == pGameInstance->Collision_Sphere(m_pColliderCom, pBodyCollider))
			{
				CDamage::NUMBERDESC tNumberDesc;
				tNumberDesc.eType = CDamage::TYPE_ORIGIN;
				if (false == static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_UseEnergyOrb())
					tNumberDesc.iDamage = static_cast<CMonster*>(pBodyCollider->Get_Parent())->Hit(500.f, 1.f);
				else
				{
					tNumberDesc.iDamage = static_cast<CMonster*>(pBodyCollider->Get_Parent())->Hit(2000.f, 1.f);
					static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_UseEnergyOrb();
				}
				tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pBodyCollider->Get_Parent())->Get_Strong()))
					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_TigerCannon::LateTick");
						return DEAD;
					}
				m_bDead = true;
			}
		}
	}

	if (FAILED(Collision_Terrain()))
	{
		MSGBOX("Collision_Terrain returned E_FAIL in CBullet_TigerCannon::LateTick");
		return E_FAIL;
	}
	if (LEVEL_STAGE3_2 == g_eCurrentLevel && 3.05f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_bDead = true;
	else if (LEVEL_STAGE4_2 == g_eCurrentLevel && 41.5f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_bDead = true;

	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}




HRESULT CBullet_TigerCannon::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBullet_TigerCannon::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 6)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CBullet_TigerCannon::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBullet_TigerCannon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_TigerCannon::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_TigerCannon::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_TigerCannon::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bullet_TigerCannon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Texture_Bullet_TigerCannon)");
		return E_FAIL;
	}
	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Collider)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBullet_TigerCannon::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBullet_TigerCannon::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable");
		return E_FAIL;
	}

	_int iSprite = (_int)m_fSprite;


	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteIndex", &iSprite, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable(g_iSpriteIndex)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteYIndex", &m_iSpriteY, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable(g_iSpriteYIndex)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CBullet_TigerCannon::Collision_Terrain()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CBullet_TigerCannon::Collision_Terrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CBullet_TigerCannon::Collision_Terrain");
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
	if (fTerrainY > vLocalPosFloat3.y/* - fMyHalfScaleY*/)
		m_bDead = true;

	/////////////////////////////////////////////////////////////////////
	return S_OK;
}

void CBullet_TigerCannon::Make_Smoke()
{
	CEffect::EFFECTDESC tEffectDesc;
	tEffectDesc.eMove = CEffect::MOVE_STAND;
	tEffectDesc.eType = CEffect::TYPE_2D;
	tEffectDesc.eWay = CEffect::WAY_ALPHADOWN;
	tEffectDesc.fDownAlphaSpeed = 255.f;
	tEffectDesc.iPass = 23;
	tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.005f, _float(rand() % 101 - 50) * 0.005f, 0.f);
	tEffectDesc.pDead = &m_bDead;
	tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
	tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_WeaponSmoke");
	tEffectDesc.vScale = XMVectorSet(2.f, 2.f, 1.f, 0.f);
	tEffectDesc.iNumTexIndex = 0;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBullet_TigerCannon::Make_Smoke");
		return;
	}
}

CBullet_TigerCannon * CBullet_TigerCannon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBullet_TigerCannon*	pInstance = new CBullet_TigerCannon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBullet_TigerCannon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBullet_TigerCannon::Clone(void * pArg)
{
	CBullet_TigerCannon*	pInstance = new CBullet_TigerCannon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBullet_TigerCannon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBullet_TigerCannon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
