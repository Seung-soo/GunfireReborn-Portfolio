#include "stdafx.h"
#include "..\public\Lobstar.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"
#include "HpBack.h"
#include "MonsterHpBar.h"
#include "Player_Hand.h"
#include "LobstarBullet.h"
#include "LobstarSholder.h"
#include "Effect.h"
#include "MonsterSwordTrail.h"

CLobstar::CLobstar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{

}

CLobstar::CLobstar(const CLobstar & rhs)
	: CMonster(rhs)
{
}

HRESULT CLobstar::Set_Navigation(const _tchar * pPrototypeTag)
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pNavigationCom);
	m_Components.erase(TEXT("Com_Navigation"));
	/* For.Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(g_eCurrentLevel, pPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstar::Set_Navigation(Navigation)");
		return E_FAIL;
	}
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return S_OK;
}

HRESULT CLobstar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CLobstar::NativeContruct_Prototype");
		return E_FAIL;
	}

	m_tStatus.fMaxHp = 8000.f;
	m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
	m_tStatus.fMaxShield = 0.f;
	m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
	m_tStatus.fAttack = 60.f;
	m_tStatus.fDefend = 10.f;

	return S_OK;
}

HRESULT CLobstar::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CLobstar::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CLobstar::NativeConstruct");
		return E_FAIL;
	}


	return S_OK;
}

_int CLobstar::Tick(_double dTimeDelta)
{
	if (true == m_bDead && true == m_bDissolve)
	{
		m_bDissolve = false;
		m_fDissolveAlpha = 0.f;
		m_eState = JUMP_DOWN;

		m_bJump = false;
		m_iHowManyBomb = 8;
		m_dAttackInterval = 0.0;
		m_bAttack = false;
		m_bAttackCollision = false;

		m_bFire = false;
		m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
		m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
		m_iAnimationType = ANIM_ONETIME;


		m_pHpBack->Set_Dead(true);
		m_pHpBar->Set_Dead(true);
		m_pMiniMap_Icon->Set_Dead(true);
		m_pMiniMap_Icon = nullptr;
		Element_Reset();

		Safe_Release(m_pPlayerTransform);
		m_pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		Safe_AddRef(m_pPlayerTransform);

		m_bFreeze = false;
		m_fFreezeTime = 0.f;
		m_fCurrentFreezeTime = 0.f;
		m_bChain = false;
		m_bDead = false;

		m_iPass = 0;

		return RETURN_OBJECTPOOL;
	}
	if (false == m_bDissolve)
	{
		if (FAILED(Add_ElementIcon()))
		{
			MSGBOX("Add_ElementIcon returned E_FAIL in CLobstar::Tick");
			return DEAD;
		}
	}
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CLobstar::Tick");
		return DEAD;
	}
	if (true == m_bDissolve)
	{
		if (nullptr != m_pShoulder[0])
		{
			m_pShoulder[0]->Set_Dead(true);
			m_pShoulder[0] = nullptr;
		}
		if (nullptr != m_pShoulder[1])
		{
			m_pShoulder[1]->Set_Dead(true);
			m_pShoulder[1] = nullptr;
		}
		if (false == m_bDead)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		return S_OK;
	}

	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_fTargetDistance = XMVectorGetX(XMVector4Length(vPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	// 처음 등장할 때 점프 내려오면서 등장하고 점프가 끝나면 취할 행동을 정찰과 가만히 있는 것 둘 중에 하나로
	if (JUMP_DOWN == m_eState && m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = rand() % 2 == 0 ? PATROL : STAND;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(XMVectorSet(_float(rand() % 100 - 50.f), 0.f, _float(rand() % 100 - 50.f), 0.f)));
		m_pTransformCom->ArrangeMatrix();
		m_pTransformCom->Set_Speed(1.f);
		if (PATROL == m_eState)
			m_iAnimationType = ANIM_ROOF;
		else if (STAND == m_eState)
			m_iAnimationType = ANIM_ONETIME;
	}
	if (false == m_bFreeze)
		if (FAILED(State_Manage(dTimeDelta)))
		{
			MSGBOX("State_Manage returned E_FAIL in CLobstar::Tick");
			return DEAD;
		}

	if (true == m_bFreeze && false == m_bChain)
	{
		CChain::CHAINDESC tChainDesc;
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		//tChainDesc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		tChainDesc.pChain = &m_bChain;
		CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("Bip001");
		_matrix NodeWorldMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		tChainDesc.WorldMatrix = XMMatrixTranslation(NodeWorldMatrix.r[3].m128_f32[0], NodeWorldMatrix.r[3].m128_f32[1], NodeWorldMatrix.r[3].m128_f32[2]);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLobstar::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLobstar::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_LINE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(0.4f, 0.3f, 0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLobstar::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(-0.5f, 0.3f, -0.3f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLobstar::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(0.4f, 0.3f, -0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLobstar::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(-0.3f, 0.3f, 0.5f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLobstar::Ready_Layer_Effect");
			return E_FAIL;
		}
		m_bChain = true;
	}

	m_FrustumType = g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f);

	// 절두체
	if (DISJOINT != m_FrustumType)
	{
		g_pGameInstance->Add_Collider(TEXT("Weakness"), CCollision_Manager::TARGET_ENEMY, m_pWeakCollider);
		g_pGameInstance->Add_Collider(TEXT("Body"), CCollision_Manager::TARGET_ENEMY, m_pBodyCollider);
	}

	return NOPROBLEM;
}

_int CLobstar::LateTick(_double dTimeDelta)
{
	if (true == m_bDissolve)
	{
		return S_OK;
	}
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CLobstar::LateTick");
		return DEAD;
	}

	if (true == m_bUIRender)
	{
		m_pHpBack->Set_Render(true);
		m_pHpBar->Set_Render(true);
	}

	m_bUIRender = false;
	if (false == m_bFreeze)
		m_pModelCom->Update_CombinedTransformationMatrices(m_iAnimationType, dTimeDelta * m_fSpeed);

	if (FAILED(Collider_Update()))
	{
		MSGBOX("Collider_Update returned E_FAIL in CLobstar::LateTick");
		return DEAD;
	}

	if (CCell::TYPE_ROAD == m_pNavigationCom->Get_Type())
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos = XMVectorSetY(vPos, m_pNavigationCom->Compute_Y(vPos));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (false == m_bJump)
		if (FAILED(__super::SetUp_OnTerrain()))
		{
			MSGBOX("__super::SetUp_OnTerrain returned E_FAIL in CLobstar::LateTick");
			return E_FAIL;
		}

	// 절두체
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pWeakCollider);
		m_pRendererCom->Add_Debug(m_pBodyCollider);
		if (true == m_bAttack)
			m_pRendererCom->Add_Debug(m_pAttackCollider);
#endif
	}

	return NOPROBLEM;
}




HRESULT CLobstar::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pRendererCom)
	{
		MSGBOX("nullptr == m_pShaderCom or nullptr == m_pRendererCom in CLobstar::Render");
		return E_FAIL;
	}

	if (FAILED(__super::SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CLobstar::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

_int CLobstar::Hit(_float fAttack, _float fCriticalValue, _bool bHeavy)
{
	_float fDamage = fAttack * (1.f - m_tStatus.fDefend / (m_tStatus.fDefend + 100.f)) * fCriticalValue;
	fDamage += ((rand() % _int(fDamage * 0.2f)) - fDamage * 0.1f);

	if (true == static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_Talents()[0]
		&& (true == m_bBurning || true == m_bShock || true == m_bDecay || true == m_bManipulation || true == m_bCombustion || true == m_bMiasma))
		fDamage *= 1.6f;
	if (m_tStatus.fCurrentShield > 0)
	{
		fDamage /= 3.f;
		if (m_tStatus.fCurrentShield < fDamage)
		{
			fDamage -= m_tStatus.fCurrentShield;
			fDamage *= 3.f;

			m_tStatus.fCurrentShield = 0;
			m_tStatus.fCurrentHp -= fDamage;
		}
		else
		{
			m_tStatus.fCurrentShield -= fDamage;
		}
	}
	else
	{
		m_tStatus.fCurrentHp -= fDamage;
	}

	if (0.f > m_tStatus.fCurrentHp)
	{
		m_tStatus.fCurrentHp = 0.f;
		m_bDissolve = true;
		if (true == m_bBurning || true == m_bShock || true == m_bDecay || true == m_bManipulation || true == m_bCombustion || true == m_bMiasma)
			static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_Oil();
	}

	if (false == bHeavy)
		return (_int)fDamage;

	if (1.5f > fCriticalValue)
	{
		m_eState = HIT_BODY;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
	else
	{
		m_eState = HIT_HEAD;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
	return (_int)fDamage;
}

HRESULT CLobstar::StartSetting()
{
	m_pModelCom->SetUp_AnimIndex(m_eState);

	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_vPatrolPoint = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CHpBack::HPBACKDESC tHpBackDesc;
	tHpBackDesc.pModel = m_pModelCom;
	tHpBackDesc.pSocketName = "MonsterHp";
	tHpBackDesc.pTransform = m_pTransformCom;
	tHpBackDesc.pParent = this;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_HpBack"), TEXT("Prototype_GameObject_HpBack"), &tHpBackDesc)))
	{
		MSGBOX("Add_GameObject returned E_FAIL in CLobstar::StartSetting");
		return E_FAIL;
	}
	m_pHpBack = (CHpBack*)g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_HpBack"));

	CMonsterHpBar::HPBARDESC tHpBarDesc;
	tHpBarDesc.pModel = m_pModelCom;
	tHpBarDesc.pSocketName = "MonsterHp";
	tHpBarDesc.pTransform = m_pTransformCom;
	tHpBarDesc.pParent = this;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MonsterHpBar"), TEXT("Prototype_GameObject_MonsterHpBar"), &tHpBarDesc)))
	{
		MSGBOX("Add_GameObject returned E_FAIL in CLobstar::StartSetting");
		return E_FAIL;
	}
	m_pHpBar = (CMonsterHpBar*)g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MonsterHpBar"));


	m_fPatrolRadius = 6.f;

	INFO_UI tInfoUI = {};
	tInfoUI.fScaleX = 16.f;
	tInfoUI.fScaleY = 16.f;
	tInfoUI.fDepth = 3.f;
	tInfoUI.iTextureIndex = CMiniMap_Icon::TARGET_MONSTER;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon"), TEXT("Prototype_GameObject_MiniMap_Icon"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstar::NativeConstruct");
		return E_FAIL;
	}
	m_pMiniMap_Icon = static_cast<CMiniMap_Icon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon")));
	m_pMiniMap_Icon->Set_Target(m_pTransformCom);


	CLobstarSholder::SHOULDERDESC tShoulderDesc;
	tShoulderDesc.pModel = m_pModelCom;
	tShoulderDesc.pTransform = m_pTransformCom;
	tShoulderDesc.pParent = this;
	tShoulderDesc.pSocketName = "muzzle_L";
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Shoulder"), TEXT("Prototype_GameObject_LobstarShoulder"), &tShoulderDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstar::StartSetting");
		return E_FAIL;
	}
	m_pShoulder[0] = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Shoulder"));


	tShoulderDesc.pSocketName = "muzzle_R";
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Shoulder"), TEXT("Prototype_GameObject_LobstarShoulder"), &tShoulderDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstar::StartSetting");
		return E_FAIL;
	}
	m_pShoulder[1] = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Shoulder"));

	CMonsterSwordTrail::SWORDTRAIL	tSwordTrail;
	tSwordTrail.bUseNoise = false;
	tSwordTrail.fFirstUV = 0.f;
	tSwordTrail.fFirstUV_Y = 1.f;
	tSwordTrail.fTrailSpeed = 0.f;
	tSwordTrail.fTrailYSpeed = -1.f;
	tSwordTrail.pModelTag = TEXT("Prototype_Component_Model_AppearMonster");
	tSwordTrail.pParentTransform = m_pTransformCom;
	tSwordTrail.pTextureTag = TEXT("Prototype_Component_Texture_AppearMonster");
	tSwordTrail.RotationMatrix = XMMatrixIdentity();
	tSwordTrail.vTrailScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
	tSwordTrail.vTranslation = _float3(0.f, 0.f, 0.f);
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
	{
		__debugbreak();
		return E_FAIL;
	}
	tSwordTrail.RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(60.f));
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
	{
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLobstar::Add_ElementIcon()
{
	if (nullptr == m_pHpBack)
	{
		MSGBOX("nullptr == m_pHpBack returned E_FAIL in CLobstar::Add_ElementIcon");
		return E_FAIL;
	}

	if (true == m_bBurning && true == m_bBurningStart)
		m_pHpBack->Add_ElementIcon(CElementIcon::TYPE_BURNING);
	if (true == m_bShock && true == m_bShockStart)
		m_pHpBack->Add_ElementIcon(CElementIcon::TYPE_SHOCK);
	if (true == m_bDecay && true == m_bDecayStart)
		m_pHpBack->Add_ElementIcon(CElementIcon::TYPE_DECAY);
	if (true == m_bCombustion && true == m_bCombustionStart)
		m_pHpBack->Add_ElementIcon(CElementIcon::TYPE_COMBUSTION);
	if (true == m_bManipulation && true == m_bManipulationStart)
		m_pHpBack->Add_ElementIcon(CElementIcon::TYPE_MANIPULATION);
	if (true == m_bMiasma && true == m_bMiasmaStart)
		m_pHpBack->Add_ElementIcon(CElementIcon::TYPE_MIASMA);

	return S_OK;
}

HRESULT CLobstar::SetUp_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_Lobstar"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstar::SetUp_Components(Model_Lobstar)");
		return E_FAIL;
	}

	/* For.Com_Collder */

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(0.8f, 0.8f, 0.8f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Weak"), (CComponent**)&m_pWeakCollider, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstar::SetUp_Components(Collider_Weak)");
		return E_FAIL;
	}
	m_pWeakCollider->Set_Parent(this);

	CCollider::COLLIDERDESC tBodyColliderDesc;
	tBodyColliderDesc.vScale = _float3(2.1f, 2.1f, 2.1f);
	tBodyColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tBodyColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body"), (CComponent**)&m_pBodyCollider, &tBodyColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstar::SetUp_Components(Collider_Body)");
		return E_FAIL;
	}
	m_pBodyCollider->Set_Parent(this);

	CCollider::COLLIDERDESC tAttackColliderDesc;
	tAttackColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	tAttackColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tAttackColliderDesc.vTranslation = _float4(0.f, 1.29f, -2.61f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack0"), (CComponent**)&m_pAttackCollider, &tAttackColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstar::SetUp_Components(Collider_Attack0)");
		return E_FAIL;
	}
	m_pAttackCollider->Set_Parent(this);

	return S_OK;
}

void CLobstar::Stand_Combat(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float fDegree = XMConvertToDegrees(acos(XMVectorGetX(XMVector4Dot(XMVector4Normalize(XMVectorSetY(vPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f)), XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))))));
	if (10.f <= fDegree || -10.f >= fDegree)
		m_pTransformCom->LookTurn(m_pTransformCom->Get_State(CTransform::STATE_UP), vPlayerPosition, TimeDelta * 10.0);
	if (10.f > fDegree && -10.f < fDegree)
	{
		m_pTransformCom->LookAtXZ(vPlayerPosition);
		if (false == m_pModelCom->Get_EndAnimation(m_eState))
			return;

		if (8 == m_iHowManyBomb)
		{
			m_eState = FIRE_UP;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;
		}
		else
		{
			m_eState = RUN;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ROOF;
			m_pTransformCom->Set_Speed(6.f);
		}
	}
}

void CLobstar::Walk_Front(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);
	m_pTransformCom->Go_Straight(TimeDelta * m_fSpeed, m_pNavigationCom);

	if (15.f < m_fTargetDistance || m_pModelCom->Get_EndAnimation(WALK_FRONT))
	{
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

void CLobstar::Walk_Right(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);
	m_pTransformCom->Go_Right(TimeDelta * m_fSpeed, m_pNavigationCom);

	if (15.f < m_fTargetDistance || m_pModelCom->Get_EndAnimation(WALK_RIGHT))
	{
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

void CLobstar::Walk_Back(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);
	m_pTransformCom->Go_Backward(TimeDelta * m_fSpeed, m_pNavigationCom);

	if (15.f < m_fTargetDistance || m_pModelCom->Get_EndAnimation(WALK_BACK))
	{
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

void CLobstar::Walk_Left(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);
	m_pTransformCom->Go_Left(TimeDelta * m_fSpeed, m_pNavigationCom);

	if (15.f < m_fTargetDistance || m_pModelCom->Get_EndAnimation(WALK_LEFT))
	{
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

void CLobstar::Patrol(_double TimeDelta)
{
	if (PATROL == m_eState)
	{
		m_pTransformCom->Go_Straight(TimeDelta * m_fSpeed, m_pNavigationCom);
		if (m_fPatrolRadius < XMVectorGetX(XMVector4Length(m_vPatrolPoint - m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
		{
			m_pTransformCom->Go_Backward(TimeDelta * m_fSpeed, m_pNavigationCom);
			m_eState = STAND;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;
		}
	}

	if (30.f > m_fTargetDistance || true == m_pTargetFind)
	{
		m_pTargetFind = true;
		m_eState = STAND_COMBAT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CLobstar::Stand(_double TimeDelta)
{
	if (STAND == m_eState)
	{
		if (true == m_pModelCom->Get_EndAnimation(STAND))
		{
			m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(_float(rand() % 91 + 135)));
			m_eState = PATROL;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->Set_Speed(1.f);
			m_iAnimationType = ANIM_ROOF;
		}
	}

	if (30.f > m_fTargetDistance || true == m_pTargetFind)
	{
		m_pTargetFind = true;
		m_eState = STAND_COMBAT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CLobstar::Hit_Head(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

void CLobstar::Run(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);
	m_pTransformCom->Go_Straight(TimeDelta * m_fSpeed, m_pNavigationCom);
	m_dAttackInterval += TimeDelta;


	if (4.0 < m_dAttackInterval)
	{
		m_eState = RELOAD;
		m_pModelCom->SetUp_AnimIndex(RELOAD);
		m_iAnimationType = ANIM_ONETIME;
		m_dAttackInterval = 0.0;
	}
	else if (3.5f > m_fTargetDistance)
	{
		m_eState = ATTACK;
		m_pModelCom->SetUp_AnimIndex(ATTACK);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CLobstar::Jump_Up(_double TimeDelta)
{
}

void CLobstar::Jump_Down(_double TimeDelta)
{
}

void CLobstar::Attack(_double TimeDelta)
{
	if (false == m_bAttack && 0.766 < m_pModelCom->Get_PlayAcc() && 0.84 > m_pModelCom->Get_PlayAcc())
		m_bAttack = true;
	else if (true == m_bAttack && 0.84 <= m_pModelCom->Get_PlayAcc())
	{
		m_bAttack = false;
		m_bAttackCollision = false;
	}

	if (true == m_bAttack && false == m_bAttackCollision)
	{
		if (false == m_bManipulation)
		{
			CCollider* pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));
			m_bAttackCollision = g_pGameInstance->Collision_Sphere(m_pAttackCollider, pPlayerCollider);
			if (true == m_bAttackCollision)
				static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(m_tStatus.fAttack);
		}
		else
		{
			auto ColliderList = g_pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
			if (nullptr != ColliderList)
			{
				for (auto pCollider : *ColliderList)
				{
					if (m_pBodyCollider == pCollider)
						continue;
					m_bAttackCollision = g_pGameInstance->Collision_Sphere(m_pAttackCollider, pCollider);
					if (true == m_bAttackCollision)
						static_cast<CMonster*>(pCollider->Get_Parent())->Hit(m_tStatus.fAttack, 1.f);
				}
			}
		}
	}

	if (true == m_pModelCom->Get_EndAnimation(ATTACK))
	{
		if (GetAsyncKeyState('A') & 0x8000)
			m_eState = WALK_LEFT;
		else if (GetAsyncKeyState('D') & 0x8000)
			m_eState = WALK_RIGHT;
		else if (GetAsyncKeyState('S') & 0x8000)
			m_eState = WALK_FRONT;
		else
			m_eState = WALK_BACK;

		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_pTransformCom->Set_Speed(3.f);
	}
}

void CLobstar::Fire_Up(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Go_Vector(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * m_fSpeed, m_pNavigationCom);
	m_pTransformCom->LookAtXZ(vPlayerPosition);

	if (true == m_pModelCom->Get_EndAnimation(FIRE_UP))
	{
		if (m_iHowManyBomb % 2 == 0)
			m_eState = FIRE_SHOT1;
		else
			m_eState = FIRE_SHOT2;

		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CLobstar::Fire_Shot1(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);

	if (false == m_bFire && 0.2 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::MONSTER3);
		g_pGameInstance->LetsPlaySound(TEXT("LobstarFire.mp3"), CSoundMgr::MONSTER3, 0.3f);
		m_bFire = true;

		CHierarchyNode* pNode = nullptr;

		pNode = m_pModelCom->Find_HierarchyNode("muzzle_L");
		if (nullptr == pNode)
		{
			MSGBOX("nullptr == pNode in CLobstar::Fire_Shot1");
			return;
		}
		_matrix TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();

		CLobstarBullet::LOBSTARBULLETDESC tLobstarBulletDesc;
		tLobstarBulletDesc.vStartPoint = TransformMatrix.r[3];
		tLobstarBulletDesc.vTargetPoint = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_LobstarBullet"), TEXT("Prototype_GameObject_LobstarBullet"), &tLobstarBulletDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstar::Fire_Shot1");
			return;
		}
		if (true == m_bManipulation)
			static_cast<CLobstarBullet*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_LobstarBullet")))->Set_Manipulation(m_pTargetMonster);

		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.bNotLookCamera = false;
		tEffectDesc.eType = CEffect::TYPE_2D;
		tEffectDesc.eType = CEffect::TYPE_SPRITE;
		tEffectDesc.eWay = CEffect::WAY_FINAL;
		tEffectDesc.iSpriteNumX = 4;
		tEffectDesc.iSpriteNumY = 4;
		tEffectDesc.iSpriteStartX = 0;
		tEffectDesc.iSpriteStartY = 0;
		tEffectDesc.fSpriteSpeed = 25.f;
		tEffectDesc.iPass = 20;
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_MuzzleBomb");
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, 0.5f);
		tEffectDesc.pDead = nullptr;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, TransformMatrix);
		tEffectDesc.pParentWorldMatrix = &Matrix;
		tEffectDesc.vScale = XMVectorSet(4.f, 4.f, 1.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstarBullet::Tick");
			return;
		}
	}

	if (m_pModelCom->Get_EndAnimation(FIRE_SHOT1))
	{
		--m_iHowManyBomb;
		m_bFire = false;
		if (0 < m_iHowManyBomb)
		{
			m_eState = FIRE_SHOT2;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;
		}
		else
		{
			m_eState = STAND_COMBAT;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;
		}
	}
}

void CLobstar::Fire_Shot2(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);

	if (false == m_bFire && 0.2 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::MONSTER2);
		g_pGameInstance->LetsPlaySound(TEXT("LobstarFire.mp3"), CSoundMgr::MONSTER2, 0.3f);

		m_bFire = true;

		CHierarchyNode* pNode = nullptr;

		pNode = m_pModelCom->Find_HierarchyNode("muzzle_R");
		if (nullptr == pNode)
		{
			MSGBOX("nullptr == pNode in CLobstar::Fire_Shot1");
			return;
		}
		_matrix TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();

		CLobstarBullet::LOBSTARBULLETDESC tLobstarBulletDesc;
		tLobstarBulletDesc.vStartPoint = TransformMatrix.r[3];
		tLobstarBulletDesc.vTargetPoint = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_LobstarBullet"), TEXT("Prototype_GameObject_LobstarBullet"), &tLobstarBulletDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstar::Fire_Shot1");
			return;
		}
		if (true == m_bManipulation)
			static_cast<CLobstarBullet*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_LobstarBullet")))->Set_Manipulation(m_pTargetMonster);

		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.bNotLookCamera = false;
		tEffectDesc.eType = CEffect::TYPE_2D;
		tEffectDesc.eType = CEffect::TYPE_SPRITE;
		tEffectDesc.eWay = CEffect::WAY_FINAL;
		tEffectDesc.iSpriteNumX = 4;
		tEffectDesc.iSpriteNumY = 4;
		tEffectDesc.iSpriteStartX = 0;
		tEffectDesc.iSpriteStartY = 0;
		tEffectDesc.fSpriteSpeed = 25.f;
		tEffectDesc.iPass = 20;
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_MuzzleBomb");
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, 0.5f);
		tEffectDesc.pDead = nullptr;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, TransformMatrix);
		tEffectDesc.pParentWorldMatrix = &Matrix;
		tEffectDesc.vScale = XMVectorSet(4.f, 4.f, 1.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstarBullet::Tick");
			return;
		}
	}

	if (m_pModelCom->Get_EndAnimation(FIRE_SHOT2))
	{
		--m_iHowManyBomb;
		m_bFire = false;

		if (0 < m_iHowManyBomb)
		{
			m_eState = FIRE_SHOT1;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;
		}
		else
		{
			m_eState = STAND_COMBAT;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;
		}
	}
}

void CLobstar::Reload(_double TimeDelta)
{
	if (false == m_bReload && 2.5 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::MONSTER1);
		g_pGameInstance->LetsPlaySound(TEXT("LobstarReload.mp3"), CSoundMgr::MONSTER1, 0.3f);
		m_bReload = true;
	}
	if (m_pModelCom->Get_EndAnimation(RELOAD))
	{
		m_eState = FIRE_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_iHowManyBomb = 8;
		m_bReload = false;
	}
}

void CLobstar::Hit_Body(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

HRESULT CLobstar::Collider_Update()
{
	if (nullptr == m_pWeakCollider || nullptr == m_pBodyCollider)
	{
		MSGBOX("nullptr == m_pWeakCollider || nullptr == m_pBodyCollider in CLobstar::Collider_Update");
		return E_FAIL;
	}
	CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("Bip001 Head");
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CLobstar::Collider_Update");
		return E_FAIL;
	}

	_matrix			TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pWeakCollider->Update_Transform(TransformMatrix);

	pNode = m_pModelCom->Find_HierarchyNode("Bip001");
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CLobstar::Collider_Update");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pBodyCollider->Update_Transform(TransformMatrix);

	if (true == m_bAttack)
	{
		pNode = m_pModelCom->Find_HierarchyNode("RootNode");
		if (nullptr == pNode)
		{
			MSGBOX("nullptr == pNode in CLobstar::Collider_Update");
			return E_FAIL;
		}

		TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		m_pAttackCollider->Update_Transform(TransformMatrix);
	}

	return S_OK;
}

HRESULT CLobstar::State_Manage(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nullptr in CLobstar::State_Manage");
		return E_FAIL;
	}

	switch (m_eState)
	{
	case STAND_COMBAT:
		Stand_Combat(TimeDelta);
		break;
	case WALK_FRONT:
		Walk_Front(TimeDelta);
		break;
	case WALK_RIGHT:
		Walk_Right(TimeDelta);
		break;
	case WALK_BACK:
		Walk_Back(TimeDelta);
		break;
	case WALK_LEFT:
		Walk_Left(TimeDelta);
		break;
	case ATTACK:
		Attack(TimeDelta);
		break;
	case FIRE_UP:
		Fire_Up(TimeDelta);
		break;
	case FIRE_SHOT1:
		Fire_Shot1(TimeDelta);
		break;
	case FIRE_SHOT2:
		Fire_Shot2(TimeDelta);
		break;
	case RELOAD:
		Reload(TimeDelta);
		break;
	case PATROL:
		Patrol(TimeDelta);
		break;
	case STAND:
		Stand(TimeDelta);
		break;
	case HIT_HEAD:
		Hit_Head(TimeDelta);
		break;
	case RUN:
		Run(TimeDelta);
		break;
	case JUMP_UP:
		Jump_Up(TimeDelta);
		break;
	case JUMP_DOWN:
		Jump_Down(TimeDelta);
		break;
	case HIT_BODY:
		Hit_Body(TimeDelta);
		break;
	}

	return S_OK;
}

CLobstar * CLobstar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLobstar*	pInstance = new CLobstar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CLobstar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLobstar::Clone(void * pArg)
{
	CLobstar*	pInstance = new CLobstar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CLobstar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLobstar::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBodyCollider);
	Safe_Release(m_pWeakCollider);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAttackCollider);
}



