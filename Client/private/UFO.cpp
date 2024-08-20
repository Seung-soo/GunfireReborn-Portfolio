#include "stdafx.h"
#include "..\public\UFO.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"
#include "HpBack.h"
#include "MonsterHpBar.h"
#include "MonsterWave.h"

CUFO::CUFO(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{

}

CUFO::CUFO(const CUFO & rhs)
	: CMonster(rhs)
{
}

HRESULT CUFO::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CUFO::NativeContruct_Prototype");
		return E_FAIL;
	}

	m_tStatus.fMaxHp = 15000.f;
	m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
	m_tStatus.fMaxShield = 0.f;
	m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
	m_tStatus.fAttack = 50.f;
	m_tStatus.fDefend = 40.f;

	return S_OK;
}

HRESULT CUFO::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CUFO::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CUFO::NativeConstruct");
		return E_FAIL;
	}

	m_bIsBoss = true;

	return S_OK;
}

_int CUFO::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
	{
		m_eState = PATROL;

		m_iBullet = 9;

		m_pHpBack->Set_Dead(true);
		m_pHpBar->Set_Dead(true);
		m_bDead = false;

		m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
		m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
		m_iAnimationType = ANIM_ROOF;

		m_pMiniMap_Icon->Set_Dead(true);
		m_pMiniMap_Icon = nullptr;

		Element_Reset();
		g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Aura"));

		m_bFreeze = false;
		m_fFreezeTime = 0.f;
		m_fCurrentFreezeTime = 0.f;
		m_bChain = false;

		return RETURN_OBJECTPOOL;
	}

	if (FAILED(Add_ElementIcon()))
	{
		MSGBOX("Add_ElementIcon returned E_FAIL in CUFO::Tick");
		return DEAD;
	}

	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CUFO::Tick");
		return DEAD;
	}
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_fTargetDistance = XMVectorGetX(XMVector4Length(vPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	if (false == m_bFreeze)
		if (FAILED(State_Manage(dTimeDelta)))
		{
			MSGBOX("State_Manage returned E_FAIL in CUFO::Tick");
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
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CUFO::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CUFO::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_LINE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(0.4f, 0.3f, 0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CUFO::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(-0.5f, 0.3f, -0.3f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CUFO::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(0.4f, 0.3f, -0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CUFO::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(-0.3f, 0.3f, 0.5f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CUFO::Ready_Layer_Effect");
			return E_FAIL;
		}
		m_bChain = true;
	}

	if (true == m_bDamage && STAND_COMBAT == m_eState)
	{
		m_fDamageInterval += (_float)dTimeDelta;
		if (2.f < m_fDamageInterval)
		{
			m_fDamageInterval = 0.f;
			m_bDamage = false;
			m_fDamageCount = 0.f;
		}
	}

	m_FrustumType = g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f);

	// 절두체
	if (DISJOINT != m_FrustumType)
	{
		g_pGameInstance->Add_Collider(TEXT("Weakness"), CCollision_Manager::TARGET_ENEMY, m_pWeakCollider);
		//g_pGameInstance->Add_Collider(TEXT("Body"), CCollision_Manager::TARGET_ENEMY, m_pBodyCollider);
	}

	return NOPROBLEM;
}

_int CUFO::LateTick(_double dTimeDelta)
{

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CUFO::LateTick");
		return DEAD;
	}

	if (true == m_bUIRender)
	{
		m_pHpBack->Set_Render(true);
		m_pHpBar->Set_Render(true);
	}

	m_bUIRender = false;
	if (false == m_bFreeze)
		m_pModelCom->Update_CombinedTransformationMatrices(m_iAnimationType, dTimeDelta  * m_fSpeed);

	if (FAILED(Collider_Update()))
	{
		MSGBOX("Collider_Update returned E_FAIL in CUFO::LateTick");
		return DEAD;
	}

	// 절두체
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pWeakCollider);
		m_pRendererCom->Add_Debug(m_pBuffCollider);
#endif
	}

	return NOPROBLEM;
}




HRESULT CUFO::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pRendererCom)
	{
		MSGBOX("nullptr == m_pShaderCom or nullptr == m_pRendererCom in CUFO::Render");
		return E_FAIL;
	}

	if (FAILED(__super::SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CUFO::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 0, i, "g_BoneMatrices");
	}

	return S_OK;
}

_int CUFO::Hit(_float fAttack, _float fCriticalValue, _bool bHeavy)
{
	m_bDamage = true;

	_float fDamage = fAttack * (1.f - m_tStatus.fDefend / (m_tStatus.fDefend + 100.f)) * fCriticalValue;
	if (m_tStatus.fCurrentShield > 0)
	{
		fDamage /= 3.f;
		if (m_tStatus.fCurrentShield < fDamage)
		{
			fDamage -= m_tStatus.fCurrentShield;
			fDamage *= 3.f;
			m_fDamageCount += fDamage;
			m_fDamageInterval = 0.f;
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
		m_fDamageCount += fDamage;
		m_fDamageInterval = 0.f;
		m_tStatus.fCurrentHp -= fDamage;
	}

	if (0.f >= m_tStatus.fCurrentHp)
	{
		m_tStatus.fCurrentHp = 0.f;
		m_eState = DIE;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;

		if (nullptr != g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_UFORay")) && 0 < g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_UFORay")))
		{
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_UFORay"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CUFO::Hit");
				return (_int)fDamage;
			}
		}
		if (nullptr != g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_Aura")) && 0 < g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_Aura")))
		{
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Aura"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CUFO::Hit");
				return (_int)fDamage;
			}
		}
	}

	return (_int)fDamage;
}

HRESULT CUFO::StartSetting()
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
		MSGBOX("Add_GameObject returned E_FAIL in CUFO::StartSetting");
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
		MSGBOX("Add_GameObject returned E_FAIL in CUFO::StartSetting");
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
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CUFO::NativeConstruct");
		return E_FAIL;
	}
	m_pMiniMap_Icon = static_cast<CMiniMap_Icon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon")));
	m_pMiniMap_Icon->Set_Target(m_pTransformCom);

	return S_OK;
}

HRESULT CUFO::Add_ElementIcon()
{
	if (nullptr == m_pHpBack)
	{
		MSGBOX("nullptr == m_pHpBack returned E_FAIL in CUFO::Add_ElementIcon");
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

HRESULT CUFO::SetUp_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_UFO"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFO::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Collder */

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(1.06f, 1.06f, 1.06f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Weak"), (CComponent**)&m_pWeakCollider, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFO::SetUp_Components(Collider_Weak)");
		return E_FAIL;
	}
	m_pWeakCollider->Set_Parent(this);

	tColliderDesc.vScale = _float3(20.f, 20.f, 20.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Buff"), (CComponent**)&m_pBuffCollider, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFO::SetUp_Components(Collider_Buff)");
		return E_FAIL;
	}
	m_pBuffCollider->Set_Parent(this);

	//CCollider::COLLIDERDESC tBodyColliderDesc;
	//tBodyColliderDesc.vScale = _float3(1.81f, 1.81f, 1.81f);
	//tBodyColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	//tBodyColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body"), (CComponent**)&m_pBodyCollider, &tBodyColliderDesc)))
	//{
	//	MSGBOX("__super::Add_Component returned E_FAIL in CBear::SetUp_Components(Collider_Body)");
	//	return E_FAIL;
	//}
	//m_pBodyCollider->Set_Parent(this);

	return S_OK;
}

void CUFO::Stand_Combat(_double TimeDelta)
{
	_vector vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_bool bInMonster = false;
	auto MonsterList = CMonsterWave::GetInstance()->Get_MonstersList();
	for (auto& pMonster : *MonsterList)
	{
		if (this == pMonster || true == static_cast<CMonster*>(pMonster)->Get_Buff())
			continue;

		CTransform* MonsterTransform = static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")));
		_float fDist = XMVectorGetX(XMVector4Length(XMVectorSetY(MonsterTransform->Get_State(CTransform::STATE_POSITION), 0.f) - XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f)));
		static_cast<CMonster*>(pMonster)->Set_Buff(true);
		if (fDist < 12.f)
		{
			bInMonster = true;
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Aura"), TEXT("Prototype_GameObject_Aura"), static_cast<CMonster*>(pMonster))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CUFO::Fire");
				return;
			}
		}
	}

	// 플레이어를 향해 돈다
	_float fDegree = XMConvertToDegrees(acos(XMVectorGetX(XMVector4Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVector4Normalize(XMVectorSetY(vPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f))))));
	m_pTransformCom->LookTurn(m_pTransformCom->Get_State(CTransform::STATE_UP), vPlayerPosition, TimeDelta * 10.0);
	if (10.f > fDegree && -10.f < fDegree)
	{
		m_pTransformCom->LookAtXZ(vPlayerPosition);
		if (m_tStatus.fMaxHp * 0.1f < m_fDamageCount || false == bInMonster)
		{
			auto MonsterList = CMonsterWave::GetInstance()->Get_MonstersList();
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vFarPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			_float	fFarDistance = 0.f;
			for (auto& pMonster : *MonsterList)
			{
				_vector MonsterPos = XMVectorSetY(static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION), XMVectorGetY(vMyPos));

				if (true == XMVector3Equal(vMyPos, MonsterPos))
					continue;

				if (fFarDistance < XMVectorGetX(XMVector3Length(MonsterPos - vMyPos)))
				{
					vFarPos = MonsterPos;
					fFarDistance = XMVectorGetX(XMVector3Length(MonsterPos - vMyPos));
				}
			}

			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_UFORay"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CUFO::Stand_Combat");
				return;
			}
			m_fDamageInterval = 0.f;
			m_bDamage = false;
			m_fDamageCount = 0.f;

			// 광선 없앰

			m_vTargetPos = vFarPos;

			m_eState = MOVE;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ROOF;
			m_pTransformCom->Set_Speed(6.f);
		}
	}
}

void CUFO::Patrol(_double TimeDelta)
{
	if (PATROL == m_eState)
	{
		m_pTransformCom->Go_Straight(TimeDelta * m_fSpeed);
		if (m_fPatrolRadius < XMVectorGetX(XMVector4Length(m_vPatrolPoint - m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
		{
			m_pTransformCom->Go_Backward(TimeDelta * m_fSpeed);
			m_eState = STAND;
			m_pModelCom->SetUp_AnimIndex(STAND);
			m_iAnimationType = ANIM_ONETIME;
		}
	}

	if (30.f > m_fTargetDistance)
	{
		auto MonsterList = CMonsterWave::GetInstance()->Get_MonstersList();
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vNearPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		_vector vFarPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		_float	fNearDistance = 30.f;
		_float	fFarDistance = 0.f;
		for (auto& pMonster : *MonsterList)
		{
			_vector MonsterPos = XMVectorSetY(static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION), XMVectorGetY(vMyPos));


			if (true == XMVector3Equal(vMyPos, MonsterPos))
				continue;

			if (fNearDistance > XMVectorGetX(XMVector3Length(MonsterPos - vMyPos)))
			{
				vNearPos = MonsterPos;
				fNearDistance = XMVectorGetX(XMVector3Length(MonsterPos - vMyPos));
			}
			if (fFarDistance < XMVectorGetX(XMVector3Length(MonsterPos - vMyPos)))
			{
				vFarPos = MonsterPos;
				fFarDistance = XMVectorGetX(XMVector3Length(MonsterPos - vMyPos));
			}
		}
		if (false == m_pTargetFind)
			m_vTargetPos = vNearPos;
		else
			m_vTargetPos = vFarPos;

		m_eState = MOVE;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);

		m_pTargetFind = true;

	}
}

void CUFO::Stand(_double TimeDelta)
{
	if (STAND == m_eState)
	{
		if (true == m_pModelCom->Get_EndAnimation(STAND))
		{
			m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(_float(rand() % 91 + 135)));
			m_eState = PATROL;
			m_pModelCom->SetUp_AnimIndex(PATROL);
			m_pTransformCom->Set_Speed(3.f);
			m_iAnimationType = ANIM_ROOF;
		}
	}

	if (30.f > m_fTargetDistance)
	{

		auto MonsterList = CMonsterWave::GetInstance()->Get_MonstersList();
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vNearPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		_vector vFarPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		_float	fNearDistance = 30.f;
		_float	fFarDistance = 0.f;
		for (auto& pMonster : *MonsterList)
		{
			_vector MonsterPos = XMVectorSetY(static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION), XMVectorGetY(vMyPos));

			if (true == XMVector3Equal(vMyPos, MonsterPos))
				continue;

			if (fNearDistance > XMVectorGetX(XMVector3Length(MonsterPos - vMyPos)))
			{
				vNearPos = MonsterPos;
				fNearDistance = XMVectorGetX(XMVector3Length(MonsterPos - vMyPos));
			}
			if (fFarDistance < XMVectorGetX(XMVector3Length(MonsterPos - vMyPos)))
			{
				vFarPos = MonsterPos;
				fFarDistance = XMVectorGetX(XMVector3Length(MonsterPos - vMyPos));
			}
		}
		if (false == m_pTargetFind)
			m_vTargetPos = vNearPos;
		else
			m_vTargetPos = vFarPos;

		m_eState = MOVE;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ROOF;
		m_pTransformCom->Set_Speed(6.f);

		m_pTargetFind = true;

	}
}

void CUFO::Move(_double TimeDelta)
{
	_vector vDir = m_vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Go_Vector(XMVector4Normalize(m_vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)), TimeDelta * m_fSpeed);

	if (0.1f > XMVectorGetX(XMVector3Length(vDir)))
	{
		m_eState = FIRE;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
	}
}

HRESULT CUFO::Collider_Update()
{
	if (nullptr == m_pWeakCollider/* || nullptr == m_pBodyCollider*/)
	{
		MSGBOX("nullptr == m_pWeakCollider || nullptr == m_pBodyCollider in CUFO::Collider_Update");
		return E_FAIL;
	}
	CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("CenterPoint");
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CUFO::Collider_Update");
		return E_FAIL;
	}

	_matrix			TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pWeakCollider->Update_Transform(TransformMatrix);
	m_pBuffCollider->Update_Transform(TransformMatrix);

	//pNode = m_pModelCom->Find_HierarchyNode("Bip001");
	//if (nullptr == pNode)
	//{
	//	MSGBOX("nullptr == pNode in CUFO::Collider_Update");
	//	return E_FAIL;
	//}

	//TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	//TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	//TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	//TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	//m_pBodyCollider->Update_Transform(TransformMatrix);

	return S_OK;
}

void CUFO::Fire(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(FIRE))
	{
		--m_iBullet;

		if (0 >= m_iBullet)
		{
			m_eState = STAND_COMBAT;
			m_pModelCom->SetUp_AnimIndex(STAND_COMBAT);
			m_iAnimationType = ANIM_ROOF;
			m_pTransformCom->Set_Speed(3.f);
			m_iBullet = 9;

			auto MonsterList = CMonsterWave::GetInstance()->Get_MonstersList();
			for (auto& pMonster : *MonsterList)
			{
				if (this == pMonster)
					continue;

				CTransform* MonsterTransform = static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")));
				_float fDist = XMVectorGetX(XMVector4Length(XMVectorSetY(MonsterTransform->Get_State(CTransform::STATE_POSITION), 0.f) - XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f)));
				static_cast<CMonster*>(pMonster)->Set_Buff(true);
				if (fDist < 12.f)
				{
					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Aura"), TEXT("Prototype_GameObject_Aura"), static_cast<CMonster*>(pMonster))))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CUFO::Fire");
						return;
					}
				}
			}
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_UFORay"), TEXT("Prototype_GameObject_UFORay"), m_pTransformCom)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CUFO::Fire");
				return;
			}
		}
		else
		{
			m_eState = FIRE;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;
		}

		CHierarchyNode* pNode = nullptr;
		if (m_iBullet % 3 == 1)
			pNode = m_pModelCom->Find_HierarchyNode("muzzle1");
		else if (m_iBullet % 3 == 2)
			pNode = m_pModelCom->Find_HierarchyNode("muzzle2");
		else if (m_iBullet % 3 == 0)
			pNode = m_pModelCom->Find_HierarchyNode("muzzle3");
		if (nullptr == pNode)
		{
			MSGBOX("nullptr == pNode in CUFO::Fire");
			return;
		}
		_matrix TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_2, TEXT("Layer_UFOBullet"), TEXT("Prototype_GameObject_UFOBullet"), &TransformMatrix)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CUFO::Fire");
			return;
		}
	}
}

void CUFO::Die(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(m_eState))
	{
		m_bDead = true;
	}
}

HRESULT CUFO::State_Manage(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nullptr in CUFO::State_Manage");
		return E_FAIL;
	}

	switch (m_eState)
	{
	case STAND_COMBAT:
		Stand_Combat(TimeDelta);
		break;
	case PATROL:
		Patrol(TimeDelta);
		break;
	case MOVE:
		Move(TimeDelta);
		break;
	case STAND:
		Stand(TimeDelta);
		break;
	case FIRE:
		Fire(TimeDelta);
		break;
	case DIE:
		Die(TimeDelta);
		break;
	}

	return S_OK;
}

CUFO * CUFO::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUFO*	pInstance = new CUFO(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUFO");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUFO::Clone(void * pArg)
{
	CUFO*	pInstance = new CUFO(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUFO");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUFO::Free()
{
	__super::Free();

	Safe_Release(m_pBuffCollider);
	Safe_Release(m_pBodyCollider);
	Safe_Release(m_pWeakCollider);
	Safe_Release(m_pModelCom);
}



