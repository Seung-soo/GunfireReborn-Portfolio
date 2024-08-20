#include "stdafx.h"
#include "..\public\CatfishWarrior.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"
#include "HpBack.h"
#include "MonsterHpBar.h"
#include "Player_Hand.h"
#include "MonsterSwordTrail.h"

CCatfishWarrior::CCatfishWarrior(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{

}

CCatfishWarrior::CCatfishWarrior(const CCatfishWarrior & rhs)
	: CMonster(rhs)
{
}

HRESULT CCatfishWarrior::Set_Navigation(const _tchar * pPrototypeTag)
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pNavigationCom);
	m_Components.erase(TEXT("Com_Navigation"));
	/* For.Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(g_eCurrentLevel, pPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CCatfishWarrior::Set_Navigation(Navigation)");
		return E_FAIL;
	}
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return S_OK;
}

HRESULT CCatfishWarrior::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CCatfishWarrior::NativeContruct_Prototype");
		return E_FAIL;
	}

	m_tStatus.fMaxHp = 3000.f;
	m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
	m_tStatus.fMaxShield = 1500.f;
	m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
	m_tStatus.fAttack = 30.f;
	m_tStatus.fDefend = 5.f;

	return S_OK;
}

HRESULT CCatfishWarrior::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CCatfishWarrior::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CCatfishWarrior::NativeConstruct");
		return E_FAIL;
	}

	return S_OK;
}

_int CCatfishWarrior::Tick(_double dTimeDelta)
{
	if (true == m_bDead && true == m_bDissolve)
	{
		m_bDissolve = false;
		m_fDissolveAlpha = 0.f;

		m_bDead = false;

		m_eState = JUMP_DOWN;
		m_bAttack = false;
		m_bAttackCollision = false;

		m_pHpBack->Set_Dead(true);
		m_pHpBar->Set_Dead(true);
		m_pGuardBar->Set_Dead(true);
		m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
		m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
		m_iAnimationType = ANIM_ONETIME;

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
		m_iPass = 0;

		return RETURN_OBJECTPOOL;
	}
	if (false == m_bDissolve)
	{
		if (FAILED(Add_ElementIcon()))
		{
			MSGBOX("Add_ElementIcon returned E_FAIL in CCatfishWarrior::Tick");
			return DEAD;
		}
	}

	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CCatfishWarrior::Tick");
		return DEAD;
	}
	if (true == m_bDissolve)
	{
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
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(XMVectorSet(
			_float(rand() % 100 - 50.f), 0.f, _float(rand() % 100 - 50.f), 0.f)));
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
			MSGBOX("State_Manage returned E_FAIL in CCatfishWarrior::Tick");
			return DEAD;
		}

	if (true == m_bFreeze && false == m_bChain)
	{
		CChain::CHAINDESC tChainDesc;
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		//tChainDesc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		tChainDesc.pChain = &m_bChain;
		CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("CenterPoint");
		_matrix NodeWorldMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		tChainDesc.WorldMatrix = XMMatrixTranslation(NodeWorldMatrix.r[3].m128_f32[0], NodeWorldMatrix.r[3].m128_f32[1], NodeWorldMatrix.r[3].m128_f32[2]);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CCatfishWarrior::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CCatfishWarrior::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_LINE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(0.4f, 0.3f, 0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CCatfishWarrior::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(-0.5f, 0.3f, -0.3f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CCatfishWarrior::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(0.4f, 0.3f, -0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CCatfishWarrior::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(-0.3f, 0.3f, 0.5f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CCatfishWarrior::Ready_Layer_Effect");
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

_int CCatfishWarrior::LateTick(_double dTimeDelta)
{
	if (true == m_bDissolve)
	{
		return S_OK;
	}
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CCatfishWarrior::LateTick");
		return DEAD;
	}

	if (true == m_bUIRender)
	{
		m_pHpBack->Set_Render(true);
		m_pHpBar->Set_Render(true);
		m_pGuardBar->Set_Render(true);
	}

	m_bUIRender = false;
	if (false == m_bFreeze)
		m_pModelCom->Update_CombinedTransformationMatrices(m_iAnimationType, dTimeDelta * m_fSpeed);

	if (FAILED(Collider_Update()))
	{
		MSGBOX("Collider_Update returned E_FAIL in CCatfishWarrior::LateTick");
		return DEAD;
	}

	if (CCell::TYPE_ROAD == m_pNavigationCom->Get_Type())
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos = XMVectorSetY(vPos, m_pNavigationCom->Compute_Y(vPos));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (FAILED(__super::SetUp_OnTerrain()))
	{
		MSGBOX("__super::SetUp_OnTerrain returned E_FAIL in CCatfishWarrior::LateTick");
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
		{
			if (STAND_ATTACK == m_eState)
				m_pRendererCom->Add_Debug(m_pAttackCollider[0]);
			else if (RUN_ATTACK == m_eState)
				m_pRendererCom->Add_Debug(m_pAttackCollider[1]);
		}
#endif
	}

	return NOPROBLEM;
}




HRESULT CCatfishWarrior::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pRendererCom)
	{
		MSGBOX("nullptr == m_pShaderCom or nullptr == m_pRendererCom in CCatfishWarrior::Render");
		return E_FAIL;
	}

	if (FAILED(__super::SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CCatfishWarrior::Render");
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

_int CCatfishWarrior::Hit(_float fAttack, _float fCriticalValue, _bool bHeavy)
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

HRESULT CCatfishWarrior::StartSetting()
{
	m_pModelCom->SetUp_AnimIndex(m_eState);


	m_vPatrolPoint = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CHpBack::HPBACKDESC tHpBackDesc;
	tHpBackDesc.pModel = m_pModelCom;
	tHpBackDesc.pSocketName = "MonsterHp";
	tHpBackDesc.pTransform = m_pTransformCom;
	tHpBackDesc.pParent = this;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_HpBack"), TEXT("Prototype_GameObject_HpBack"), &tHpBackDesc)))
	{
		MSGBOX("Add_GameObject returned E_FAIL in CCatfishWarrior::StartSetting");
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
		MSGBOX("Add_GameObject returned E_FAIL in CCatfishWarrior::StartSetting");
		return E_FAIL;
	}
	m_pHpBar = (CMonsterHpBar*)g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MonsterHpBar"));

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MonsterGuardBar"), TEXT("Prototype_GameObject_MonsterGuardBar"), &tHpBarDesc)))
	{
		MSGBOX("Add_GameObject returned E_FAIL in CCatfishWarrior::NativeConstruct");
		return E_FAIL;
	}
	m_pGuardBar = (CMonsterHpBar*)g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MonsterGuardBar"));

	m_fPatrolRadius = 6.f;

	INFO_UI tInfoUI = {};
	tInfoUI.fScaleX = 16.f;
	tInfoUI.fScaleY = 16.f;
	tInfoUI.fDepth = 3.f;
	tInfoUI.iTextureIndex = CMiniMap_Icon::TARGET_MONSTER;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon"), TEXT("Prototype_GameObject_MiniMap_Icon"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CCatfishWarrior::NativeConstruct");
		return E_FAIL;
	}
	m_pMiniMap_Icon = static_cast<CMiniMap_Icon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon")));
	m_pMiniMap_Icon->Set_Target(m_pTransformCom);

	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

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

HRESULT CCatfishWarrior::Add_ElementIcon()
{
	if (nullptr == m_pHpBack)
	{
		MSGBOX("nullptr == m_pHpBack returned E_FAIL in CCatfishWarrior::Add_ElementIcon");
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

HRESULT CCatfishWarrior::SetUp_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_CatfishWarrior"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonster::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Collder */

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(0.85f, 0.85f, 0.85f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Weak"), (CComponent**)&m_pWeakCollider, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CCatfishWarrior::SetUp_Components(Collider_Weak)");
		return E_FAIL;
	}
	m_pWeakCollider->Set_Parent(this);

	CCollider::COLLIDERDESC tBodyColliderDesc;
	tBodyColliderDesc.vScale = _float3(1.25f, 1.25f, 1.25f);
	tBodyColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tBodyColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body"), (CComponent**)&m_pBodyCollider, &tBodyColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CCatfishWarrior::SetUp_Components(Collider_Body)");
		return E_FAIL;
	}
	m_pBodyCollider->Set_Parent(this);

	CCollider::COLLIDERDESC tAttackColliderDesc;
	tAttackColliderDesc.vScale = _float3(0.88f, 0.88f, 0.88f);
	tAttackColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tAttackColliderDesc.vTranslation = _float4(0.f, -0.2f, -1.45f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack0"), (CComponent**)&m_pAttackCollider[0], &tAttackColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CCatfishWarrior::SetUp_Components(Collider_Attack0)");
		return E_FAIL;
	}
	m_pAttackCollider[0]->Set_Parent(this);

	tAttackColliderDesc.vScale = _float3(0.88f, 0.88f, 0.88f);
	tAttackColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tAttackColliderDesc.vTranslation = _float4(0.f, -0.23f, -0.58f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack1"), (CComponent**)&m_pAttackCollider[1], &tAttackColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CCatfishWarrior::SetUp_Components(Collider_Attack1)");
		return E_FAIL;
	}
	m_pAttackCollider[1]->Set_Parent(this);
	return S_OK;
}

void CCatfishWarrior::Stand_Combat(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float fDegree = XMConvertToDegrees(acos(XMVectorGetX(XMVector4Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVector4Normalize(XMVectorSetY(vPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f))))));
	m_pTransformCom->LookTurn(m_pTransformCom->Get_State(CTransform::STATE_UP), vPlayerPosition, TimeDelta * 10.0);
	if (10.f > fDegree && -10.f < fDegree)
	{
		m_pTransformCom->LookAtXZ(vPlayerPosition);
		if (4.f > m_fTargetDistance)
			m_eState = STAND_ATTACK;
		else
			m_eState = HIDE_BACK;

		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CCatfishWarrior::Walk_Front(_double TimeDelta)
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

void CCatfishWarrior::Walk_Right(_double TimeDelta)
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

void CCatfishWarrior::Walk_Back(_double TimeDelta)
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

void CCatfishWarrior::Walk_Left(_double TimeDelta)
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

void CCatfishWarrior::Patrol(_double TimeDelta)
{
	if (PATROL == m_eState)
	{
		m_pTransformCom->Go_Straight(TimeDelta * m_fSpeed, m_pNavigationCom);
		if (m_fPatrolRadius < XMVectorGetX(XMVector4Length(m_vPatrolPoint 
			- m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
		{
			m_pTransformCom->Go_Backward(TimeDelta * m_fSpeed, m_pNavigationCom);
			m_eState = STAND;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;
		}
	}

	if (20.f > m_fTargetDistance)
	{
		m_pTargetFind = true;
		m_eState = STAND_COMBAT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
	}
}

void CCatfishWarrior::Stand(_double TimeDelta)
{
	if (STAND == m_eState)
	{
		if (true == m_pModelCom->Get_EndAnimation(STAND))
		{
			m_pTransformCom->NowRotation(m_pTransformCom
				->Get_State(CTransform::STATE_UP)
				, XMConvertToRadians(_float(rand() % 91 + 135)));
			m_eState = PATROL;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->Set_Speed(1.f);
			m_iAnimationType = ANIM_ROOF;
		}
	}

	if (20.f > m_fTargetDistance)
	{
		m_pTargetFind = true;
		m_eState = STAND_COMBAT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
	}
}

void CCatfishWarrior::Hit_Head(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

void CCatfishWarrior::Run(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform
		->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);
	m_pTransformCom->Go_Straight(TimeDelta * m_fSpeed, m_pNavigationCom);
	if (6.f >= m_fTargetDistance)
	{
		m_eState = RUN_ATTACK;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CCatfishWarrior::Jump_Up(_double TimeDelta)
{
}

void CCatfishWarrior::Jump_Down(_double TimeDelta)
{
}

void CCatfishWarrior::Hide_Right(_double TimeDelta)
{
}

void CCatfishWarrior::Hide_Left(_double TimeDelta)
{
}

void CCatfishWarrior::Hide_Back(_double TimeDelta)
{
	m_pTransformCom->Go_Backward(TimeDelta * m_fSpeed, m_pNavigationCom);
	if (true == m_pModelCom->Get_EndAnimation(HIDE_BACK))
	{
		if (GetAsyncKeyState('A') & 0x8000)
			m_eState = WALK_LEFT;
		else if (GetAsyncKeyState('D') & 0x8000)
			m_eState = WALK_RIGHT;
		else if (GetAsyncKeyState('W') & 0x8000)
			m_eState = WALK_BACK;
		else if (GetAsyncKeyState('S') & 0x8000)
			m_eState = WALK_FRONT;
		else
		{
			m_eState = RUN;
			m_iAnimationType = ANIM_ROOF;
			m_pTransformCom->Set_Speed(6.f);
		}
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}

void CCatfishWarrior::Run_Attack(_double TimeDelta)
{
	if (false == m_bAttack && 0.633 < m_pModelCom->Get_PlayAcc() && 0.733 > m_pModelCom->Get_PlayAcc())
	{
		m_bAttack = true;

		CMonsterSwordTrail::SWORDTRAIL tSwordTrail;
		tSwordTrail.RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(80.f)) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f),XMConvertToRadians(-30.f));
		tSwordTrail.fFirstUV = -0.5f;
		tSwordTrail.fTrailSpeed = 10.f;
		tSwordTrail.pModelTag = TEXT("Prototype_Component_Model_SwordTrail");
		tSwordTrail.pTextureTag = TEXT("Prototype_Component_Texture_CatfishWarriorTrail");
		tSwordTrail.pParentTransform = m_pTransformCom;
		tSwordTrail.vTrailScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		tSwordTrail.vTranslation = _float3(0.f, 1.8f, 0.5f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CCatfishWarrior::Run_Attack");
			return;
		}

		g_pGameInstance->StopSound(CSoundMgr::EFFECT9);
		g_pGameInstance->LetsPlaySound(TEXT("sword2.mp3"), CSoundMgr::EFFECT9, 0.3f);
	}
	else if (true == m_bAttack && 0.733 <= m_pModelCom->Get_PlayAcc())
	{
		m_bAttack = false;
		m_bAttackCollision = false;
	}

	if (true == m_bAttack && false == m_bAttackCollision)
	{
		if (false == m_bManipulation)
		{
			CCollider* pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));
			m_bAttackCollision = g_pGameInstance->Collision_Sphere(m_pAttackCollider[0], pPlayerCollider);
			if (true == m_bAttackCollision)
				static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(m_tStatus.fAttack);
			else
			{
				m_bAttackCollision = g_pGameInstance->Collision_Sphere(m_pAttackCollider[1], pPlayerCollider);
				if (true == m_bAttackCollision)
					static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(m_tStatus.fAttack);
			}
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
					m_bAttackCollision = g_pGameInstance->Collision_Sphere(m_pAttackCollider[0], pCollider);
					if (true == m_bAttackCollision)
						static_cast<CMonster*>(pCollider->Get_Parent())->Hit(m_tStatus.fAttack, 1.f);
					else
					{
						m_bAttackCollision = g_pGameInstance->Collision_Sphere(m_pAttackCollider[1], pCollider);
						if (true == m_bAttackCollision)
							static_cast<CMonster*>(pCollider->Get_Parent())->Hit(m_tStatus.fAttack, 1.f);
					}
				}
			}		
		}
	}

	if (0.6f > m_pModelCom->Get_PlayAcc())
		m_pTransformCom->Go_Straight(TimeDelta * m_fSpeed, m_pNavigationCom);
	else if (m_pModelCom->Get_EndAnimation(RUN_ATTACK))
	{
		m_eState = STAND_COMBAT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(3.f);
	}
}

void CCatfishWarrior::Stand_Attack(_double TimeDelta)
{
	if (false == m_bAttack && 0.415 < m_pModelCom->Get_PlayAcc() && 0.5 > m_pModelCom->Get_PlayAcc())
	{
		m_bAttack = true;
		CMonsterSwordTrail::SWORDTRAIL tSwordTrail;
		tSwordTrail.RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-20.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(50.f))/* * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-30.f))*/;
		tSwordTrail.fFirstUV = -1.f;
		tSwordTrail.fTrailSpeed = 10.f;
		tSwordTrail.pModelTag = TEXT("Prototype_Component_Model_SwordTrail");
		tSwordTrail.pTextureTag = TEXT("Prototype_Component_Texture_CatfishWarriorTrail");
		tSwordTrail.pParentTransform = m_pTransformCom;
		tSwordTrail.vTrailScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		tSwordTrail.vTranslation = _float3(0.f, 0.8f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CCatfishWarrior::Run_Attack");
			return;
		}
		g_pGameInstance->StopSound(CSoundMgr::EFFECT10);
		g_pGameInstance->LetsPlaySound(TEXT("sword4 (2).mp3"), CSoundMgr::EFFECT10, 0.3f);
	}
	else if (true == m_bAttack && 0.5 <= m_pModelCom->Get_PlayAcc())
	{
		m_bAttack = false;
		m_bAttackCollision = false;
	}

	if (true == m_bAttack && false == m_bAttackCollision)
	{
		if (false == m_bManipulation)
		{
			CCollider* pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));
			m_bAttackCollision = g_pGameInstance->Collision_Sphere(m_pAttackCollider[0], pPlayerCollider);
			if (true == m_bAttackCollision)
				static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(m_tStatus.fAttack);
			else
			{
				m_bAttackCollision = g_pGameInstance->Collision_Sphere(m_pAttackCollider[1], pPlayerCollider);
				if (true == m_bAttackCollision)
					static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(m_tStatus.fAttack);
			}
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
					m_bAttackCollision = g_pGameInstance->Collision_Sphere(m_pAttackCollider[0], pCollider);
					if (true == m_bAttackCollision)
						static_cast<CMonster*>(pCollider->Get_Parent())->Hit(m_tStatus.fAttack, 1.f);
					else
					{
						m_bAttackCollision = g_pGameInstance->Collision_Sphere(m_pAttackCollider[1], pCollider);
						if (true == m_bAttackCollision)
							static_cast<CMonster*>(pCollider->Get_Parent())->Hit(m_tStatus.fAttack, 1.f);
					}
				}
			}
		}
	}

	if (m_pModelCom->Get_EndAnimation(STAND_ATTACK))
	{
		m_eState = STAND_COMBAT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(3.f);
	}
}

void CCatfishWarrior::Hit_Body(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

HRESULT CCatfishWarrior::Collider_Update()
{
	if (nullptr == m_pWeakCollider || nullptr == m_pBodyCollider)
	{
		MSGBOX("nullptr == m_pWeakCollider || nullptr == m_pBodyCollider in CCatfishWarrior::Collider_Update");
		return E_FAIL;
	}
	CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("aimatWeaknessPoint");
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CCatfishWarrior::Collider_Update");
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
		MSGBOX("nullptr == pNode in CCatfishWarrior::Collider_Update");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pBodyCollider->Update_Transform(TransformMatrix);

	if (true == m_bAttack)
	{
		if (STAND_ATTACK == m_eState)
		{
			pNode = m_pModelCom->Find_HierarchyNode("Bip001 Prop1");
			if (nullptr == pNode)
			{
				MSGBOX("nullptr == pNode in CCatfishWarrior::Collider_Update");
				return E_FAIL;
			}

			TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

			m_pAttackCollider[0]->Update_Transform(TransformMatrix);
		}
		else if (RUN_ATTACK == m_eState)
		{
			pNode = m_pModelCom->Find_HierarchyNode("Bip001 Prop1");
			if (nullptr == pNode)
			{
				MSGBOX("nullptr == pNode in CCatfishWarrior::Collider_Update");
				return E_FAIL;
			}

			TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

			m_pAttackCollider[1]->Update_Transform(TransformMatrix);
		}
	}

	return S_OK;
}

HRESULT CCatfishWarrior::State_Manage(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nullptr in CCatfishWarrior::State_Manage");
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
	case HIDE_RIGHT:
		Hide_Right(TimeDelta);
		break;
	case HIDE_LEFT:
		Hide_Left(TimeDelta);
		break;
	case HIDE_BACK:
		Hide_Back(TimeDelta);
		break;
	case RUN_ATTACK:
		Run_Attack(TimeDelta);
		break;
	case STAND_ATTACK:
		Stand_Attack(TimeDelta);
		break;
	case HIT_BODY:
		Hit_Body(TimeDelta);
		break;
	}

	return S_OK;
}

CCatfishWarrior * CCatfishWarrior::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCatfishWarrior*	pInstance = new CCatfishWarrior(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CCatfishWarrior");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCatfishWarrior::Clone(void * pArg)
{
	CCatfishWarrior*	pInstance = new CCatfishWarrior(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CCatfishWarrior");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCatfishWarrior::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBodyCollider);
	Safe_Release(m_pWeakCollider);
	Safe_Release(m_pModelCom);
	for (_uint i = 0; i < 2; i++)
	{
		Safe_Release(m_pAttackCollider[i]);
	}
}



