#include "stdafx.h"
#include "..\public\Ghost.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"
#include "HpBack.h"
#include "MonsterHpBar.h"
#include "Player_Hand.h"
#include "TakeAim.h"
#include "GhostBlinkEffect.h"
#include "Particle_Effect.h"
#include "Effect.h"
#include "MonsterSwordTrail.h"

CGhost::CGhost(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{

}

CGhost::CGhost(const CGhost & rhs)
	: CMonster(rhs)
{
}

HRESULT CGhost::Set_Navigation(const _tchar * pPrototypeTag)
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pNavigationCom);
	m_Components.erase(TEXT("Com_Navigation"));
	/* For.Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(g_eCurrentLevel, pPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CGhost::Set_Navigation(Navigation)");
		return E_FAIL;
	}
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return S_OK;
}

HRESULT CGhost::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CGhost::NativeContruct_Prototype");
		return E_FAIL;
	}

	m_tStatus.fMaxHp = 5000.f;
	m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
	m_tStatus.fMaxShield = 1200.f;
	m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
	m_tStatus.fAttack = 50.f;
	m_tStatus.fDefend = 10.f;

	return S_OK;
}

HRESULT CGhost::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CGhost::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CGhost::NativeConstruct");
		return E_FAIL;
	}

	return S_OK;
}

_int CGhost::Tick(_double dTimeDelta)
{
	if (true == m_bDead && true == m_bDissolve)
	{
		m_bDissolve = false;
		m_fDissolveAlpha = 0.f;

		m_eState = JUMP_DOWN;
		m_bAttack = false;
		m_bAttackCollision = false;

		m_dFireInterval = 0.0;
		m_dWalk = 0.0;

		m_pHpBack->Set_Dead(true);
		m_pHpBar->Set_Dead(true);
		m_pGuardBar->Set_Dead(true);
		if (nullptr != m_pSnipeRay)
		{
			m_pSnipeRay->Set_Dead(true);
			m_pSnipeRay = nullptr;
		}
		m_bDead = false;

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

		return RETURN_OBJECTPOOL;
	}
	if (false == m_bDissolve)
	{
		if (FAILED(Add_ElementIcon()))
		{
			MSGBOX("Add_ElementIcon returned E_FAIL in CGhost::Tick");
			return DEAD;
		}
	}
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CGhost::Tick");
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
			MSGBOX("State_Manage returned E_FAIL in CGhost::Tick");
			return DEAD;
		}

	if (true == m_bFreeze && false == m_bChain)
	{
		CChain::CHAINDESC tChainDesc;
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		//tChainDesc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		tChainDesc.pChain = &m_bChain;
		CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("aimatWeaknessPoint2");
		_matrix NodeWorldMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		tChainDesc.WorldMatrix = XMMatrixTranslation(NodeWorldMatrix.r[3].m128_f32[0], NodeWorldMatrix.r[3].m128_f32[1], NodeWorldMatrix.r[3].m128_f32[2]);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CGhost::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CGhost::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_LINE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(0.4f, 0.3f, 0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CGhost::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(-0.5f, 0.3f, -0.3f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CGhost::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(0.4f, 0.3f, -0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CGhost::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(-0.3f, 0.3f, 0.5f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CGhost::Ready_Layer_Effect");
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

_int CGhost::LateTick(_double dTimeDelta)
{
	if (true == m_bDissolve)
	{
		return S_OK;
	}
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CGhost::LateTick");
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
		MSGBOX("Collider_Update returned E_FAIL in CGhost::LateTick");
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
		MSGBOX("__super::SetUp_OnTerrain returned E_FAIL in CGhost::LateTick");
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




HRESULT CGhost::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pRendererCom)
	{
		MSGBOX("nullptr == m_pShaderCom or nullptr == m_pRendererCom in CGhost::Render");
		return E_FAIL;
	}

	if (FAILED(__super::SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CGhost::Render");
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

_int CGhost::Hit(_float fAttack, _float fCriticalValue, _bool bHeavy)
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

	if (nullptr != m_pSnipeRay)
	{
		m_pSnipeRay->Set_Dead(true);
		m_pSnipeRay = nullptr;
	}

	return (_int)fDamage;
}

HRESULT CGhost::StartSetting()
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
		MSGBOX("Add_GameObject returned E_FAIL in CGhost::StartSetting");
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
		MSGBOX("Add_GameObject returned E_FAIL in CGhost::StartSetting");
		return E_FAIL;
	}
	m_pHpBar = (CMonsterHpBar*)g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MonsterHpBar"));

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MonsterGuardBar"), TEXT("Prototype_GameObject_MonsterGuardBar"), &tHpBarDesc)))
	{
		MSGBOX("Add_GameObject returned E_FAIL in CGhost::StartSetting");
		return E_FAIL;
	}
	m_pGuardBar = (CMonsterHpBar*)g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MonsterGuardBar"));

	m_fPatrolRadius = 6.f;
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	INFO_UI tInfoUI = {};
	tInfoUI.fScaleX = 16.f;
	tInfoUI.fScaleY = 16.f;
	tInfoUI.fDepth = 3.f;
	tInfoUI.iTextureIndex = CMiniMap_Icon::TARGET_MONSTER;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon"), TEXT("Prototype_GameObject_MiniMap_Icon"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CGhost::NativeConstruct");
		return E_FAIL;
	}
	m_pMiniMap_Icon = static_cast<CMiniMap_Icon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon")));
	m_pMiniMap_Icon->Set_Target(m_pTransformCom);

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

HRESULT CGhost::Add_ElementIcon()
{
	if (nullptr == m_pHpBack)
	{
		MSGBOX("nullptr == m_pHpBack returned E_FAIL in CGhost::Add_ElementIcon");
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

HRESULT CGhost::SetUp_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Ghost"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CGhost::SetUp_Components(Model_Ghost)");
		return E_FAIL;
	}

	/* For.Com_Collder */

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(0.62f, 0.62f, 0.62f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Weak"), (CComponent**)&m_pWeakCollider, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CGhost::SetUp_Components(Collider_Weak)");
		return E_FAIL;
	}
	m_pWeakCollider->Set_Parent(this);

	CCollider::COLLIDERDESC tBodyColliderDesc;
	tBodyColliderDesc.vScale = _float3(1.35f, 1.35f, 1.35f);
	tBodyColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tBodyColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body"), (CComponent**)&m_pBodyCollider, &tBodyColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CGhost::SetUp_Components(Collider_Body)");
		return E_FAIL;
	}
	m_pBodyCollider->Set_Parent(this);

	CCollider::COLLIDERDESC tAttackColliderDesc;
	tAttackColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	tAttackColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tAttackColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack0"), (CComponent**)&m_pAttackCollider, &tAttackColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CGhost::SetUp_Components(Collider_Attack0)");
		return E_FAIL;
	}
	m_pAttackCollider->Set_Parent(this);

	return S_OK;
}

void CGhost::Stand_Combat(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float fDegree = XMConvertToDegrees(acos(XMVectorGetX(XMVector4Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVector4Normalize(XMVectorSetY(vPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f))))));
	if (10.f <= fDegree || -10.f >= fDegree)
		m_pTransformCom->LookTurn(m_pTransformCom->Get_State(CTransform::STATE_UP), vPlayerPosition, TimeDelta * 10.0);
	if (10.f > fDegree && -10.f < fDegree)
	{
		m_pTransformCom->LookAtXZ(vPlayerPosition);
		if (false == m_pModelCom->Get_EndAnimation(m_eState))
			return;

		m_eState = WEAPON_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CGhost::Walk_Front(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	if (3.f > m_fTargetDistance)
	{
		m_eState = ATTACK;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}

	m_pTransformCom->Go_Straight(TimeDelta * m_fSpeed, m_pNavigationCom);
	m_dWalk += TimeDelta;
	if (4.0 < m_dWalk)
	{
		m_eState = WEAPON_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}
}

void CGhost::Walk_Right(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	if (3.f > m_fTargetDistance)
	{
		m_eState = ATTACK;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}

	m_pTransformCom->Go_Right(TimeDelta * m_fSpeed, m_pNavigationCom);
	m_dWalk += TimeDelta;
	if (4.0 < m_dWalk)
	{
		m_eState = WEAPON_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}
}

void CGhost::Walk_Back(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	if (3.f > m_fTargetDistance)
	{
		m_eState = ATTACK;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}

	m_pTransformCom->Go_Backward(TimeDelta * m_fSpeed, m_pNavigationCom);
	m_dWalk += TimeDelta;
	if (4.0 < m_dWalk)
	{
		m_eState = WEAPON_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}
}

void CGhost::Walk_Left(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	if (3.f > m_fTargetDistance)
	{
		m_eState = ATTACK;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}

	m_pTransformCom->Go_Left(TimeDelta * m_fSpeed, m_pNavigationCom);
	m_dWalk += TimeDelta;
	if (4.0 < m_dWalk)
	{
		m_eState = WEAPON_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}
}

void CGhost::Patrol(_double TimeDelta)
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

void CGhost::Stand(_double TimeDelta)
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

void CGhost::Hit_Head(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = FLASH_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;


	}
}

void CGhost::Run(_double TimeDelta)
{
}

void CGhost::Jump_Up(_double TimeDelta)
{
}

void CGhost::Jump_Down(_double TimeDelta)
{
}

void CGhost::Attack(_double TimeDelta)
{
	if (false == m_bAttack && 0.533 < m_pModelCom->Get_PlayAcc() && 0.6 > m_pModelCom->Get_PlayAcc())
		m_bAttack = true;
	else if (true == m_bAttack && 0.6 <= m_pModelCom->Get_PlayAcc())
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
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(3.f);
	}
}

void CGhost::Weapon_Up(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);

	if (true == m_pModelCom->Get_EndAnimation(WEAPON_UP))
	{
		m_eState = WEAPON_KEEP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;

		CTakeAim::AIMDESC tAimDesc;
		tAimDesc.pModel = m_pModelCom;
		tAimDesc.pTransform = m_pTransformCom;
		tAimDesc.pSocketName = "muzzle";
		tAimDesc.pParent = this;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_SnipeRay"), TEXT("Prototype_GameObject_TakeAim"), &tAimDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CGhost::Weapon_Up");
			return;
		}
		m_pSnipeRay = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_SnipeRay"));
	}
}

void CGhost::Weapon_Keep(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);



	if (3.f > m_fTargetDistance)
	{
		m_eState = ATTACK;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dFireInterval = 0.0;
	}

	m_dFireInterval += TimeDelta;

	if (3.0 < m_dFireInterval)
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT11);
		g_pGameInstance->LetsPlaySound(TEXT("GhostFire.mp3"), CSoundMgr::EFFECT11, 0.4f);
		m_eState = FIRE2;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dFireInterval = 0.0;

		CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("muzzle");
		if (nullptr == pNode)
		{
			MSGBOX("nullptr == pNode in CGhost::Collider_Update");
			return;
		}

		_matrix TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[3].m128_f32[3] = 1.f;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_GhostBullet"), TEXT("Prototype_GameObject_GhostBullet"), &TransformMatrix.r[3])))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CGhost::Weapon_Keep");
			return;
		}
		m_pSnipeRay->Set_Dead(true);
		m_pSnipeRay = nullptr;
		_float4 vPosition;
		XMStoreFloat4(&vPosition, TransformMatrix.r[3]);

		// 총구 터지는 파티클
		CParticle_Effect::PARTICLEDESC tParticleDesc;
		tParticleDesc.iTexIndex = 4;
		tParticleDesc.fMinSpeed = 2.2f;
		tParticleDesc.fMaxSpeed = 7.f;
		tParticleDesc.fOriginScale = 0.11f;
		tParticleDesc.fScaleSpeed = 0.f;
		tParticleDesc.fGravityPower = 0.f;
		tParticleDesc.fLiveTime = 0.36f;
		tParticleDesc.fMakeRange = 0.f;
		tParticleDesc.fMaxDistance = 1.14f;
		tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
		tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
		tParticleDesc.pParentPosition = &vPosition;
		tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
		tParticleDesc.bUseEasing = false;
		tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
		tParticleDesc.bChase = false;
		tParticleDesc.bRoof = false;
		tParticleDesc.bTrail = false;
		tParticleDesc.fTrailInterval = 0.f;
		tParticleDesc.pDead = nullptr;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CGhost::Weapon_Keep");
			return;
		}

		_float4x4	MuzzleMatrix;
		XMStoreFloat4x4(&MuzzleMatrix, TransformMatrix);
		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.bNotLookCamera = false;
		tEffectDesc.eMove = CEffect::MOVE_STAND;
		tEffectDesc.eType = CEffect::TYPE_2D;
		tEffectDesc.eWay = CEffect::WAY_SCALEDOWN;
		tEffectDesc.fDownAlphaSpeed = 100.f;
		tEffectDesc.iPass = 24;
		_matrix MyMatrix = m_pTransformCom->Get_WorldMatrix();
		tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
		tEffectDesc.pDead = nullptr;
		tEffectDesc.pParentWorldMatrix = &MuzzleMatrix;
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_GhostBoom");
		tEffectDesc.vScale = XMVectorSet(5.f, 5.f, 5.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CGhost::Weapon_Keep");
			return;
		}
	}
}

void CGhost::Reload(_double TimeDelta)
{
	if (false == m_bReload && 1.7 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::MONSTER1);
		g_pGameInstance->LetsPlaySound(TEXT("reload.mp3"), CSoundMgr::MONSTER1, 0.3f);

		m_bReload = true;
	}
	if (true == m_pModelCom->Get_EndAnimation(RELOAD))
	{
		if (3.f > m_fTargetDistance)
			m_eState = ATTACK;
		if (GetAsyncKeyState('A') & 0x8000)
			m_eState = WALK_LEFT;
		else if (GetAsyncKeyState('D') & 0x8000)
			m_eState = WALK_RIGHT;
		else if (GetAsyncKeyState('S') & 0x8000)
			m_eState = WALK_FRONT;
		else
			m_eState = WALK_BACK;

		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(3.f);

		m_bReload = false;
	}
}

void CGhost::Fire(_double TimeDelta)
{
	m_pTransformCom->Go_Backward(TimeDelta * 0.3f, m_pNavigationCom);
	if (m_pModelCom->Get_EndAnimation(FIRE2))
	{
		m_eState = FLASH_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CGhost::Flash(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	if (m_pModelCom->Get_EndAnimation(FLASH))
	{
		_float3* pNotUse = nullptr;

		_int iDir = rand() % 2 == 0 ? -1 : 1;

		_vector vFlashPoint = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * (_float)iDir * 5.f;
		if (false == m_pNavigationCom->Move_OnNavigation(vFlashPoint, m_pTransformCom->Get_State(CTransform::STATE_POSITION), &pNotUse))
			vFlashPoint = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * -(_float)iDir * 5.f;
		if (false == m_pNavigationCom->Move_OnNavigation(vFlashPoint, m_pTransformCom->Get_State(CTransform::STATE_POSITION), &pNotUse))
			vFlashPoint = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 5.f;
		if (false == m_pNavigationCom->Move_OnNavigation(vFlashPoint, m_pTransformCom->Get_State(CTransform::STATE_POSITION), &pNotUse))
			vFlashPoint = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 5.f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFlashPoint);

		m_eState = FLASH_DOWN;
		m_pModelCom->SetUp_AnimIndex(m_eState);

		m_pGhostBlinkEffect->Set_EffectDown();
		m_pGhostBlinkEffect = nullptr;
	}
}

void CGhost::Flash_Up(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(FLASH_UP))
	{
		m_eState = FLASH;
		m_pModelCom->SetUp_AnimIndex(m_eState);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_GhostBlinkEffect"), m_pTransformCom->Get_WorldFloat4x4_Pointer())))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CGhost::Flash_Up");
			return;
		}
		m_pGhostBlinkEffect = static_cast<CGhostBlinkEffect*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Effect")));
	}
}

void CGhost::Flash_Down(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(FLASH_DOWN))
	{
		m_eState = RELOAD;
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}


void CGhost::Hit_Body(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = FLASH_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

HRESULT CGhost::Collider_Update()
{
	if (nullptr == m_pWeakCollider || nullptr == m_pBodyCollider)
	{
		MSGBOX("nullptr == m_pWeakCollider || nullptr == m_pBodyCollider in CGhost::Collider_Update");
		return E_FAIL;
	}
	CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("2164_eye");
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CGhost::Collider_Update");
		return E_FAIL;
	}

	_matrix			TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pWeakCollider->Update_Transform(TransformMatrix);

	pNode = m_pModelCom->Find_HierarchyNode("aimatWeaknessPoint2");
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CGhost::Collider_Update");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pBodyCollider->Update_Transform(TransformMatrix);

	if (true == m_bAttack)
	{
		pNode = m_pModelCom->Find_HierarchyNode("muzzle");
		if (nullptr == pNode)
		{
			MSGBOX("nullptr == pNode in CGhost::Collider_Update");
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

HRESULT CGhost::State_Manage(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nullptr in CGhost::State_Manage");
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
	case WEAPON_UP:
		Weapon_Up(TimeDelta);
		break;
	case WEAPON_KEEP:
		Weapon_Keep(TimeDelta);
		break;
	case FIRE2:
		Fire(TimeDelta);
		break;
	case FLASH:
		Flash(TimeDelta);
		break;
	case FLASH_UP:
		Flash_Up(TimeDelta);
		break;
	case FLASH_DOWN:
		Flash_Down(TimeDelta);
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
	case JUMP_UP:
		Jump_Up(TimeDelta);
		break;
	case JUMP_DOWN:
		Jump_Down(TimeDelta);
		break;
	case RELOAD:
		Reload(TimeDelta);
		break;
	case HIT_BODY:
		Hit_Body(TimeDelta);
		break;
	}

	return S_OK;
}

CGhost * CGhost::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGhost*	pInstance = new CGhost(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CGhost");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGhost::Clone(void * pArg)
{
	CGhost*	pInstance = new CGhost(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CGhost");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGhost::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBodyCollider);
	Safe_Release(m_pWeakCollider);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAttackCollider);
}