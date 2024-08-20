#include "stdafx.h"
#include "..\public\Kappa.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"
#include "HpBack.h"
#include "MonsterHpBar.h"
#include "KappaFire.h"
#include "Player_Hand.h"
#include "MonsterSwordTrail.h"

CKappa::CKappa(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{

}

CKappa::CKappa(const CKappa & rhs)
	: CMonster(rhs)
{
}

HRESULT CKappa::Set_Navigation(const _tchar * pPrototypeTag)
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pNavigationCom);
	m_Components.erase(TEXT("Com_Navigation"));
	/* For.Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(g_eCurrentLevel, pPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CKappa::Set_Navigation(Navigation)");
		return E_FAIL;
	}
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

HRESULT CKappa::StartSetting()
{
	m_pModelCom->SetUp_AnimIndex(m_eState);
	m_pModelCom->SetUp_AnimIndex(m_eState);

	m_vPatrolPoint = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CHpBack::HPBACKDESC tHpBackDesc;
	tHpBackDesc.pModel = m_pModelCom;
	tHpBackDesc.pSocketName = "All_Root";
	tHpBackDesc.pTransform = m_pTransformCom;
	tHpBackDesc.pParent = this;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_HpBack"), TEXT("Prototype_GameObject_HpBack"), &tHpBackDesc)))
	{
		MSGBOX("Add_GameObject returned E_FAIL in CKappa::NativeConstruct");
		return E_FAIL;
	}
	m_pHpBack = (CHpBack*)g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_HpBack"));

	CMonsterHpBar::HPBARDESC tHpBarDesc;
	tHpBarDesc.pModel = m_pModelCom;
	tHpBarDesc.pSocketName = "All_Root";
	tHpBarDesc.pTransform = m_pTransformCom;
	tHpBarDesc.pParent = this;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MonsterHpBar"), TEXT("Prototype_GameObject_MonsterHpBar"), &tHpBarDesc)))
	{
		MSGBOX("Add_GameObject returned E_FAIL in CKappa::NativeConstruct");
		return E_FAIL;
	}
	m_pHpBar = (CMonsterHpBar*)g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MonsterHpBar"));

	m_fPatrolRadius = 6.f;
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	INFO_UI tInfoUI = {};
	tInfoUI.fScaleX = 16.f;
	tInfoUI.fScaleY = 16.f;
	tInfoUI.fDepth = 3.f;
	tInfoUI.iTextureIndex = CMiniMap_Icon::TARGET_MONSTER;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon"), TEXT("Prototype_GameObject_MiniMap_Icon"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CKappa::NativeConstruct");
		return E_FAIL;
	}
	m_pMiniMap_Icon = static_cast<CMiniMap_Icon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon")));
	m_pMiniMap_Icon->Set_Target(m_pTransformCom);

	CMonsterSwordTrail::SWORDTRAIL	tSwordTrail;
	tSwordTrail.bUseNoise = false;
	tSwordTrail.fFirstUV = 0.f;
	tSwordTrail.fFirstUV_Y = 1.f;
	tSwordTrail.fTrailSpeed = 0.f;
	tSwordTrail.fTrailYSpeed = -2.f;
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

HRESULT CKappa::Add_ElementIcon()
{
	if (nullptr == m_pHpBack)
	{
		MSGBOX("nullptr == m_pHpBack returned E_FAIL in CKappa::Add_ElementIcon");
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

HRESULT CKappa::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CKappa::NativeContruct_Prototype");
		return E_FAIL;
	}

	m_tStatus.fMaxHp = 5000.f;
	m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
	m_tStatus.fMaxShield = 0.f;
	m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
	m_tStatus.fAttack = 70.f;
	m_tStatus.fDefend = 5.f;

	return S_OK;
}

HRESULT CKappa::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CKappa::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CKappa::NativeConstruct");
		return E_FAIL;
	}

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, -1.f, 0.f));
	
	return S_OK;
}

_int CKappa::Tick(_double dTimeDelta)
{
	if (true == m_bDead && true == m_bDissolve)
	{
		m_bDissolve = false;
		m_fDissolveAlpha = 0.f;

		m_bDead = false;
		m_pHpBack->Set_Dead(true);
		m_pHpBar->Set_Dead(true);
		m_bJump = false;
		m_bScare = false;
		m_iFireIndex = 0;
		for (auto& pKappaFire : m_vKappaFire)
			pKappaFire->Set_Dead(true);
		m_vKappaFire.clear();
		m_eState = JUMP_DOWN;

		m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
		m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
		m_iAnimationType = ANIM_ONETIME;

		m_pMiniMap_Icon->Set_Dead(true);
		m_pMiniMap_Icon = nullptr;

		Element_Reset();

		m_bFreeze = false;
		m_fFreezeTime = 0.f;
		m_fCurrentFreezeTime = 0.f;
		m_bChain = false;

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, -1.f, 0.f));

		m_iPass = 0;


		return RETURN_OBJECTPOOL;
	}
	if (false == m_bDissolve)
	{
		if (FAILED(Add_ElementIcon()))
		{
			MSGBOX("Add_ElementIcon returned E_FAIL in CKappa::Tick");
			return DEAD;
		}
	}
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CKappa::Tick");
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
			MSGBOX("State_Manage returned E_FAIL in CKappa::Tick");
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

_int CKappa::LateTick(_double dTimeDelta)
{
	if (true == m_bDissolve)
	{
		return S_OK;
	}
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CKappa::LateTick");
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
		MSGBOX("Collider_Update returned E_FAIL in CKappa::LateTick");
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
			MSGBOX("__super::SetUp_OnTerrain returned E_FAIL in CKappa::LateTick");
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




HRESULT CKappa::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pRendererCom)
	{
		MSGBOX("nullptr == m_pShaderCom or nullptr == m_pRendererCom in CKappa::Render");
		return E_FAIL;
	}

	if (FAILED(__super::SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CKappa::Render");
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

_int CKappa::Hit(_float fAttack, _float fCriticalValue, _bool bHeavy)
{
	_float fDamage = fAttack * (1.f - m_tStatus.fDefend / (m_tStatus.fDefend + 100.f)) * fCriticalValue;
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

HRESULT CKappa::SetUp_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_Kappa"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CKappa::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Collder */

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(1.17f, 1.17f, 1.17f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Weak"), (CComponent**)&m_pWeakCollider, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CKappa::SetUp_Components(Collider_Weak)");
		return E_FAIL;
	}
	m_pWeakCollider->Set_Parent(this);

	CCollider::COLLIDERDESC tBodyColliderDesc;
	tBodyColliderDesc.vScale = _float3(1.54f, 1.54f, 1.54f);
	tBodyColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tBodyColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body"), (CComponent**)&m_pBodyCollider, &tBodyColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CKappa::SetUp_Components(Collider_Body)");
		return E_FAIL;
	}
	m_pBodyCollider->Set_Parent(this);


	return S_OK;
}

void CKappa::Stand_Combat(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float fDegree = XMConvertToDegrees(acos(XMVectorGetX(XMVector4Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVector4Normalize(XMVectorSetY(vPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f))))));
	if (10.f <= fDegree || -10.f >= fDegree)
		m_pTransformCom->LookTurn(m_pTransformCom->Get_State(CTransform::STATE_UP), vPlayerPosition, TimeDelta * 10.0);
	else if (10.f > fDegree && -10.f < fDegree)
	{
		m_pTransformCom->LookAtXZ(vPlayerPosition);
		if (false == m_pModelCom->Get_EndAnimation(m_eState))
			return;

		
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

void CKappa::Walk_Front(_double TimeDelta)
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

void CKappa::Walk_Right(_double TimeDelta)
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

void CKappa::Walk_Back(_double TimeDelta)
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

void CKappa::Walk_Left(_double TimeDelta)
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

void CKappa::Patrol(_double TimeDelta)
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

	if (30.f > m_fTargetDistance)
	{
		m_pTargetFind = true;
		m_eState = STAND_COMBAT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CKappa::Stand(_double TimeDelta)
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

	if (30.f > m_fTargetDistance)
	{
		m_pTargetFind = true;
		m_eState = STAND_COMBAT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

void CKappa::Hit_Head(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

void CKappa::Run(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	if ((true == m_bScare && 40.f > m_fTargetDistance) || 15.f > m_fTargetDistance)
	{
		if ((++m_iRunDirReset) % 90 == 0)
			m_pTransformCom->MatchIt_XZ(vPlayerPosition);
	}
	else
		m_pTransformCom->LookAtXZ(vPlayerPosition);

	m_pTransformCom->Go_Straight(TimeDelta * m_fSpeed, m_pNavigationCom);
	if (false == m_bScare && 20.f >= m_fTargetDistance && 15.f <= m_fTargetDistance)
	{
		m_eState = FIRE_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_pTransformCom->Set_Speed(3.f);
		m_bJump = true;
	}

	if (true == m_bScare && 40.f <= m_fTargetDistance)
	{
		m_eState = FIRE_UP;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_pTransformCom->Set_Speed(3.f);
		m_bJump = true;
	}
}

void CKappa::Jump_Up(_double TimeDelta)
{
}

void CKappa::Jump_Down(_double TimeDelta)
{
}

void CKappa::Fire_Up(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Go_Vector(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * m_fSpeed, m_pNavigationCom);
	m_pTransformCom->LookAtXZ(vPlayerPosition);

	if (true == m_pModelCom->Get_EndAnimation(FIRE_UP))
	{
		m_eState = FIRE_SHOT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;

		CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("RootNode");
		if (nullptr == pNode)
		{
			MSGBOX("nullptr == pNode in CKappa::Fire_Up");
			return;
		}

		_matrix LocalMatrix = XMMatrixIdentity();
		LocalMatrix.r[3] = XMVectorSet(0.f, 4.21f, 0.f, 1.f);

		_matrix TransformMatrix = LocalMatrix * XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_KappaFire"), TEXT("Prototype_GameObject_KappaFire"), &TransformMatrix)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CKappa::Fire_Up");
			return;
		}
		m_vKappaFire.push_back(static_cast<CKappaFire*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_KappaFire"))));


		LocalMatrix.r[3] = XMVectorSet(1.44f, 3.87f, 0.f, 1.f);

		TransformMatrix = LocalMatrix * XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_KappaFire"), TEXT("Prototype_GameObject_KappaFire"), &TransformMatrix)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CKappa::Fire_Up");
			return;
		}
		m_vKappaFire.push_back(static_cast<CKappaFire*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_KappaFire"))));

		LocalMatrix.r[3] = XMVectorSet(-2.57f, 2.03f, 0.f, 1.f);

		TransformMatrix = LocalMatrix * XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_KappaFire"), TEXT("Prototype_GameObject_KappaFire"), &TransformMatrix)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CKappa::Fire_Up");
			return;
		}
		m_vKappaFire.push_back(static_cast<CKappaFire*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_KappaFire"))));

		LocalMatrix.r[3] = XMVectorSet(-2.f, 0.67f, 1.24f, 1.f);

		TransformMatrix = LocalMatrix * XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_KappaFire"), TEXT("Prototype_GameObject_KappaFire"), &TransformMatrix)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CKappa::Fire_Up");
			return;
		}
		m_vKappaFire.push_back(static_cast<CKappaFire*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_KappaFire"))));

		LocalMatrix.r[3] = XMVectorSet(-1.8f, 2.64f, 1.73f, 1.f);

		TransformMatrix = LocalMatrix * XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_KappaFire"), TEXT("Prototype_GameObject_KappaFire"), &TransformMatrix)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CKappa::Fire_Up");
			return;
		}
		m_vKappaFire.push_back(static_cast<CKappaFire*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_KappaFire"))));

		LocalMatrix.r[3] = XMVectorSet(2.12f, 1.5f, 0.f, 1.f);

		TransformMatrix = LocalMatrix * XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_KappaFire"), TEXT("Prototype_GameObject_KappaFire"), &TransformMatrix)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CKappa::Fire_Up");
			return;
		}
		m_vKappaFire.push_back(static_cast<CKappaFire*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_KappaFire"))));

		LocalMatrix.r[3] = XMVectorSet(1.5f, 2.f, 0.95f, 1.f);

		TransformMatrix = LocalMatrix * XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_KappaFire"), TEXT("Prototype_GameObject_KappaFire"), &TransformMatrix)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CKappa::Fire_Up");
			return;
		}
		m_vKappaFire.push_back(static_cast<CKappaFire*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_KappaFire"))));

	}
}

void CKappa::Fire_Shot(_double TimeDelta)
{
	if (0 == g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_KappaFire")))
	{
		m_eState = FIRE_DOWN;
		m_pTransformCom->Set_Speed(10.f);
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_bScare = true;
		for (auto& pKappaFire : m_vKappaFire)
			pKappaFire->Set_Dead(true);
		m_vKappaFire.clear();
		return;
	}

	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);

	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = FIRE_SHOT2;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;

		if (0 < m_vKappaFire.size() && nullptr != m_vKappaFire[m_iFireIndex])
		{
			m_vKappaFire[m_iFireIndex]->Go_Fire(CElementFire::FIRETYPE(m_iFireIndex % 3));
			m_vKappaFire[m_iFireIndex] = nullptr;
			m_vKappaFire.erase(m_vKappaFire.begin());
			if (m_iFireIndex >= m_vKappaFire.size())
				m_iFireIndex = 0;
		}
	}


}

void CKappa::Fire_Shot2(_double TimeDelta)
{
	if (0 == g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_KappaFire")))
	{
		m_eState = FIRE_DOWN;
		m_pTransformCom->Set_Speed(10.f);
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_bScare = true;
		//for (auto& pKappaFire : m_vKappaFire)
		//	pKappaFire->Set_Dead(true);
		m_vKappaFire.clear();
		return;
	}

	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);

	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = FIRE_SHOT;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;

		if (0 < m_vKappaFire.size() && nullptr != m_vKappaFire[m_iFireIndex])
		{
			m_vKappaFire[m_iFireIndex]->Go_Fire(CElementFire::FIRETYPE(m_iFireIndex % 3));
			m_vKappaFire[m_iFireIndex] = nullptr;
			m_vKappaFire.erase(m_vKappaFire.begin());
			if (m_iFireIndex >= m_vKappaFire.size())
				m_iFireIndex = 0;
		}

	}


}

void CKappa::Fire_Down(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->LookAtXZ(vPlayerPosition);

	if (24.f < m_pModelCom->Get_PlayAcc() && 29.f > m_pModelCom->Get_PlayAcc())
		m_pTransformCom->Go_Vector(XMVectorSet(0.f, -1.f, 0.f, 0.f), TimeDelta * m_fSpeed, m_pNavigationCom);

	if (m_pModelCom->Get_EndAnimation(FIRE_DOWN))
	{
		m_bJump = false;

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
			m_eState = STAND_COMBAT;
			m_iAnimationType = ANIM_ONETIME;
		}
		m_pModelCom->SetUp_AnimIndex(m_eState);

		m_pTransformCom->Set_Speed(2.f);
	}
}

void CKappa::Hit_Body(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_eState = RUN;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);
	}
}

HRESULT CKappa::Collider_Update()
{
	if (nullptr == m_pWeakCollider || nullptr == m_pBodyCollider)
	{
		MSGBOX("nullptr == m_pWeakCollider || nullptr == m_pBodyCollider in CKappa::Collider_Update");
		return E_FAIL;
	}
	CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("Bone063");
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CKappa::Collider_Update");
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
		MSGBOX("nullptr == pNode in CKappa::Collider_Update");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pBodyCollider->Update_Transform(TransformMatrix);

	return S_OK;
}

HRESULT CKappa::State_Manage(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nullptr in CKappa::State_Manage");
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
	case FIRE_UP:
		Fire_Up(TimeDelta);
		break;
	case FIRE_SHOT:
		Fire_Shot(TimeDelta);
		break;
	case FIRE_SHOT2:
		Fire_Shot2(TimeDelta);
		break;
	case FIRE_DOWN:
		Fire_Down(TimeDelta);
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

CKappa * CKappa::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CKappa*	pInstance = new CKappa(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CKappa");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CKappa::Clone(void * pArg)
{
	CKappa*	pInstance = new CKappa(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CKappa");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKappa::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBodyCollider);
	Safe_Release(m_pWeakCollider);
	Safe_Release(m_pModelCom);
}



