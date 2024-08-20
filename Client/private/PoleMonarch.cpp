#include "stdafx.h"
#include "..\public\PoleMonarch.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"
#include "Player_Hand.h"
#include "BossHp.h"
#include "SwordPower.h"
#include "BossGun.h"
#include "BackGround.h"
#include "RevolutionRock.h"
#include "MonsterSwordTrail.h"
#include "EndPhase.h"
#include "Camera_Dynamic.h"

CPoleMonarch::CPoleMonarch(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{

}

CPoleMonarch::CPoleMonarch(const CPoleMonarch & rhs)
	: CMonster(rhs)
{
}

HRESULT CPoleMonarch::Set_Navigation(const _tchar * pPrototypeTag)
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pNavigationCom);
	m_Components.erase(TEXT("Com_Navigation"));
	/* For.Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, pPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::Set_Navigation(Navigation)");
		return E_FAIL;
	}
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return S_OK;
}

HRESULT CPoleMonarch::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CPoleMonarch::NativeContruct_Prototype");
		return E_FAIL;
	}

	m_tStatus.fMaxHp = 110000.f;
	m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
	m_tStatus.fMaxShield = 10000.f;
	m_tStatus.fCurrentShield = 0.f;
	m_tStatus.fAttack = 50.f;
	m_tStatus.fDefend = 60.f;

	return S_OK;
}

HRESULT CPoleMonarch::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 6.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CPoleMonarch::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CPoleMonarch::NativeConstruct");
		return E_FAIL;
	}
	m_pModelCom->SetUp_AnimIndex(m_eState);
	m_pModelCom->SetUp_AnimIndex(m_eState);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vector*)pArg);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, -1.f, 0.f));
	m_pTransformCom->ArrangeMatrix();

	m_bIsBoss = true;

	// 분노바 배경
	INFO_UI tInfoUI_RageBackGround;
	ZeroMemory(&tInfoUI_RageBackGround, sizeof(INFO_UI));

	tInfoUI_RageBackGround.fScaleX = 430.f;
	tInfoUI_RageBackGround.fScaleY = 240.f;
	tInfoUI_RageBackGround.fPositionX = 652.f;
	tInfoUI_RageBackGround.fPositionY = 130.f;
	tInfoUI_RageBackGround.fDepth = 4.f;
	tInfoUI_RageBackGround.iTextureIndex = 77;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_BossHpBack"), TEXT("Prototype_GameObject_UI"), &tInfoUI_RageBackGround)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::NativeConstruct(tInfoUI_RageBackGround)");
		return E_FAIL;
	}

	INFO_UI tInfoUI_Rage;
	ZeroMemory(&tInfoUI_Rage, sizeof(INFO_UI));

	tInfoUI_Rage.fScaleX = 410.f;
	tInfoUI_Rage.fScaleY = 246.f;
	tInfoUI_Rage.fPositionX = 650.f;
	tInfoUI_Rage.fPositionY = 131.f;
	tInfoUI_Rage.fDepth = 3.f;
	tInfoUI_Rage.iTextureIndex = 78;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_BossHp"), TEXT("Prototype_GameObject_BossHp"), &tInfoUI_Rage)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::NativeConstruct(tInfoUI_Rage)");
		return E_FAIL;
	}

	// 체력바 배경
	INFO_UI tInfoUI_HpBackGround;
	ZeroMemory(&tInfoUI_HpBackGround, sizeof(INFO_UI));

	tInfoUI_HpBackGround.fScaleX = 700.f;
	tInfoUI_HpBackGround.fScaleY = 600.f;
	tInfoUI_HpBackGround.fPositionX = 640.f;
	tInfoUI_HpBackGround.fPositionY = 80.f;
	tInfoUI_HpBackGround.fDepth = 4.f;
	tInfoUI_HpBackGround.iTextureIndex = 75;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_BossHpBack"), TEXT("Prototype_GameObject_UI"), &tInfoUI_HpBackGround)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::NativeConstruct(HpBack)");
		return E_FAIL;
	}

	INFO_UI tInfoUI_Hp;
	ZeroMemory(&tInfoUI_Hp, sizeof(INFO_UI));

	tInfoUI_Hp.fScaleX = 410.f;
	tInfoUI_Hp.fScaleY = 370.f;
	tInfoUI_Hp.fPositionX = 650.f;
	tInfoUI_Hp.fPositionY = 90.f;
	tInfoUI_Hp.fDepth = 3.f;
	tInfoUI_Hp.iTextureIndex = 74;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_BossHp"), TEXT("Prototype_GameObject_BossHp"), &tInfoUI_Hp)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::NativeConstruct(Hp)");
		return E_FAIL;
	}

	INFO_UI tInfoUI_Phase2BackGround;
	ZeroMemory(&tInfoUI_Phase2BackGround, sizeof(INFO_UI));

	tInfoUI_Phase2BackGround.fScaleX = 34.f;
	tInfoUI_Phase2BackGround.fScaleY = 34.f;
	tInfoUI_Phase2BackGround.fPositionX = 547.f;
	tInfoUI_Phase2BackGround.fPositionY = 130.f;
	tInfoUI_Phase2BackGround.fDepth = 2.f;
	tInfoUI_Phase2BackGround.iTextureIndex = 79;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_BossPhase"), TEXT("Prototype_GameObject_UI"), &tInfoUI_Phase2BackGround)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::NativeConstruct(tInfoUI_Phase2BackGround)");
		return E_FAIL;
	}
	static_cast<CBackGround*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossPhase")))->Set_Pass(34);
	INFO_UI tInfoUI_Phase3BackGround;
	ZeroMemory(&tInfoUI_Phase3BackGround, sizeof(INFO_UI));

	tInfoUI_Phase3BackGround.fScaleX = 34.f;
	tInfoUI_Phase3BackGround.fScaleY = 34.f;
	tInfoUI_Phase3BackGround.fPositionX = 649.f;
	tInfoUI_Phase3BackGround.fPositionY = 130.f;
	tInfoUI_Phase3BackGround.fDepth = 2.f;
	tInfoUI_Phase3BackGround.iTextureIndex = 80;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_BossPhase"), TEXT("Prototype_GameObject_UI"), &tInfoUI_Phase3BackGround)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::NativeConstruct(tInfoUI_Phase3BackGround)");
		return E_FAIL;
	}
	static_cast<CBackGround*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossPhase")))->Set_Pass(34);
	INFO_UI tInfoUI = {};
	tInfoUI.fScaleX = 24.f;
	tInfoUI.fScaleY = 24.f;
	tInfoUI.fDepth = 3.f;
	tInfoUI.iTextureIndex = CMiniMap_Icon::TARGET_BOSS2;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon"), TEXT("Prototype_GameObject_MiniMap_Icon"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::NativeConstruct");
		return E_FAIL;
	}
	m_pMiniMap_Icon = static_cast<CMiniMap_Icon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon")));
	m_pMiniMap_Icon->Set_Target(m_pTransformCom);

	m_pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));

	return S_OK;
}

_int CPoleMonarch::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CPoleMonarch::Tick");
		return DEAD;
	}
	if (false == m_bFreeze)
		if (FAILED(State_Manage(dTimeDelta)))
		{
			MSGBOX("State_Manage returned E_FAIL in CPoleMonarch::Tick");
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
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(4.f, 4.f, 4.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-30.f)) * XMMatrixTranslation(0.f, 0.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_LINE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(8.f, 8.f, 8.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(0.53f, 0.f, 0.53f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(8.f, 8.f, 8.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(-0.66f, 0.f, -0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CCatfishWarrior::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(8.f, 8.f, 8.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(0.53f, 0.f, -0.53f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(8.f, 8.f, 8.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(-0.4f, 0.f, 0.66f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Ready_Layer_Effect");
			return E_FAIL;
		}
		m_bChain = true;
	}

	// 플레이어 포지션 갱신
	m_pPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_fTargetDistance = XMVectorGetX(XMVector4Length(m_pPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (g_pGameInstance->Get_KeyEnter(DIK_K))
		m_tStatus.fCurrentHp = 1.f;
	if (g_pGameInstance->Get_KeyEnter(DIK_N))
		m_tStatus.fCurrentHp *= 0.5f;
	if (g_pGameInstance->Get_KeyEnter(DIK_H))
		m_iForce += 10;

	for (auto& pWeakCollider : m_WeakCollider)
	{
		g_pGameInstance->Add_Collider(TEXT("Weakness"), CCollision_Manager::TARGET_ENEMY, pWeakCollider);
	}
	for (auto& pBodyCollider : m_BodyCollider)
	{
		g_pGameInstance->Add_Collider(TEXT("Body"), CCollision_Manager::TARGET_ENEMY, pBodyCollider);
	}


	if (false == m_bFire && m_tStatus.fCurrentHp < m_tStatus.fMaxHp * 0.5f)
	{
		m_bFire = true;
		m_bFirePattern = true;
	}
	else if (false == m_bEndPhase && m_tStatus.fCurrentHp <= m_tStatus.fMaxHp * 0.05f)
	{
		if (TRIPLEATTACK_1 == m_eState || TRIPLEATTACK_2 == m_eState || BLINKATTACK1_1 == m_eState || BLINKATTACK2_1 == m_eState)
		{
			CCollider::COLLIDERDESC tColliderDesc = {};

			if (BLINKATTACK1_1 == m_eState)
				tColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
			else if (TRIPLEATTACK_1 == m_eState || TRIPLEATTACK_2 == m_eState || BLINKATTACK2_1 == m_eState)
				tColliderDesc.vScale = _float3(0.25f, 0.25f, 0.25f);

			for (auto& pAttackCollider : m_AttackCollider)
				pAttackCollider->Set_ColliderDesc(tColliderDesc);
		}

		m_bEndPhase = true;
		m_eState = RUSH1;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pTransformCom->Set_Speed(15.f);
	}

	// 마지막 페이즈 돌 날라오기
	if (true == m_bPhase4)
	{
		m_fFireTime += (_float)dTimeDelta;
		if (0.5f < m_fFireTime)
		{
			m_fFireTime = 0.f;
			CRevolutionRock* pRock = static_cast<CRevolutionRock*>(m_RockList.front());
			pRock->Set_Throw(true);
			m_RockList.pop_front();
			m_RockList.push_back(pRock);
		}
	}
	m_fMoveUV += (_float)dTimeDelta;

	return NOPROBLEM;
}

_int CPoleMonarch::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CPoleMonarch::LateTick");
		return DEAD;
	}
	if (false == m_bFreeze)
		m_pModelCom->Update_CombinedTransformationMatrices(m_iAnimationType, dTimeDelta);

	if (FAILED(Collider_Update()))
	{
		MSGBOX("Collider_Update returned E_FAIL in CPoleMonarch::LateTick");
		return DEAD;
	}

	// 네비게이션 태우기
	if (CCell::TYPE_ROAD == m_pNavigationCom->Get_Type())
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos = XMVectorSetY(vPos, m_pNavigationCom->Compute_Y(vPos));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (DIE != m_eState)
	{
		static_cast<CBossHp*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossHp")))->Set_Hp((_uint)m_tStatus.fMaxHp, (_uint)m_tStatus.fCurrentHp);
		static_cast<CBossHp*>(g_pGameInstance->Get_Front(g_eCurrentLevel, TEXT("Layer_BossHp")))->Set_Hp(80, m_iForce);
	}

	if (false == m_bPhase2)
	{
		if (20 <= m_iForce)
		{

			m_bPhase2 = true;
			static_cast<CBackGround*>(g_pGameInstance->Get_Front(g_eCurrentLevel, TEXT("Layer_BossPhase")))->Set_Pass(0);
		}
	}
	else if (false == m_bPhase3)
	{
		if (40 <= m_iForce)
		{
			static_cast<CBackGround*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossPhase")))->Set_Pass(0);

			m_bPhase3 = true;
		}
	}



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
	for (auto& pBodyCollider : m_BodyCollider)
		m_pRendererCom->Add_Debug(pBodyCollider);
	for (auto& pWeakCollider : m_WeakCollider)
		m_pRendererCom->Add_Debug(pWeakCollider);
	for (auto& pAttackCollider : m_AttackCollider)
		m_pRendererCom->Add_Debug(pAttackCollider);
#endif

	return NOPROBLEM;
}




HRESULT CPoleMonarch::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pRendererCom)
	{
		MSGBOX("nullptr == m_pShaderCom or nullptr == m_pRendererCom in CPoleMonarch::Render");
		return E_FAIL;
	}

	if (FAILED(__super::SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CPoleMonarch::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		if (1 == i || 5 == i || 6 == i/* || 13 == i || 14 == i*/)
			continue;
		if (9 <= i && 14 >= i)
			continue;
		if (true == m_bIce)
		{
			if (0 == i)
			{
				if (FAILED(m_pTextureCom[0]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
				{
					MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CPoleMonarch::Render");
					return E_FAIL;
				}
			}
			else if (2 == i)
			{
				if (FAILED(m_pTextureCom[1]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
				{
					MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CPoleMonarch::Render");
					return E_FAIL;
				}
			}
			else if (12 == i)
			{
				if (FAILED(m_pTextureCom[2]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
				{
					MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CPoleMonarch::Render");
					return E_FAIL;
				}
			}
			else if (8 == i)
			{
				if (FAILED(m_pTextureCom[3]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
				{
					MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CPoleMonarch::Render");
					return E_FAIL;
				}
			}
			else
				m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		}
		else if (8 == i)
		{
			if (FAILED(m_pTextureCom[3]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
			{
				MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CPoleMonarch::Render");
				return E_FAIL;
			}
		}
		else if (12 == i)
		{
			if (FAILED(m_pTextureCom[2]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
			{
				MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CPoleMonarch::Render");
				return E_FAIL;
			}
		}
		else
			m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (8 == i)
		{
			if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV", &m_fMoveUV, sizeof(_float))))
			{
				MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer::SetUp_ConstantTable(g_fMoveUV)");
				return E_FAIL;
			}
			m_pModelCom->Render(m_pShaderCom, 3, i, "g_BoneMatrices");
		}
		else if (false == m_bIce)
			m_pModelCom->Render(m_pShaderCom, 0, i, "g_BoneMatrices");
		else
			m_pModelCom->Render(m_pShaderCom, 6, i, "g_BoneMatrices");
	}
#ifdef _DEBUG
	for (auto& pBodyCollider : m_BodyCollider)
		pBodyCollider->Render();
	for (auto& pWeakCollider : m_WeakCollider)
		pWeakCollider->Render();
	for (auto& pAttackCollider : m_AttackCollider)
		pAttackCollider->Render();
#endif
	return S_OK;
}

void CPoleMonarch::Stand(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(STAND))
	{
		m_eState = RUN;
		m_iAnimationType = ANIM_ROOF;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pTransformCom->Set_Speed(5.f);
	}
}

void CPoleMonarch::Walk_Front(_double TimeDelta)
{
	m_pTransformCom->LookAtXZ(m_pPlayerPosition);
	m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);
	if (true == m_pModelCom->Get_EndAnimation(WALK_F))
	{
		if (20 > m_iForce)
		{
			m_eState = RUN;
			m_iAnimationType = ANIM_ROOF;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->Set_Speed(5.f);
		}
		else
		{
			if (m_iShotPattern % 4 != 3)
				m_eState = BLINK1;
			else
				m_eState = RUSH1;




			if (true == m_bFirePattern)
				m_eState = RUSH1;
			else
				++m_iShotPattern;

			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->Set_Speed(10.f);
		}
	}
}

void CPoleMonarch::Walk_Back(_double TimeDelta)
{
	m_pTransformCom->LookAtXZ(m_pPlayerPosition);
	m_pTransformCom->Go_Backward(TimeDelta, m_pNavigationCom);
	if (true == m_pModelCom->Get_EndAnimation(WALK_B))
	{
		if (20 > m_iForce)
		{
			m_eState = RUN;
			m_iAnimationType = ANIM_ROOF;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->Set_Speed(5.f);
		}
		else
		{
			if (m_iShotPattern % 4 != 3)
				m_eState = BLINK1;
			else
				m_eState = RUSH1;

			if (true == m_bFirePattern)
				m_eState = RUSH1;
			else
				++m_iShotPattern;

			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->Set_Speed(10.f);
		}
	}
}

void CPoleMonarch::Walk_Left(_double TimeDelta)
{
	m_pTransformCom->LookAtXZ(m_pPlayerPosition);
	m_pTransformCom->Go_Left(TimeDelta, m_pNavigationCom);
	if (true == m_pModelCom->Get_EndAnimation(WALK_L))
	{
		if (20 > m_iForce)
		{
			m_eState = RUN;
			m_iAnimationType = ANIM_ROOF;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->Set_Speed(5.f);
		}
		else
		{
			if (m_iShotPattern % 4 != 3)
				m_eState = BLINK1;
			else
				m_eState = RUSH1;

			if (true == m_bFirePattern)
				m_eState = RUSH1;
			else
				++m_iShotPattern;

			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->Set_Speed(10.f);
		}
	}
}

void CPoleMonarch::Walk_Right(_double TimeDelta)
{
	m_pTransformCom->LookAtXZ(m_pPlayerPosition);
	m_pTransformCom->Go_Right(TimeDelta, m_pNavigationCom);
	if (true == m_pModelCom->Get_EndAnimation(WALK_R))
	{
		if (20 > m_iForce)
		{
			m_eState = RUN;
			m_iAnimationType = ANIM_ROOF;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->Set_Speed(5.f);
		}
		else
		{
			if (m_iShotPattern % 4 != 3)
				m_eState = BLINK1;
			else
				m_eState = RUSH1;

			if (true == m_bFirePattern)
				m_eState = RUSH1;
			else
				++m_iShotPattern;

			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->Set_Speed(10.f);
		}
	}
}

void CPoleMonarch::Run(_double TimeDelta)
{
	m_pTransformCom->LookAtXZ(m_pPlayerPosition);
	m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);

	if (3.f > m_fTargetDistance)
	{
		m_eState = ATTACK2_1;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_fRunTime = 0.f;

		CCollider::COLLIDERDESC tColliderDesc = {};
		tColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
		for (auto& pAttackCollider : m_AttackCollider)
			pAttackCollider->Set_ColliderDesc(tColliderDesc);
	}
	else if (2.f < m_fRunTime)
	{
		if (6.f > m_fTargetDistance)
		{
			m_eState = ATTACK2_1;
			CCollider::COLLIDERDESC tColliderDesc = {};
			tColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
			for (auto& pAttackCollider : m_AttackCollider)
				pAttackCollider->Set_ColliderDesc(tColliderDesc);
		}
		else
			m_eState = ATTACK3_1;

		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_fRunTime = 0.f;
	}

	m_fRunTime += (_float)TimeDelta;
}

void CPoleMonarch::Die(_double TimeDelta)
{
	m_fEndingTime += (_float)TimeDelta;
	static _bool bEnding = false;

	if (false == bEnding && 20.f < m_fEndingTime)
	{
		INFO_UI	tInfoUI;
		tInfoUI.fDepth = -20.f;
		tInfoUI.fPositionX = 640.f;
		tInfoUI.fPositionY = 360.f;
		tInfoUI.fScaleX = 1280.f;
		tInfoUI.fScaleY = 720.f;
		tInfoUI.iTextureIndex = 100;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Ending"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
		{
			__debugbreak();
			return;
		}
		g_bCameraAction = true;
		g_pGameInstance->StopAll();
		bEnding = true;
	}

}

void CPoleMonarch::Attack1_1(_double TimeDelta)
{
	if (false == m_bCollision && 37.0 < m_pModelCom->Get_PlayAcc())
	{
		for (auto& pAttackCollider : m_AttackCollider)
		{
			if (true == g_pGameInstance->Collision_Sphere(pAttackCollider, m_pPlayerCollider))
			{
				static_cast<CPlayer_Hand*>(m_pPlayerCollider->Get_Parent())->Hit(80.f);
				m_bCollision = true;
				m_iForce += 5;
				if (80 < m_iForce)
					m_iForce = 80;
				break;
			}
		}
	}

	if (true == m_pModelCom->Get_EndAnimation(ATTACK1_1))
	{
		m_eState = ATTACK1_2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bCollision = false;

		CCollider::COLLIDERDESC tColliderDesc = {};
		tColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		for (auto& pAttackCollider : m_AttackCollider)
			pAttackCollider->Set_ColliderDesc(tColliderDesc);
	}
}

void CPoleMonarch::Attack1_2(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(ATTACK1_2))
	{
		if (4.f > m_fTargetDistance)
		{
			m_eState = ATTACK1_1;
			m_iAnimationType = ANIM_ONETIME;
			CCollider::COLLIDERDESC tColliderDesc = {};
			tColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
			for (auto& pAttackCollider : m_AttackCollider)
				pAttackCollider->Set_ColliderDesc(tColliderDesc);
		}
		else if (GetAsyncKeyState('W') & 0x8000)
		{
			m_eState = WALK_B;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			m_eState = WALK_F;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			m_eState = WALK_L;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			m_eState = WALK_R;
			m_pTransformCom->Set_Speed(2.f);
		}
		else
		{
			if (20 > m_iForce)
			{

				if (true == m_bFirePattern)
				{
					m_eState = RUSH1;
					m_iAnimationType = ANIM_ONETIME;
					m_pTransformCom->Set_Speed(10.f);
				}
				else
				{
					m_eState = RUN;
					m_iAnimationType = ANIM_ROOF;
					m_pTransformCom->Set_Speed(5.f);
				}
			}
			else if (20 <= m_iForce)
			{
				if (true == m_bFirePattern)
					m_eState = RUSH1;
				else
					++m_iShotPattern;

				m_iAnimationType = ANIM_ONETIME;
				m_pTransformCom->Set_Speed(10.f);
			}
		}
		m_pModelCom->SetUp_AnimIndex(m_eState);

	}
}

void CPoleMonarch::Attack2_1(_double TimeDelta)
{
	if (false == m_bSwingSound && 29.0 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT11);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2Swing.mp3"), CSoundMgr::EFFECT11, 1.f);
		m_bSwingSound = true;
	}
	if (false == m_bSwordTrail && 44.0 < m_pModelCom->Get_PlayAcc())
	{
		CMonsterSwordTrail::SWORDTRAIL tSwordTrail;
		tSwordTrail.RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		tSwordTrail.fFirstUV = -1.f;
		tSwordTrail.fTrailSpeed = 3.f;
		tSwordTrail.pModelTag = TEXT("Prototype_Component_Model_BossSwordTrail");
		tSwordTrail.pTextureTag = TEXT("Prototype_Component_Texture_BossSwordTrail");
		tSwordTrail.pParentTransform = m_pTransformCom;
		tSwordTrail.vTrailScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		tSwordTrail.vTranslation = _float3(0.f, 0.9f, 0.f);
		tSwordTrail.bUseNoise = true;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Attack2_1");
			return;
		}
		tSwordTrail.vTranslation = _float3(0.f, 0.6f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Attack2_1");
			return;
		}
		m_bSwordTrail = true;
	}
	if (58.0 < m_pModelCom->Get_PlayAcc() && m_bCollision == false)
	{
		for (auto& pAttackCollider : m_AttackCollider)
		{
			if (true == g_pGameInstance->Collision_Sphere(pAttackCollider, m_pPlayerCollider))
			{
				static_cast<CPlayer_Hand*>(m_pPlayerCollider->Get_Parent())->Hit(100.f);
				m_bCollision = true;
				m_iForce += 5;
				if (80 < m_iForce)
					m_iForce = 80;
				break;
			}
		}
	}
	if (true == m_pModelCom->Get_EndAnimation(ATTACK2_1))
	{
		m_eState = ATTACK2_2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bCollision = false;
		CCollider::COLLIDERDESC tColliderDesc = {};
		tColliderDesc.vScale = _float3(0.25f, 0.25f, 0.25f);
		for (auto& pAttackCollider : m_AttackCollider)
			pAttackCollider->Set_ColliderDesc(tColliderDesc);
		m_bSwordTrail = false;
		m_bSwingSound = false;
	}
}

void CPoleMonarch::Attack2_2(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(ATTACK2_2))
	{
		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_eState = WALK_B;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			m_eState = WALK_F;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			m_eState = WALK_L;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			m_eState = WALK_R;
			m_pTransformCom->Set_Speed(2.f);
		}
		else
		{
			if (20 > m_iForce)
			{
				if (true == m_bFirePattern)
				{
					m_eState = RUSH1;
					m_iAnimationType = ANIM_ONETIME;
					m_pTransformCom->Set_Speed(10.f);
				}
				else
				{
					m_eState = RUN;
					m_iAnimationType = ANIM_ROOF;
					m_pTransformCom->Set_Speed(5.f);
				}

			}
			else if (20 <= m_iForce)
			{
				m_eState = BLINK1;
				m_iAnimationType = ANIM_ONETIME;
				if (true == m_bFirePattern)
					m_eState = RUSH1;
				else
					++m_iShotPattern;

				m_pTransformCom->Set_Speed(10.f);
			}
		}
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}

void CPoleMonarch::Attack3_1(_double TimeDelta)
{
	if (false == m_bSwordTrail && 20.0 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT9);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2EnergyShot.mp3"), CSoundMgr::EFFECT9, 0.4f);
		g_pGameInstance->StopSound(CSoundMgr::EFFECT10);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2Energy.mp3"), CSoundMgr::EFFECT10, 0.4f);

		CMonsterSwordTrail::SWORDTRAIL tSwordTrail;
		tSwordTrail.RotationMatrix = XMMatrixIdentity();
		tSwordTrail.fFirstUV = -1.f;
		tSwordTrail.fTrailSpeed = 3.f;
		tSwordTrail.pModelTag = TEXT("Prototype_Component_Model_BossEnergySwordTrail");
		tSwordTrail.pTextureTag = TEXT("Prototype_Component_Texture_BossSwordTrail");
		tSwordTrail.pParentTransform = m_pTransformCom;
		tSwordTrail.vTrailScale = XMVectorSet(1.5f, 1.5f, 1.5f, 0.f);
		tSwordTrail.vTranslation = _float3(0.f, 0.f, 0.f);
		tSwordTrail.bUseNoise = true;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Attack3_1");
			return;
		}
		m_bSwordTrail = true;
	}
	if (false == m_bSwordPower && 31.0 < m_pModelCom->Get_PlayAcc())
	{
		m_bSwordPower = true;

		CSwordPower::BULLETDESC tBulletDesc = {};
		tBulletDesc.pModel = m_pModelCom;
		tBulletDesc.pTransform = m_pTransformCom;
		tBulletDesc.pSocketName = "3924_weapon01";
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_SwordPower"), TEXT("Prototype_GameObject_SwordPower"), &tBulletDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Attack3_1");
			return;
		}
	}
	if (16.0 > m_pModelCom->Get_PlayAcc())
	{
		m_pTransformCom->LookAtXZ(m_pPlayerPosition);
	}

	if (true == m_pModelCom->Get_EndAnimation(ATTACK3_1))
	{
		m_eState = ATTACK3_2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bSwordPower = false;
		m_bSwordTrail = false;
	}
}

void CPoleMonarch::Attack3_2(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(ATTACK3_2))
	{
		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_eState = WALK_B;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			m_eState = WALK_F;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			m_eState = WALK_L;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			m_eState = WALK_R;
			m_pTransformCom->Set_Speed(2.f);
		}
		else
		{
			if (20 > m_iForce)
			{
				if (true == m_bFirePattern)
				{
					m_eState = RUSH1;
					m_iAnimationType = ANIM_ONETIME;
					m_pTransformCom->Set_Speed(10.f);
				}
				else
				{
					m_eState = RUN;
					m_iAnimationType = ANIM_ROOF;
					m_pTransformCom->Set_Speed(5.f);
				}
			}
			else if (20 <= m_iForce)
			{
				m_eState = BLINK1;
				m_iAnimationType = ANIM_ONETIME;
				if (true == m_bFirePattern)
					m_eState = RUSH1;
				else
					++m_iShotPattern;

				m_pTransformCom->Set_Speed(10.f);
			}
		}
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}

void CPoleMonarch::TripleAttack_1(_double TimeDelta)
{
	if (false == m_bSwingSound && 19.0 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT11);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2Swing.mp3"), CSoundMgr::EFFECT11, 1.f);
		m_bSwingSound = true;
	}
	if (false == m_bSwordTrail && 34.0 < m_pModelCom->Get_PlayAcc())
	{
		CMonsterSwordTrail::SWORDTRAIL tSwordTrail;
		tSwordTrail.RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f));
		tSwordTrail.fFirstUV = -1.f;
		tSwordTrail.fTrailSpeed = 3.f;
		tSwordTrail.pModelTag = TEXT("Prototype_Component_Model_BossSwordTrail");
		tSwordTrail.pTextureTag = TEXT("Prototype_Component_Texture_BossSwordTrail");
		tSwordTrail.pParentTransform = m_pTransformCom;
		tSwordTrail.vTrailScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		tSwordTrail.vTranslation = _float3(0.f, 0.9f, 0.f);
		tSwordTrail.bUseNoise = true;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::TripleAttack_1");
			return;
		}
		tSwordTrail.vTranslation = _float3(0.f, 0.6f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::TripleAttack_1");
			return;
		}
		m_bSwordTrail = true;
	}
	if (false == m_bCollision && 37.0 < m_pModelCom->Get_PlayAcc())
	{
		for (auto& pAttackCollider : m_AttackCollider)
		{
			if (true == g_pGameInstance->Collision_Sphere(pAttackCollider, m_pPlayerCollider))
			{
				static_cast<CPlayer_Hand*>(m_pPlayerCollider->Get_Parent())->Hit(120.f);
				m_bCollision = true;
				m_iForce += 5;
				if (80 < m_iForce)
					m_iForce = 80;
				break;
			}
		}
	}
	if (true == m_pModelCom->Get_EndAnimation(TRIPLEATTACK_1))
	{
		m_eState = TRIPLEATTACK_2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bCollision = false;
		//CCollider::COLLIDERDESC tColliderDesc = {};
		//tColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
		//for (auto& pAttackCollider : m_AttackCollider)
		//	pAttackCollider->Set_ColliderDesc(tColliderDesc);

		m_pTransformCom->Set_Speed(20.f);
		m_bSwordTrail = false;
		m_bSwingSound = false;
	}
}

void CPoleMonarch::TripleAttack_2(_double TimeDelta)
{
	if (false == m_bSwingSound && 7.0 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT11);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2Swing.mp3"), CSoundMgr::EFFECT11, 1.f);
		m_bSwingSound = true;
	}
	if (false == m_bSwordTrail && 20.0 < m_pModelCom->Get_PlayAcc())
	{
		CMonsterSwordTrail::SWORDTRAIL tSwordTrail;
		tSwordTrail.RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		tSwordTrail.fFirstUV = -1.f;
		tSwordTrail.fTrailSpeed = 3.f;
		tSwordTrail.pModelTag = TEXT("Prototype_Component_Model_BossSwordTrail");
		tSwordTrail.pTextureTag = TEXT("Prototype_Component_Texture_BossSwordTrail");
		tSwordTrail.pParentTransform = m_pTransformCom;
		tSwordTrail.vTrailScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		tSwordTrail.bUseNoise = true;
		_float fTranslationY = 0.6f;
		for (_uint i = 0; i < 5; i++)
		{
			tSwordTrail.vTranslation = _float3(0.f, fTranslationY, 0.f);
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::BlinkAttack2_1");
				return;
			}
			fTranslationY += 0.3f;
		}
		m_bSwordTrail = true;
	}
	if (28.0 > m_pModelCom->Get_PlayAcc())
	{
		m_pTransformCom->LookTurn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pPlayerPosition, TimeDelta * 5.f);
		if (3.f < m_fTargetDistance)
			m_pTransformCom->Go_Vector(m_pPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION), TimeDelta * 1.3f, m_pNavigationCom);

	}
	else if (44.0 < m_pModelCom->Get_PlayAcc() && false == m_bCollision)
	{
		for (auto& pAttackCollider : m_AttackCollider)
		{
			if (true == g_pGameInstance->Collision_Sphere(pAttackCollider, m_pPlayerCollider))
			{
				static_cast<CPlayer_Hand*>(m_pPlayerCollider->Get_Parent())->Hit(140.f);
				m_bCollision = true;
				m_iForce += 5;
				if (80 < m_iForce)
					m_iForce = 80;
				break;
			}
		}
	}
	if (true == m_pModelCom->Get_EndAnimation(TRIPLEATTACK_2))
	{
		m_eState = TRIPLEATTACK_3;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bCollision = false;
		CCollider::COLLIDERDESC tColliderDesc = {};
		tColliderDesc.vScale = _float3(0.25f, 0.25f, 0.25f);
		for (auto& pAttackCollider : m_AttackCollider)
			pAttackCollider->Set_ColliderDesc(tColliderDesc);
		m_vTripleAttackTargetPoint = m_pPlayerPosition;
		m_bSwordTrail = false;
		m_bSwingSound = false;
	}
}

void CPoleMonarch::TripleAttack_3(_double TimeDelta)
{
	if (22.0 > m_pModelCom->Get_PlayAcc())
		m_pTransformCom->LookTurn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pPlayerPosition, TimeDelta * 8.f);

	if (false == m_bSwordTrail && 20.0 < m_pModelCom->Get_PlayAcc())
	{
		CMonsterSwordTrail::SWORDTRAIL tSwordTrail;
		tSwordTrail.RotationMatrix = XMMatrixIdentity();
		tSwordTrail.fFirstUV = -1.f;
		tSwordTrail.fTrailSpeed = 3.f;
		tSwordTrail.pModelTag = TEXT("Prototype_Component_Model_BossEnergySwordTrail");
		tSwordTrail.pTextureTag = TEXT("Prototype_Component_Texture_BossSwordTrail");
		tSwordTrail.pParentTransform = m_pTransformCom;
		tSwordTrail.vTrailScale = XMVectorSet(1.5f, 1.5f, 1.5f, 0.f);
		tSwordTrail.vTranslation = _float3(0.f, 0.f, 0.f);
		tSwordTrail.bUseNoise = true;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::TripleAttack_3");
			return;
		}
		g_pGameInstance->StopSound(CSoundMgr::EFFECT9);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2EnergyShot.mp3"), CSoundMgr::EFFECT9, 0.4f);
		g_pGameInstance->StopSound(CSoundMgr::EFFECT10);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2Energy.mp3"), CSoundMgr::EFFECT10, 0.4f);

		m_bSwordTrail = true;
	}

	if (false == m_bSwordPower && 38.0 < m_pModelCom->Get_PlayAcc())
	{
		m_bSwordPower = true;

		CSwordPower::BULLETDESC tBulletDesc = {};
		tBulletDesc.pModel = m_pModelCom;
		tBulletDesc.pTransform = m_pTransformCom;
		tBulletDesc.pSocketName = "3924_weapon01";
		tBulletDesc.fDist = 0.f;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_SwordPower"), TEXT("Prototype_GameObject_SwordPower"), &tBulletDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::TripleAttack_3");
			return;
		}

		if (true == m_bPhase4)
		{
			tBulletDesc.fDist = -20.f;
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_SwordPower"), TEXT("Prototype_GameObject_SwordPower"), &tBulletDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::TripleAttack_3");
				return;
			}

			tBulletDesc.fDist = 20.f;
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_SwordPower"), TEXT("Prototype_GameObject_SwordPower"), &tBulletDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::TripleAttack_3");
				return;
			}
		}
	}

	if (true == m_pModelCom->Get_EndAnimation(TRIPLEATTACK_3))
	{
		m_eState = TRIPLEATTACK_4;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bSwordPower = false;
		m_bSwordTrail = false;
	}
}

void CPoleMonarch::TripleAttack_4(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(TRIPLEATTACK_4))
	{
		m_eState = BLINK1;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pTransformCom->Set_Speed(15.f);
	}
}

void CPoleMonarch::Shot1(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(SHOT1))
	{
		m_eState = SHOT2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}

void CPoleMonarch::Shot2(_double TimeDelta)
{
	if (0.1f < m_fFireTime && 5 > m_iGunSize)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(_float(rand() % 100) / 25.f - 2.f, 4.f + _float(rand() % 100) / 50.f - 1.f, _float(rand() % 100) / 25.f - 2.f, 0.f);
		if (FAILED(g_pGameInstance->ThrowToLayer(g_eCurrentLevel, TEXT("Layer_BossGun"))))
		{
			MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CPoleMonarch::Shot1");
			return;
		}
		CTransform* pBossGunTransform = static_cast<CTransform*>(static_cast<CBossGun*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossGun")))->Get_Component(TEXT("Com_Transform")));
		pBossGunTransform->Set_State(CTransform::STATE_POSITION, vPos);
		pBossGunTransform->LookAt(m_pPlayerPosition + XMVectorSet(_float(rand() % 101 - 50) / 15.f, 0.f, _float(rand() % 101 - 50) / 15.f, 0.f));

		m_iGunSize++;
		m_fFireTime = 0.f;
	}
	m_fFireTime += (_float)TimeDelta;

	if (true == m_pModelCom->Get_EndAnimation(SHOT2))
	{
		m_eState = SHOT3;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_fFireTime = 0.f;
		m_iGunSize = 0;
	}
}

void CPoleMonarch::Shot3(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(SHOT3))
	{

		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_eState = WALK_B;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			m_eState = WALK_F;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			m_eState = WALK_L;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			m_eState = WALK_R;
			m_pTransformCom->Set_Speed(2.f);
		}
		else
		{
			if (20 > m_iForce)
			{
				m_eState = RUN;
				m_iAnimationType = ANIM_ROOF;
				m_pTransformCom->Set_Speed(5.f);
			}
			else if (20 <= m_iForce)
			{
				m_eState = BLINK1;
				m_iAnimationType = ANIM_ONETIME;
				m_pTransformCom->Set_Speed(10.f);
			}
		}
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}

void CPoleMonarch::Rush1(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(RUSH1))
	{
		m_eState = RUSH2;
		m_iAnimationType = ANIM_ROOF;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pTransformCom->LookAtXZ(XMVectorSet(54.f, 41.3f, 37.f, 1.f));
		m_bIce = true;
	}
}

void CPoleMonarch::Rush2(_double TimeDelta)
{
	_float fCenterLength = XMVectorGetX(XMVector4Length(XMVectorSet(54.f, 41.3f, 37.f, 1.f) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (0.2f > fCenterLength)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(54.f, 41.3f, 37.f, 1.f));
		if (true == m_bFirePattern)
		{
			m_eState = FIRE1;
			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_bFirePattern = false;

			m_bStrong = true;
			static_cast<CBossHp*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossHp")))->Set_Pass(1, 76);
		}
		else if (true == m_bEndPhase)
		{
			m_eState = END1;
			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->LookAtXZ(m_pPlayerPosition);

			g_pGameInstance->StopSound(CSoundMgr::BGM);
			g_pGameInstance->LetsPlaySound(TEXT("Boss2FinalPatternStart.mp3"), CSoundMgr::BGM, 1.f);

			m_bStrong = true;
			static_cast<CBossHp*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossHp")))->Set_Pass(1, 76);
		}
		else
		{
			m_eState = SHOT1;
			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pTransformCom->LookAtXZ(m_pPlayerPosition);
		}
		m_pTransformCom->LookAtXZ(m_pPlayerPosition);

		m_bIce = false;
	}
	else
		m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);
}

void CPoleMonarch::Fire1(_double TimeDelta)
{
	if (false == m_bSwordPower && 57 < m_pModelCom->Get_PlayAcc())
	{
		_float4	vPosition;
		XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BossWind"), &vPosition)))
		{
			__debugbreak();
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Fire1");
			return;
		}
		m_bSwordPower = true;
	}
	if (true == m_pModelCom->Get_EndAnimation(FIRE1))
	{
		g_pGameInstance->StopSound(CSoundMgr::KILL_MONSTER);
		g_pGameInstance->PlayRoofSound(TEXT("Boss2Fire.mp3"), CSoundMgr::KILL_MONSTER, 1.f);

		m_eState = FIRE2;
		m_iAnimationType = ANIM_ROOF;
		m_pModelCom->SetUp_AnimIndex(m_eState);

		if (FAILED(g_pGameInstance->ThrowToLayer(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))))
		{
			MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CPoleMonarch::Fire1");
			return;
		}
		static_cast<CTransform*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + 3.f * m_pTransformCom->Get_State(CTransform::STATE_LOOK) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		if (FAILED(g_pGameInstance->ThrowToLayer(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))))
		{
			MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CPoleMonarch::Fire1");
			return;
		}
		static_cast<CTransform*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - 3.f * m_pTransformCom->Get_State(CTransform::STATE_LOOK) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		if (FAILED(g_pGameInstance->ThrowToLayer(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))))
		{
			MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CPoleMonarch::Fire1");
			return;
		}
		static_cast<CTransform*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + 3.f * m_pTransformCom->Get_State(CTransform::STATE_RIGHT) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		if (FAILED(g_pGameInstance->ThrowToLayer(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))))
		{
			MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CPoleMonarch::Fire1");
			return;
		}
		static_cast<CTransform*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - 3.f * m_pTransformCom->Get_State(CTransform::STATE_RIGHT) + +XMVectorSet(0.f, 1.f, 0.f, 0.f));

		if (FAILED(g_pGameInstance->ThrowToLayer(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))))
		{
			MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CPoleMonarch::Fire1");
			return;
		}
		static_cast<CTransform*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + 3.f * XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		if (FAILED(g_pGameInstance->ThrowToLayer(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))))
		{
			MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CPoleMonarch::Fire1");
			return;
		}
		static_cast<CTransform*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + 3.f * XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		if (FAILED(g_pGameInstance->ThrowToLayer(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))))
		{
			MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CPoleMonarch::Fire1");
			return;
		}
		static_cast<CTransform*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + 3.f * XMVector4Normalize(-m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		if (FAILED(g_pGameInstance->ThrowToLayer(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))))
		{
			MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CPoleMonarch::Fire1");
			return;
		}
		static_cast<CTransform*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"))->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + 3.f * XMVector4Normalize(-m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) + XMVectorSet(0.f, 1.f, 0.f, 0.f));



		m_bSwordPower = false;
	}
}

void CPoleMonarch::Fire2(_double TimeDelta)
{
	m_fFireTime += (_float)TimeDelta;

	if (10.f < m_fFireTime)
	{
		m_eState = FIRE3;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_fFireTime = 0.f;
	}
}

void CPoleMonarch::Fire3(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(FIRE3))
	{
		g_pGameInstance->StopSound(CSoundMgr::KILL_MONSTER);
		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_eState = WALK_B;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			m_eState = WALK_F;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			m_eState = WALK_L;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			m_eState = WALK_R;
			m_pTransformCom->Set_Speed(2.f);
		}
		else
		{
			if (20 > m_iForce)
			{
				m_eState = RUN;
				m_iAnimationType = ANIM_ROOF;
				m_pTransformCom->Set_Speed(5.f);
			}
			else if (20 <= m_iForce)
			{
				m_eState = BLINK1;
				m_iAnimationType = ANIM_ONETIME;
				m_pTransformCom->Set_Speed(10.f);
			}
		}
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_fFireTime = 0.f;

		auto BossCannonList = g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_BossCannon"));
		for (auto& pBossCannon : *BossCannonList)
			pBossCannon->Set_Dead(true);

		m_bStrong = false;
		static_cast<CBossHp*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossHp")))->Set_Pass(4, 74);
	}
}

void CPoleMonarch::Blink1(_double TimeDelta)
{
	m_pTransformCom->LookAtXZ(m_pPlayerPosition);
	if (false == m_bEndPhase)
		m_pTransformCom->Set_Speed(20.f);
	else
		m_pTransformCom->Set_Speed(20.f);
	if (true == m_pModelCom->Get_EndAnimation(BLINK1))
	{
		m_eState = BLINK2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);

		m_bIce = true;
	}
}

void CPoleMonarch::Blink2(_double TimeDelta)
{
	m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);
	m_fFireTime += (_float)TimeDelta;
	if (0.05f < m_fFireTime)
	{
		_float4 vSmokePosition = _float4(m_AttackCollider[1]->Get_Sphere()->Center, 1.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_BossBlinkSmoke"), TEXT("Prototype_GameObject_BossBlinkSmoke"), &vSmokePosition)))
		{
			__debugbreak();
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Blink2");
			return;
		}
		m_fFireTime = 0.f;
	}

	if (40 <= m_iForce && 4.f > m_fTargetDistance)
	{
		m_eState = TRIPLEATTACK_1;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pTransformCom->LookAtXZ(m_pPlayerPosition);
		m_iAnimationType = ANIM_ONETIME;

		CCollider::COLLIDERDESC tColliderDesc = {};
		tColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
		for (auto& pAttackCollider : m_AttackCollider)
			pAttackCollider->Set_ColliderDesc(tColliderDesc);

		m_bIce = false;
		m_fFireTime = 0.f;
	}
	else if (40 > m_iForce && 4.f > m_fTargetDistance)
	{
		m_eState = BLINKATTACK2_1;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pTransformCom->LookAtXZ(m_pPlayerPosition);

		CCollider::COLLIDERDESC tColliderDesc = {};
		tColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
		for (auto& pAttackCollider : m_AttackCollider)
			pAttackCollider->Set_ColliderDesc(tColliderDesc);

		m_bIce = false;
		m_fFireTime = 0.f;
	}

	if (true == m_pModelCom->Get_EndAnimation(BLINK2))
	{
		if (40 > m_iForce)
		{

			if (6.f > m_fTargetDistance)
			{
				m_eState = BLINKATTACK2_1;
				CCollider::COLLIDERDESC tColliderDesc = {};
				tColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
				for (auto& pAttackCollider : m_AttackCollider)
					pAttackCollider->Set_ColliderDesc(tColliderDesc);
			}
			else
				m_eState = BLINKATTACK3_1;

		}
		else
		{
			m_eState = TRIPLEATTACK_1;
			CCollider::COLLIDERDESC tColliderDesc = {};
			tColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
			for (auto& pAttackCollider : m_AttackCollider)
				pAttackCollider->Set_ColliderDesc(tColliderDesc);
		}

		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pTransformCom->LookAtXZ(m_pPlayerPosition);
		m_bIce = false;
		m_fFireTime = 0.f;
	}
}

void CPoleMonarch::BlinkAttack1_1(_double TimeDelta)
{
	if (false == m_bCollision && 33.0 < m_pModelCom->Get_PlayAcc())
	{
		for (auto& pAttackCollider : m_AttackCollider)
		{
			if (true == g_pGameInstance->Collision_Sphere(pAttackCollider, m_pPlayerCollider))
			{
				static_cast<CPlayer_Hand*>(m_pPlayerCollider->Get_Parent())->Hit(100.f);
				m_bCollision = true;
				m_iForce += 5;
				if (80 < m_iForce)
					m_iForce = 80;
				break;
			}
		}
	}
	if (true == m_pModelCom->Get_EndAnimation(BLINKATTACK1_1))
	{
		m_eState = BLINKATTACK1_2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bCollision = false;

		CCollider::COLLIDERDESC tColliderDesc = {};
		tColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		for (auto& pAttackCollider : m_AttackCollider)
			pAttackCollider->Set_ColliderDesc(tColliderDesc);
	}
}

void CPoleMonarch::BlinkAttack1_2(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(BLINKATTACK1_2))
	{
		if (4.f > m_fTargetDistance)
		{
			m_eState = BLINKATTACK1_1;
			m_iAnimationType = ANIM_ONETIME;
			CCollider::COLLIDERDESC tColliderDesc = {};
			tColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
			for (auto& pAttackCollider : m_AttackCollider)
				pAttackCollider->Set_ColliderDesc(tColliderDesc);
		}
		else if (GetAsyncKeyState('W') & 0x8000)
		{
			m_eState = WALK_B;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			m_eState = WALK_F;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			m_eState = WALK_L;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			m_eState = WALK_R;
			m_pTransformCom->Set_Speed(2.f);
		}
		else
		{
			if (true == m_bFirePattern)
				m_eState = RUSH1;
			else
			{
				++m_iShotPattern;
				m_eState = BLINK1;
			}
			m_iAnimationType = ANIM_ONETIME;
			m_pTransformCom->Set_Speed(10.f);
		}
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}

void CPoleMonarch::BlinkAttack2_1(_double TimeDelta)
{
	if (false == m_bSwingSound && 34.0 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT11);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2Swing.mp3"), CSoundMgr::EFFECT11, 1.f);
		m_bSwingSound = true;
	}
	if (false == m_bSwordTrail && 53.0 < m_pModelCom->Get_PlayAcc())
	{
		CMonsterSwordTrail::SWORDTRAIL tSwordTrail;
		tSwordTrail.RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		tSwordTrail.fFirstUV = -1.f;
		tSwordTrail.fTrailSpeed = 3.f;
		tSwordTrail.pModelTag = TEXT("Prototype_Component_Model_BossSwordTrail");
		tSwordTrail.pTextureTag = TEXT("Prototype_Component_Texture_BossSwordTrail");
		tSwordTrail.pParentTransform = m_pTransformCom;
		tSwordTrail.vTrailScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		tSwordTrail.bUseNoise = true;
		_float fTranslationY = 0.6f;
		for (_uint i = 0; i < 5; i++)
		{
			tSwordTrail.vTranslation = _float3(0.f, fTranslationY, 0.f);
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::BlinkAttack2_1");
				return;
			}
			fTranslationY += 0.3f;
		}
		m_bSwordTrail = true;
	}
	if (58.0 < m_pModelCom->Get_PlayAcc() && m_bCollision == false)
	{
		for (auto& pAttackCollider : m_AttackCollider)
		{
			if (true == g_pGameInstance->Collision_Sphere(pAttackCollider, m_pPlayerCollider))
			{
				static_cast<CPlayer_Hand*>(m_pPlayerCollider->Get_Parent())->Hit(120.f);
				m_bCollision = true;
				m_iForce += 5;
				if (80 < m_iForce)
					m_iForce = 80;
				break;
			}
		}	
		m_bSwordTrail = false;
	}

	if (50.0 > m_pModelCom->Get_PlayAcc())
		m_pTransformCom->LookTurn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pPlayerPosition, TimeDelta * 5.f);

	if (true == m_pModelCom->Get_EndAnimation(BLINKATTACK2_1))
	{
		m_eState = BLINKATTACK2_2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bCollision = false;
		CCollider::COLLIDERDESC tColliderDesc = {};
		tColliderDesc.vScale = _float3(0.25f, 0.25f, 0.25f);
		for (auto& pAttackCollider : m_AttackCollider)
			pAttackCollider->Set_ColliderDesc(tColliderDesc);
		m_bSwingSound = false;
	}
}

void CPoleMonarch::BlinkAttack2_2(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(BLINKATTACK2_2))
	{
		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_eState = WALK_B;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			m_eState = WALK_F;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			m_eState = WALK_L;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			m_eState = WALK_R;
			m_pTransformCom->Set_Speed(2.f);
		}
		else
		{
			if (true == m_bFirePattern)
				m_eState = RUSH1;
			else
			{
				++m_iShotPattern;
				m_eState = BLINK1;
			}
			m_iAnimationType = ANIM_ONETIME;
			m_pTransformCom->Set_Speed(10.f);
		}
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}

void CPoleMonarch::BlinkAttack3_1(_double TimeDelta)
{
	if (false == m_bSwordTrail && 35.0 < m_pModelCom->Get_PlayAcc())
	{
		CMonsterSwordTrail::SWORDTRAIL tSwordTrail;
		tSwordTrail.RotationMatrix = XMMatrixIdentity();
		tSwordTrail.fFirstUV = -1.f;
		tSwordTrail.fTrailSpeed = 3.f;
		tSwordTrail.pModelTag = TEXT("Prototype_Component_Model_BossEnergySwordTrail");
		tSwordTrail.pTextureTag = TEXT("Prototype_Component_Texture_BossSwordTrail");
		tSwordTrail.pParentTransform = m_pTransformCom;
		tSwordTrail.vTrailScale = XMVectorSet(1.5f, 1.5f, 1.5f, 0.f);
		tSwordTrail.vTranslation = _float3(0.f, 0.f, 0.f);
		tSwordTrail.bUseNoise = true;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MonsterSwordTrail"), &tSwordTrail)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Attack3_1");
			return;
		}
		g_pGameInstance->StopSound(CSoundMgr::EFFECT9);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2EnergyShot.mp3"), CSoundMgr::EFFECT9, 0.4f);
		g_pGameInstance->StopSound(CSoundMgr::EFFECT10);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2Energy.mp3"), CSoundMgr::EFFECT10, 0.4f);

		m_bSwordTrail = true;
	}

	if (false == m_bSwordPower && 52.0 < m_pModelCom->Get_PlayAcc())
	{
		m_bSwordPower = true;

		CSwordPower::BULLETDESC tBulletDesc = {};
		tBulletDesc.pModel = m_pModelCom;
		tBulletDesc.pTransform = m_pTransformCom;
		tBulletDesc.pSocketName = "3924_weapon01";
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_SwordPower"), TEXT("Prototype_GameObject_SwordPower"), &tBulletDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::BlinkAttack3_1");
			return;
		}
	}

	if (26.0 > m_pModelCom->Get_PlayAcc())
		m_pTransformCom->LookTurn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pPlayerPosition, TimeDelta * 5.f);

	if (true == m_pModelCom->Get_EndAnimation(BLINKATTACK3_1))
	{
		m_eState = BLINKATTACK3_2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bSwordPower = false;
		m_bSwordTrail = false;
	}
}

void CPoleMonarch::BlinkAttack3_2(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(BLINKATTACK3_2))
	{
		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_eState = WALK_B;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			m_eState = WALK_F;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			m_eState = WALK_L;
			m_pTransformCom->Set_Speed(2.f);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			m_eState = WALK_R;
			m_pTransformCom->Set_Speed(2.f);
		}
		else
		{
			if (true == m_bFirePattern)
				m_eState = RUSH1;
			else
			{
				++m_iShotPattern;
				m_eState = BLINK1;
			}
			m_iAnimationType = ANIM_ONETIME;
			m_pTransformCom->Set_Speed(10.f);
		}
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}

void CPoleMonarch::End1(_double TimeDelta)
{
	static _bool	bFinalPatternSound = false;
	static _bool	bRoar = false;
	static _bool	bShake = false;
	m_iForce += 1;
	if (80 < m_iForce)
		m_iForce = 80;

	if (38.0 < m_pModelCom->Get_PlayAcc() && false == bRoar)
	{
		static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Set_HorizonShake(1.f, 1.2f);
		bRoar = true;
	}

	if (144.0 < m_pModelCom->Get_PlayAcc() && false == bShake)
	{
		static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Set_VerticalShake(0.2f, 2.f);
		bShake = true;
	}

	if (209.0 < m_pModelCom->Get_PlayAcc())
	{
		m_fRunTime += (_float)TimeDelta;
		if (false == bFinalPatternSound)
		{
			g_pGameInstance->StopSound(CSoundMgr::BGM);
			g_pGameInstance->LetsPlaySound(TEXT("Boss2FinalPattern.mp3"), CSoundMgr::BGM, 0.5f);
			static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Set_VerticalShake(0.2f, 2.f);

			bFinalPatternSound = true;
		}
		if (0.2f < m_fRunTime)
		{
			CEndPhase::ENDPHASEDESC tEndPhaseDesc;
			tEndPhaseDesc.vColor = _float4(0.45f, 0.46f, 1.f, 1.f);
			tEndPhaseDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EndPhase"), &tEndPhaseDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::End1");
				return;
			}
			m_fRunTime = 0.f;
		}

	}

	if (true == m_pModelCom->Get_EndAnimation(END1))
	{
		m_eState = END2;
		m_iAnimationType = ANIM_ROOF;
		m_pModelCom->SetUp_AnimIndex(m_eState);

		m_bStrong = false;
		static_cast<CBossHp*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossHp")))->Set_Pass(4, 81);
		m_tStatus.fMaxHp = 10000.f;
		m_tStatus.fCurrentHp = 10000.f;
	}
}

void CPoleMonarch::End2(_double TimeDelta)
{
	m_fFireTime += (_float)TimeDelta;
	static _float fForce = 80.f;
	fForce -= 8 * (_float)TimeDelta;
	if (0 > fForce)
		fForce = 0.f;
	m_iForce = (_int)fForce;

	m_fRunTime += (_float)TimeDelta;

	if (0.1f < m_fRunTime)
	{
		CEndPhase::ENDPHASEDESC tEndPhaseDesc;
		_int iRandom = rand() % 4;
		if (0 == iRandom)
			tEndPhaseDesc.vColor = _float4(0.45f, 0.46f, 1.f, 1.f);
		else if (1 == iRandom)
			tEndPhaseDesc.vColor = _float4(0.8f, 0.46f, 1.f, 1.f);
		else if (2 == iRandom)
			tEndPhaseDesc.vColor = _float4(0.45f, 0.8f, 1.f, 1.f);
		else if (3 == iRandom)
			tEndPhaseDesc.vColor = _float4(0.8f, 0.f, 1.f, 1.f);
		tEndPhaseDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EndPhase"), &tEndPhaseDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::End1");
			return;
		}
		m_fRunTime = 0.f;
	}
	
	if (10.f < m_fFireTime)
	{
		m_eState = END3;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pTransformCom->Set_Speed(10.f);
		m_fFireTime = 0.f;

		m_tStatus.fCurrentHp = 10000.f;
		m_iForce = 80;
		fForce = 80.f;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BossTornado"), m_pTransformCom)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::End2");
			return;
		}
	}
}

void CPoleMonarch::End3(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(END3))
	{
		m_eState = BLINK1;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pTransformCom->Set_Speed(10.f);

		m_bPhase4 = true;
		m_RockList = *g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_RevolutionRock"));

		g_pGameInstance->StopSound(CSoundMgr::BGM);
		g_pGameInstance->PlayBGM(TEXT("Stage4_2.mp3"), 0.9f);
	}
}

_int CPoleMonarch::Hit(_float fAttack, _float fCriticalValue, _bool bHeavy)
{
	if (true == m_bStrong)
		return -1;

	_float fDamage = fAttack * (1.f - m_tStatus.fDefend / (m_tStatus.fDefend + 100.f)) * fCriticalValue;
	fDamage += ((rand() % _int(fDamage * 0.2f)) - fDamage * 0.1f);

	if (true == static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_Talents()[0])
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

	if (m_eState != DIE && 0.f >= m_tStatus.fCurrentHp)
	{
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Victory"), TEXT("Prototype_GameObject_Victory"))))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPoleMonarch::Hit");
			return E_FAIL;
		}
		m_tStatus.fCurrentHp = 0.f;
		m_eState = DIE;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		m_bIce = false;
		m_bFreeze = false;
		m_fFreezeTime = 0.f;
		m_fCurrentFreezeTime = 0.f;
		m_bChain = false;
		m_bPhase4 = false;
		if (true == m_bBurning || true == m_bShock || true == m_bDecay || true == m_bManipulation || true == m_bCombustion || true == m_bMiasma)
			static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_Oil();
		static_cast<CBossHp*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossHp")))->Set_Hp((_uint)m_tStatus.fMaxHp, (_uint)m_tStatus.fCurrentHp);

	}
	else if (m_tStatus.fMaxHp * 0.5f >= m_tStatus.fCurrentHp)
		m_iBossPhase = 2;
	return (_int)fDamage;
}

HRESULT CPoleMonarch::StartSetting()
{
	return S_OK;
}

HRESULT CPoleMonarch::Add_ElementIcon()
{
	return E_NOTIMPL;
}

HRESULT CPoleMonarch::SetUp_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_PoleMonarch"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Model)");
		return E_FAIL;
	}

	m_BodyCollider.resize(5);
	m_WeakCollider.resize(1);
	m_AttackCollider.resize(3);

	/* For.Com_Collder */

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(2.08f, 2.08f, 2.08f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(-0.59f, 0.f, 0.f, 1.f);

	// 몸통
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body0"), (CComponent**)&m_BodyCollider[0], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Collider_Body0)");
		return E_FAIL;
	}

	// 왼손 오른손
	tColliderDesc.vScale = _float3(0.7f, 0.7f, 0.7f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body1"), (CComponent**)&m_BodyCollider[1], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Collider_Body1)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body2"), (CComponent**)&m_BodyCollider[2], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Collider_Body2)");
		return E_FAIL;
	}

	// 왼어깨 오른어깨
	tColliderDesc.vScale = _float3(0.6f, 0.6f, 0.6f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body3"), (CComponent**)&m_BodyCollider[3], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Collider_Body3)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body4"), (CComponent**)&m_BodyCollider[4], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Collider_Body4)");
		return E_FAIL;
	}

	// 머리
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Weak0"), (CComponent**)&m_WeakCollider[0], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Collider_Weak0)");
		return E_FAIL;
	}

	// 무기
	tColliderDesc.vScale = _float3(1.2f, 1.2f, 1.2f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack0"), (CComponent**)&m_AttackCollider[0], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Collider_Attack0)");
		return E_FAIL;
	}

	tColliderDesc.vScale = _float3(1.3f, 1.3f, 1.3f);
	//tColliderDesc.vTranslation = _float4(0.f, 0.f, 1.71f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack1"), (CComponent**)&m_AttackCollider[1], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Collider_Attack1)");
		return E_FAIL;
	}

	//tColliderDesc.vTranslation = _float4(2.39f, 0.f, 0.93f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack2"), (CComponent**)&m_AttackCollider[2], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Collider_Attack2)");
		return E_FAIL;
	}

	for (auto& pBodyCollider : m_BodyCollider)
		pBodyCollider->Set_Parent(this);
	for (auto& pWeakCollider : m_WeakCollider)
		pWeakCollider->Set_Parent(this);
	for (auto& pAttackCollider : m_AttackCollider)
		pAttackCollider->Set_Parent(this);

	// Texture
	if (FAILED(__super::Add_Component(g_eCurrentLevel, TEXT("Prototype_Component_Texture_Boss_Ice1"), TEXT("Com_Texture_Boss_Ice1"), (CComponent**)&m_pTextureCom[0])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Texture_Boss_Ice1)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(g_eCurrentLevel, TEXT("Prototype_Component_Texture_Boss_Ice2"), TEXT("Com_Texture_Boss_Ice2"), (CComponent**)&m_pTextureCom[1])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Texture_Boss_Ice2)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(g_eCurrentLevel, TEXT("Prototype_Component_Texture_BossBlade"), TEXT("Com_Texture_Boss_Blade"), (CComponent**)&m_pTextureCom[2])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Prototype_Component_Texture_BossBlade)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(g_eCurrentLevel, TEXT("Prototype_Component_Texture_BossBlades"), TEXT("Com_Texture_Boss_Blades"), (CComponent**)&m_pTextureCom[3])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPoleMonarch::SetUp_Components(Prototype_Component_Texture_BossBlade)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPoleMonarch::Collider_Update()
{
	for (auto& pBodyCollider : m_BodyCollider)
	{
		if (nullptr == pBodyCollider)
		{
			MSGBOX("nullptr == pBodyCollider in CPoleMonarch::Collider_Update");
			return E_FAIL;
		}
	}
	for (auto& pWeakCollider : m_WeakCollider)
	{
		if (nullptr == pWeakCollider)
		{
			MSGBOX("nullptr == pWeakCollider in CPoleMonarch::Collider_Update");
			return E_FAIL;
		}
	}
	for (auto& pAttackCollider : m_AttackCollider)
	{
		if (nullptr == pAttackCollider)
		{
			MSGBOX("nullptr == pAttackCollider in CPoleMonarch::Collider_Update");
			return E_FAIL;
		}
	}
	CHierarchyNode* pNode = nullptr;



#pragma region Body_Collider
	pNode = m_pModelCom->Find_HierarchyNode("CenterPoint");
	SelectCollider_Update(pNode, m_BodyCollider, 0);

	pNode = m_pModelCom->Find_HierarchyNode("Bip001 L Hand");
	SelectCollider_Update(pNode, m_BodyCollider, 1);

	pNode = m_pModelCom->Find_HierarchyNode("Bip001 R Hand");
	SelectCollider_Update(pNode, m_BodyCollider, 2);

	pNode = m_pModelCom->Find_HierarchyNode("Bip001 L UpperArm");
	SelectCollider_Update(pNode, m_BodyCollider, 3);

	pNode = m_pModelCom->Find_HierarchyNode("Bip001 R UpperArm");
	SelectCollider_Update(pNode, m_BodyCollider, 4);
#pragma endregion


#pragma region Weak_Collider
	pNode = m_pModelCom->Find_HierarchyNode("Bip001 Head");
	SelectCollider_Update(pNode, m_WeakCollider, 0);
#pragma endregion

#pragma region Attack_Collider
	pNode = m_pModelCom->Find_HierarchyNode("bone_09");
	SelectCollider_Update(pNode, m_AttackCollider, 0);

	pNode = m_pModelCom->Find_HierarchyNode("mod");
	SelectCollider_Update(pNode, m_AttackCollider, 1);

	pNode = m_pModelCom->Find_HierarchyNode("3924_weapon01");
	SelectCollider_Update(pNode, m_AttackCollider, 2);
#pragma endregion

	return S_OK;
}

HRESULT CPoleMonarch::SelectCollider_Update(CHierarchyNode * pNode, vector<CCollider*> ColliderContainer, _uint iIndex)
{
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CPoleMonarch::SelectCollider_Update");
		return E_FAIL;
	}
	_matrix			TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	ColliderContainer[iIndex]->Update_Transform(TransformMatrix);

	return S_OK;
}

HRESULT CPoleMonarch::State_Manage(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nullptr in CPoleMonarch::State_Manage");
		return E_FAIL;
	}

	switch (m_eState)
	{
	case STAND:
		Stand(TimeDelta);
		break;
	case WALK_F:
		Walk_Front(TimeDelta);
		break;
	case WALK_B:
		Walk_Back(TimeDelta);
		break;
	case WALK_L:
		Walk_Left(TimeDelta);
		break;
	case WALK_R:
		Walk_Right(TimeDelta);
		break;
	case DIE:
		Die(TimeDelta);
		break;
	case RUN:
		Run(TimeDelta);
		break;
	case ATTACK1_1:
		Attack1_1(TimeDelta);
		break;
	case ATTACK1_2:
		Attack1_2(TimeDelta);
		break;
	case ATTACK2_1:
		Attack2_1(TimeDelta);
		break;
	case ATTACK2_2:
		Attack2_2(TimeDelta);
		break;
	case ATTACK3_1:
		Attack3_1(TimeDelta);
		break;
	case ATTACK3_2:
		Attack3_2(TimeDelta);
		break;
	case TRIPLEATTACK_1:
		TripleAttack_1(TimeDelta);
		break;
	case TRIPLEATTACK_2:
		TripleAttack_2(TimeDelta);
		break;
	case TRIPLEATTACK_3:
		TripleAttack_3(TimeDelta);
		break;
	case TRIPLEATTACK_4:
		TripleAttack_4(TimeDelta);
		break;
	case SHOT1:
		Shot1(TimeDelta);
		break;
	case SHOT2:
		Shot2(TimeDelta);
		break;
	case SHOT3:
		Shot3(TimeDelta);
		break;
	case RUSH1:
		Rush1(TimeDelta);
		break;
	case RUSH2:
		Rush2(TimeDelta);
		break;
	case FIRE1:
		Fire1(TimeDelta);
		break;
	case FIRE2:
		Fire2(TimeDelta);
		break;
	case FIRE3:
		Fire3(TimeDelta);
		break;
	case BLINK1:
		Blink1(TimeDelta);
		break;
	case BLINK2:
		Blink2(TimeDelta);
		break;
	case BLINKATTACK1_1:
		BlinkAttack1_1(TimeDelta);
		break;
	case BLINKATTACK1_2:
		BlinkAttack1_2(TimeDelta);
		break;
	case BLINKATTACK2_1:
		BlinkAttack2_1(TimeDelta);
		break;
	case BLINKATTACK2_2:
		BlinkAttack2_2(TimeDelta);
		break;
	case BLINKATTACK3_1:
		BlinkAttack3_1(TimeDelta);
		break;
	case BLINKATTACK3_2:
		BlinkAttack3_2(TimeDelta);
		break;
	case END1:
		End1(TimeDelta);
		break;
	case END2:
		End2(TimeDelta);
		break;
	case END3:
		End3(TimeDelta);
		break;
	}

	return S_OK;
}

CPoleMonarch * CPoleMonarch::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPoleMonarch*	pInstance = new CPoleMonarch(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPoleMonarch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPoleMonarch::Clone(void * pArg)
{
	CPoleMonarch*	pInstance = new CPoleMonarch(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPoleMonarch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPoleMonarch::Free()
{
	__super::Free();

	for (_uint i = 0; i < 4; i++)
		Safe_Release(m_pTextureCom[i]);

	for (auto& pBodyCollider : m_BodyCollider)
		Safe_Release(pBodyCollider);
	for (auto& pWeakCollider : m_WeakCollider)
		Safe_Release(pWeakCollider);
	for (auto& pAttackCollider : m_AttackCollider)
		Safe_Release(pAttackCollider);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
}
