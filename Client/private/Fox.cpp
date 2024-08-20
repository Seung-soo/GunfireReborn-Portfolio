#include "stdafx.h"
#include "..\public\Fox.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"
#include "HpBack.h"
#include "MonsterHpBar.h"
#include "Effect.h"
#include "Particle_Effect.h"
#include "Player_Hand.h"
#include "MonsterSwordTrail.h"
CFox::CFox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{

}

CFox::CFox(const CFox & rhs)
	: CMonster(rhs)
{
}

HRESULT CFox::Set_Navigation(const _tchar * pPrototypeTag)
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pNavigationCom);
	m_Components.erase(TEXT("Com_Navigation"));
	/* For.Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(g_eCurrentLevel, pPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFox::Set_Navigation(Navigation)");
		return E_FAIL;
	}
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return S_OK;
}

HRESULT CFox::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CFox::NativeContruct_Prototype");
		return E_FAIL;
	}

	m_tStatus.fMaxHp = 4200.f;
	m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
	m_tStatus.fMaxShield = 1800.f;
	m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
	m_tStatus.fAttack = 50.f;
	m_tStatus.fDefend = 5.f;

	return S_OK;
}

HRESULT CFox::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CFox::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CFox::NativeConstruct");
		return E_FAIL;
	}

	return S_OK;
}

_int CFox::Tick(_double dTimeDelta)
{
	if (true == m_bDead && true == m_bDissolve)
	{
		m_bDissolve = false;
		m_fDissolveAlpha = 0.f;
		m_eState = JUMP_DOWN;

		m_dFireInterval = 0.0;
		m_dWalk = 0.0;
		m_iBullet = 6;

		m_pHpBack->Set_Dead(true);
		m_pHpBar->Set_Dead(true);
		m_pShieldBar->Set_Dead(true);

		m_bDead = false;

		m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
		m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
		m_iAnimationType = ANIM_ONETIME;

		m_pMiniMap_Icon->Set_Dead(true);
		m_pMiniMap_Icon = nullptr;

		m_bFreeze = false;
		m_fFreezeTime = 0.f;
		m_fCurrentFreezeTime = 0.f;
		m_bChain = false;

		return RETURN_OBJECTPOOL;
	}

	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CFox::Tick");
		return DEAD;
	}
	if (false == m_bDissolve)
	{
		if (FAILED(Add_ElementIcon()))
		{
			MSGBOX("Add_ElementIcon returned E_FAIL in CFox::Tick");
			return DEAD;
		}
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
			MSGBOX("State_Manage returned E_FAIL in CFox::Tick");
			return DEAD;
		}

	if (true == m_bFreeze && false == m_bChain)
	{
		CChain::CHAINDESC tChainDesc;
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		//tChainDesc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		tChainDesc.pChain = &m_bChain;
		CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("Bone034");
		_matrix NodeWorldMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		tChainDesc.WorldMatrix = XMMatrixTranslation(NodeWorldMatrix.r[3].m128_f32[0], NodeWorldMatrix.r[3].m128_f32[1], NodeWorldMatrix.r[3].m128_f32[2]);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CFox::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CFox::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_LINE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(0.4f, 0.3f, 0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CFox::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(-0.5f, 0.3f, -0.3f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CFox::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(0.4f, 0.3f, -0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CFox::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(-0.3f, 0.3f, 0.5f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CFox::Ready_Layer_Effect");
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

_int CFox::LateTick(_double dTimeDelta)
{
	if (true == m_bDissolve)
	{
		return S_OK;
	}
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CFox::LateTick");
		return DEAD;
	}

	if (true == m_bUIRender)
	{
		m_pHpBack->Set_Render(true);
		m_pHpBar->Set_Render(true);
		m_pShieldBar->Set_Render(true);
	}

	m_bUIRender = false;
	if (false == m_bFreeze)
		m_pModelCom->Update_CombinedTransformationMatrices(m_iAnimationType, dTimeDelta * m_fSpeed);

	if (FAILED(Collider_Update()))
	{
		MSGBOX("Collider_Update returned E_FAIL in CFox::LateTick");
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
		MSGBOX("__super::SetUp_OnTerrain returned E_FAIL in CFox::LateTick");
		return E_FAIL;
	}

	// 절두체
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pWeakCollider);
		m_pRendererCom->Add_Debug(m_pBodyCollider);
#endif
	}

	return NOPROBLEM;
}




HRESULT CFox::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pRendererCom)
	{
		MSGBOX("nullptr == m_pShaderCom or nullptr == m_pRendererCom in CFox::Render");
		return E_FAIL;
	}

	if (FAILED(__super::SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CFox::Render");
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

_int CFox::Hit(_float fAttack, _float fCriticalValue, _bool bHeavy)
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

HRESULT CFox::StartSetting()
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
		MSGBOX("Add_GameObject returned E_FAIL in CFox::StartSetting");
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
		MSGBOX("Add_GameObject returned E_FAIL in CFox::StartSetting");
		return E_FAIL;
	}
	m_pHpBar = (CMonsterHpBar*)g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MonsterHpBar"));

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MonsterShieldBar"), TEXT("Prototype_GameObject_MonsterShieldBar"), &tHpBarDesc)))
	{
		MSGBOX("Add_GameObject returned E_FAIL in CFox::StartSetting");
		return E_FAIL;
	}
	m_pShieldBar = (CMonsterHpBar*)g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MonsterShieldBar"));

	m_fPatrolRadius = 4.f;
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	INFO_UI tInfoUI = {};
	tInfoUI.fScaleX = 16.f;
	tInfoUI.fScaleY = 16.f;
	tInfoUI.fDepth = 3.f;
	tInfoUI.iTextureIndex = CMiniMap_Icon::TARGET_MONSTER;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon"), TEXT("Prototype_GameObject_MiniMap_Icon"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CFox::NativeConstruct");
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

HRESULT CFox::Add_ElementIcon()
{
	if (nullptr == m_pHpBack)
	{
		MSGBOX("nullptr == m_pHpBack returned E_FAIL in CFox::Add_ElementIcon");
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

HRESULT CFox::SetUp_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Fox"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFox::SetUp_Components(Model_Fox)");
		return E_FAIL;
	}

	/* For.Com_Collder */

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(0.8f, 0.8f, 0.8f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.1f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Weak"), (CComponent**)&m_pWeakCollider, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFox::SetUp_Components(Collider_Weak)");
		return E_FAIL;
	}
	m_pWeakCollider->Set_Parent(this);

	CCollider::COLLIDERDESC tBodyColliderDesc;
	tBodyColliderDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	tBodyColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tBodyColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body"), (CComponent**)&m_pBodyCollider, &tBodyColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFox::SetUp_Components(Collider_Body)");
		return E_FAIL;
	}
	m_pBodyCollider->Set_Parent(this);
	return S_OK;
}

void CFox::Stand_Combat(_double TimeDelta)
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

		m_eState = FIRE_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CFox::Walk_Front(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);
	m_pTransformCom->Go_Vector(m_vWalkDir, TimeDelta * m_fSpeed, m_pNavigationCom);
	m_dWalk += TimeDelta;
	if (4.0 < m_dWalk)
	{
		m_eState = FIRE_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}
}

void CFox::Walk_Right(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);
	m_pTransformCom->Go_Vector(m_vWalkDir, TimeDelta * m_fSpeed, m_pNavigationCom);
	m_dWalk += TimeDelta;
	if (4.0 < m_dWalk)
	{
		m_eState = FIRE_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}
}

void CFox::Walk_Back(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);
	m_pTransformCom->Go_Vector(m_vWalkDir, TimeDelta * m_fSpeed, m_pNavigationCom);
	m_dWalk += TimeDelta;
	if (4.0 < m_dWalk)
	{
		m_eState = FIRE_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}
}

void CFox::Walk_Left(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);
	m_pTransformCom->Go_Vector(m_vWalkDir, TimeDelta * m_fSpeed, m_pNavigationCom);
	m_dWalk += TimeDelta;
	if (4.0 < m_dWalk)
	{
		m_eState = FIRE_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_dWalk = 0.0;
	}
}

void CFox::Hide_Left(_double TimeDelta)
{
}

void CFox::Hide_Right(_double TimeDelta)
{
}

void CFox::Patrol(_double TimeDelta)
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

	if (20.f > m_fTargetDistance || true == m_pTargetFind)
	{
		m_pTargetFind = true;
		m_eState = STAND_COMBAT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CFox::Stand(_double TimeDelta)
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

	if (20.f > m_fTargetDistance || true == m_pTargetFind)
	{
		m_pTargetFind = true;
		m_eState = STAND_COMBAT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CFox::Hit_Head(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = FIRE_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CFox::Run(_double TimeDelta)
{
}

void CFox::Jump_Up(_double TimeDelta)
{
}

void CFox::Jump_Down(_double TimeDelta)
{
}

void CFox::Fire_Up(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);

	if (true == m_pModelCom->Get_EndAnimation(FIRE_UP))
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT12);
		g_pGameInstance->LetsPlaySound(TEXT("FoxFire.mp3"), CSoundMgr::EFFECT12, 0.3f);

		m_eState = FIRE;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;


		--m_iBullet;
		CHierarchyNode* pNode = nullptr;

		pNode = m_pModelCom->Find_HierarchyNode("muzzle");
		if (nullptr == pNode)
		{
			MSGBOX("nullptr == pNode in CFox::Fire");
			return;
		}
		_matrix TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
		TransformMatrix.r[3].m128_f32[3] = 1.f;
		_float4 vPos;
		memcpy(&vPos, &TransformMatrix.r[3], sizeof(_float4));
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_FoxBullet"), TEXT("Prototype_GameObject_FoxBullet"), &vPos)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CFox::Fire");
			return;
		}
		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.bNotLookCamera = false;
		tEffectDesc.eMove = CEffect::MOVE_CHASE;
		tEffectDesc.eType = CEffect::TYPE_2D;
		tEffectDesc.eWay = CEffect::WAY_SCALEDOWN;
		tEffectDesc.fDownAlphaSpeed = 80.f;
		tEffectDesc.iPass = 24;
		_matrix MyMatrix = m_pTransformCom->Get_WorldMatrix();
		tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
		tEffectDesc.pDead = &m_bDissolve;

		XMStoreFloat4x4(&m_MuzzleMatrix, TransformMatrix);
		m_MuzzleMatrix._14 = 0;
		m_MuzzleMatrix._24 = 0;
		m_MuzzleMatrix._34 = 0;

		tEffectDesc.pParentWorldMatrix = &m_MuzzleMatrix;
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_FoxMuzzle");
		tEffectDesc.vScale = XMVectorSet(4.f, 4.f, 1.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CFox::Fire_Up");
			return;
		}


		_float4 vPosition;
		XMStoreFloat4(&vPosition, TransformMatrix.r[3]);

		// 총구 터지는 파티클
		CParticle_Effect::PARTICLEDESC tParticleDesc;
		tParticleDesc.iTexIndex = 44;
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
		tParticleDesc.eParticleNumber = CParticle_Effect::NUM_30;
		tParticleDesc.bUseEasing = false;
		tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
		tParticleDesc.bChase = false;
		tParticleDesc.bRoof = false;
		tParticleDesc.bTrail = false;
		tParticleDesc.fTrailInterval = 0.f;
		tParticleDesc.pDead = nullptr;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CFox::Fire_Up");
			return;
		}
	}
}

void CFox::Reload(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);

	if (true == m_pModelCom->Get_EndAnimation(RELOAD))
	{
		if (GetAsyncKeyState('A') & 0x8000)
		{
			m_eState = WALK_LEFT;
			m_vWalkDir = -m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			m_eState = WALK_RIGHT;
			m_vWalkDir = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			m_eState = WALK_FRONT;
			m_vWalkDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		}
		else
		{
			m_eState = WALK_BACK;
			m_vWalkDir = -m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		}

		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(1.f);
	}
}

void CFox::Fire(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);

	CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("muzzle");
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CFox::Fire");
		return;
	}

	_matrix			TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
	XMStoreFloat4x4(&m_MuzzleMatrix, TransformMatrix);
	m_MuzzleMatrix._14 = 0.f;
	m_MuzzleMatrix._24 = 0.f;
	m_MuzzleMatrix._34 = 0.f;

	if (m_pModelCom->Get_EndAnimation(FIRE))
	{
		
		if (0 >= m_iBullet)
		{
			m_eState = RELOAD;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;
			m_iBullet = 6;
		}
		else
		{
			m_eState = FIRE_UP;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;
		}
	}
}

void CFox::Hit_Body(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = FIRE_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

HRESULT CFox::Collider_Update()
{
	if (nullptr == m_pWeakCollider || nullptr == m_pBodyCollider)
	{
		MSGBOX("nullptr == m_pWeakCollider || nullptr == m_pBodyCollider in CFox::Collider_Update");
		return E_FAIL;
	}
	CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("Bip001 Head");
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CFox::Collider_Update");
		return E_FAIL;
	}

	_matrix			TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pWeakCollider->Update_Transform(TransformMatrix);

	pNode = m_pModelCom->Find_HierarchyNode("Bone034");
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CFox::Collider_Update");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pBodyCollider->Update_Transform(TransformMatrix);

	return S_OK;
}

HRESULT CFox::State_Manage(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nullptr in CFox::State_Manage");
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
	case HIDE_LEFT:
		Hide_Left(TimeDelta);
		break;
	case HIDE_RIGHT:
		Hide_Right(TimeDelta);
		break;
	case FIRE_UP:
		Fire_Up(TimeDelta);
		break;
	case FIRE:
		Fire(TimeDelta);
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

CFox * CFox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CFox*	pInstance = new CFox(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CFox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFox::Clone(void * pArg)
{
	CFox*	pInstance = new CFox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CFox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFox::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBodyCollider);
	Safe_Release(m_pWeakCollider);
	Safe_Release(m_pModelCom);
}