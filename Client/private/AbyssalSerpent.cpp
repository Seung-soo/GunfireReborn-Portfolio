#include "stdafx.h"
#include "..\public\AbyssalSerpent.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"
#include "Player_Hand.h"
#include "Wave.h"
#include "WaterBall.h"
#include "BossHp.h"
#include "StageWall.h"
#include "MonsterWave.h"
#include "RushWind.h"
#include "ReadyWaterBall.h"
#include "WaterMoveWave.h"

CAbyssalSerpent::CAbyssalSerpent(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{

}

CAbyssalSerpent::CAbyssalSerpent(const CAbyssalSerpent & rhs)
	: CMonster(rhs)
{
}

void CAbyssalSerpent::Set_Type(TYPE eType, STATUS * pRealStatus)
{
	m_eType = eType;

	m_eState = RUSH1;
	m_iAnimationType = ANIM_ONETIME;
	m_pModelCom->SetUp_AnimIndex(m_eState);
	m_pModelCom->SetUp_AnimIndex(m_eState);

	if (TYPE_REAL != m_eType)
	{
		m_pRealStatus = pRealStatus;
		m_pReal = static_cast<CAbyssalSerpent*>(g_pGameInstance->Get_Front(g_eCurrentLevel, TEXT("Layer_Boss")));
	}
	else
	{
		INFO_UI tInfoUI = {};
		tInfoUI.fScaleX = 24.f;
		tInfoUI.fScaleY = 24.f;
		tInfoUI.fDepth = 3.f;
		tInfoUI.iTextureIndex = CMiniMap_Icon::TARGET_BOSS1;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon"), TEXT("Prototype_GameObject_MiniMap_Icon"), &tInfoUI)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::NativeConstruct");
			return;
		}
		m_pMiniMap_Icon = static_cast<CMiniMap_Icon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon")));
		m_pMiniMap_Icon->Set_Target(m_pTransformCom);
	}
}

HRESULT CAbyssalSerpent::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CAbyssalSerpent::NativeContruct_Prototype");
		return E_FAIL;
	}

	m_tStatus.fMaxHp = 100000.f;
	m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
	m_tStatus.fMaxShield = 0.f;
	m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
	m_tStatus.fAttack = 50.f;
	m_tStatus.fDefend = 60.f;

	return S_OK;
}

HRESULT CAbyssalSerpent::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 15.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CAbyssalSerpent::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CAbyssalSerpent::NativeConstruct");
		return E_FAIL;
	}
	m_pModelCom->SetUp_AnimIndex(m_eState);
	m_pModelCom->SetUp_AnimIndex(m_eState);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(27.53f, -4.f, 115.91f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, -1.f, 0.f));
	m_pTransformCom->ArrangeMatrix();
	m_bRender = false;

	m_bIsBoss = true;

	return S_OK;
}

_int CAbyssalSerpent::Tick(_double dTimeDelta)
{
	if (TYPE_REAL != m_eType && DIE == m_pReal->Get_State())
		return RETURN_OBJECTPOOL;

	if (true == m_bDead)
	{
		m_bDead = false;

		if (nullptr != m_pMiniMap_Icon)
		{
			m_pMiniMap_Icon->Set_Dead(true);
			m_pMiniMap_Icon = nullptr;
		}

		if (TYPE_REAL == m_eType)
		{
			m_pRendererCom->Set_DontRenderUI(false);
			m_dCameraSlow = 1.0;
			m_bCameraScene = false;
			g_bCameraAction = false;

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Victory"), TEXT("Prototype_GameObject_Victory"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Tick");
				return E_FAIL;
			}

		}


		m_bFreeze = false;
		m_fFreezeTime = 0.f;
		m_fCurrentFreezeTime = 0.f;
		m_bChain = false;

		return RETURN_OBJECTPOOL;
	}

	if (TYPE_REAL == m_eType && 0.f >= m_tStatus.fCurrentHp && false == m_bCameraScene)
	{
		m_eState = DIE;
		m_pModelCom->SetUp_AnimIndex(DIE);
		m_dCameraSlow = 0.6;
		m_bCameraScene = true;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(27.53f, -4.f, 115.91f, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, -1.f, 0.f));
		m_pTransformCom->ArrangeMatrix();
		m_pRendererCom->Set_DontRenderUI(true);
		g_bCameraAction = true;
		m_pPlayerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(27.53f, 3.f, 85.f, 1.f));
		static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->LookAtXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-20.f));

		m_iAnimationType = ANIM_ONETIME;

		m_bFreeze = false;
		m_fFreezeTime = 0.f;
		m_fCurrentFreezeTime = 0.f;
		m_bChain = false;
	}

	if (true == m_bCameraScene)
		static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Go_Vector(XMVectorSet(0.f, 0.f, -1.f, 0.f), dTimeDelta * 0.1f);


	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CAbyssalSerpent::Tick");
		return DEAD;
	}
	if (false == m_bFreeze)
		if (FAILED(State_Manage(dTimeDelta)))
		{
			MSGBOX("State_Manage returned E_FAIL in CAbyssalSerpent::Tick");
			return DEAD;
		}

	if (true == m_bFreeze && false == m_bChain)
	{
		CChain::CHAINDESC tChainDesc;
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		//tChainDesc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		tChainDesc.pChain = &m_bChain;
		CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode("Bone020");
		_matrix NodeWorldMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		tChainDesc.WorldMatrix = XMMatrixTranslation(NodeWorldMatrix.r[3].m128_f32[0], NodeWorldMatrix.r[3].m128_f32[1], NodeWorldMatrix.r[3].m128_f32[2]);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_CIRCLE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(3.f, 3.f, 3.f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-30.f)) * XMMatrixTranslation(0.f, 0.3f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_LINE;
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(0.4f, 0.3f, 0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, -1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(-0.5f, 0.3f, -0.3f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(-45.f)) * XMMatrixTranslation(0.4f, 0.3f, -0.4f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Ready_Layer_Effect");
			return E_FAIL;
		}
		tChainDesc.MyWorldMatrix = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(-0.3f, 0.3f, 0.5f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Ready_Layer_Effect");
			return E_FAIL;
		}
		m_bChain = true;
	}

	if (TYPE_REAL == m_eType && m_tStatus.fCurrentHp <= m_tStatus.fMaxHp * 0.5f && false == m_bCopyCall)
	{
		if (FAILED(g_pGameInstance->ThrowToLayer(g_eCurrentLevel, TEXT("Layer_Boss"))))
		{
			MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CAbyssalSerpent::Tick");
			return -1;
		}
		CAbyssalSerpent* pCopy = static_cast<CAbyssalSerpent*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Boss")));
		pCopy->Set_Type(TYPE_COPY, &m_tStatus);
		static_cast<CTransform*>(pCopy->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(27.53f, -4.f, 115.91f, 1.f));
		m_bCopyCall = true;
	}

	if (TYPE_REAL == m_eType && true == m_bGroupAttack)
	{
		m_dGroupAttackTime += dTimeDelta;
		if (0.0 < m_dGroupAttackTime && false == m_bGroup)
		{
			if (FAILED(g_pGameInstance->ThrowToLayer(g_eCurrentLevel, TEXT("Layer_Boss"))))
			{
				MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CAbyssalSerpent::Rush2");
				return DEAD;
			}
			CAbyssalSerpent* pCopy = static_cast<CAbyssalSerpent*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Boss")));
			pCopy->Set_Type(TYPE_ATTACK, &m_tStatus);
			static_cast<CTransform*>(pCopy->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION)), -4.f, 115.91f, 1.f));

			m_bGroup = true;
		}
		else if (1.0 < m_dGroupAttackTime && true == m_bGroup)
		{
			if (FAILED(g_pGameInstance->ThrowToLayer(g_eCurrentLevel, TEXT("Layer_Boss"))))
			{
				MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CAbyssalSerpent::Rush2");
				return DEAD;
			}
			CAbyssalSerpent* pCopy = static_cast<CAbyssalSerpent*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Boss")));
			pCopy->Set_Type(TYPE_ATTACK, &m_tStatus);
			static_cast<CTransform*>(pCopy->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION)), -4.f, 115.91f, 1.f));

			m_bGroup = false;
			m_bGroupAttack = false;
			m_dGroupAttackTime = 0.0;
		}
	}

	if (TYPE_REAL == m_eType && g_pGameInstance->Get_KeyEnter(DIK_K))
		m_tStatus.fCurrentHp = 1.f;
	if (TYPE_REAL == m_eType && g_pGameInstance->Get_KeyEnter(DIK_N))
		m_tStatus.fCurrentHp *= 0.5f;

	CCollider* pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));

	for (auto& pWeakCollider : m_WeakCollider)
	{
		g_pGameInstance->Add_Collider(TEXT("Weakness"), CCollision_Manager::TARGET_ENEMY, pWeakCollider);
		if (false == m_bCollision && true == g_pGameInstance->Collision_Sphere(pWeakCollider, pPlayerCollider))
		{
			static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(100.f);
			m_bFarAttack = true;
			m_bCollision = true;
			break;
		}
	}
	for (auto& pBodyCollider : m_BodyCollider)
	{
		g_pGameInstance->Add_Collider(TEXT("Body"), CCollision_Manager::TARGET_ENEMY, pBodyCollider);
		if (false == m_bCollision && true == g_pGameInstance->Collision_Sphere(pBodyCollider, pPlayerCollider))
		{
			static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(100.f);
			m_bFarAttack = true;
			m_bCollision = true;
			break;
		}
	}

	m_fWaveTime -= _float(dTimeDelta);
	if (0.f > m_fWaveTime)
	{
		// 물파동
		_float4	vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BossWave"), &vPos)))
		{
			__debugbreak();
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Rush1(Prototype_GameObject_WaterEffect)");
			return DEAD;
		}
		m_fWaveTime = m_fWaveInterval;
	}
	return NOPROBLEM;
}

_int CAbyssalSerpent::LateTick(_double dTimeDelta)
{
	if (TYPE_REAL != m_eType && DIE == m_pReal->Get_State())
		return RETURN_OBJECTPOOL;

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CAbyssalSerpent::LateTick");
		return DEAD;
	}

	if (true == m_bMakeUI && TYPE_REAL == m_eType)
	{
		// 체력바 배경
		INFO_UI tInfoUI_HpBackGround;
		ZeroMemory(&tInfoUI_HpBackGround, sizeof(INFO_UI));

		tInfoUI_HpBackGround.fScaleX = 700.f;
		tInfoUI_HpBackGround.fScaleY = 600.f;
		tInfoUI_HpBackGround.fPositionX = 640.f;
		tInfoUI_HpBackGround.fPositionY = 80.f;
		tInfoUI_HpBackGround.fDepth = 4.f;
		tInfoUI_HpBackGround.iTextureIndex = 73;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_BossHpBack"), TEXT("Prototype_GameObject_UI"), &tInfoUI_HpBackGround)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::NativeConstruct(HpBack)");
			return E_FAIL;
		}

		INFO_UI tInfoUI_Hp;
		ZeroMemory(&tInfoUI_Hp, sizeof(INFO_UI));

		tInfoUI_Hp.fScaleX = 430.f;
		tInfoUI_Hp.fScaleY = 370.f;
		tInfoUI_Hp.fPositionX = 655.f;
		tInfoUI_Hp.fPositionY = 95.f;
		tInfoUI_Hp.fDepth = 3.f;
		tInfoUI_Hp.iTextureIndex = 74;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_BossHp"), TEXT("Prototype_GameObject_BossHp"), &tInfoUI_Hp)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::NativeConstruct(Hp)");
			return E_FAIL;
		}
		m_bMakeUI = false;
	}
	if (false == m_bFreeze)
		m_pModelCom->Update_CombinedTransformationMatrices(m_iAnimationType, dTimeDelta * m_dCameraSlow);

	if (FAILED(Collider_Update()))
	{
		MSGBOX("Collider_Update returned E_FAIL in CAbyssalSerpent::LateTick");
		return DEAD;
	}
	if (TYPE_REAL == m_eType && DIE != m_eState)
		static_cast<CBossHp*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_BossHp")))->Set_Hp((_uint)m_tStatus.fMaxHp, (_uint)m_tStatus.fCurrentHp);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
	for (auto& pBodyCollider : m_BodyCollider)
		m_pRendererCom->Add_Debug(pBodyCollider);
	for (auto& pWeakCollider : m_WeakCollider)
		m_pRendererCom->Add_Debug(pWeakCollider);
#endif
	return NOPROBLEM;
}




HRESULT CAbyssalSerpent::Render()
{
	if (false == m_bRender)
		return S_OK;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pRendererCom)
	{
		MSGBOX("nullptr == m_pShaderCom or nullptr == m_pRendererCom in CAbyssalSerpent::Render");
		return E_FAIL;
	}

	if (FAILED(__super::SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CAbyssalSerpent::Render");
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 0)))
	{
		__debugbreak();
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 7, i, "g_BoneMatrices");
	}

	return S_OK;
}


void CAbyssalSerpent::Up(_double TimeDelta)
{
	if (0.2 > m_pModelCom->Get_PlayAcc())
		m_bRender = false;
	else
		m_bRender = true;

	if (true == m_pModelCom->Get_EndAnimation(UP))
	{
		m_eState = STAND;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}

void CAbyssalSerpent::Stand(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(STAND))
	{
		m_eState = WAITING;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pModelCom->Set_EndPlayAcc();
		m_bRender = false;
		m_fWaveInterval = 0.2f;
	}
}

void CAbyssalSerpent::Waiting(_double TimeDelta)
{
	m_dWaitTime += TimeDelta;
	m_bRender = false;
	//if (true == m_bStart)
	//{
	//	if (2.0 < m_dWaitTime)
	//	{
	//		m_eState = RUSH1;
	//		m_iAnimationType = ANIM_ONETIME;
	//		m_pModelCom->SetUp_AnimIndex(m_eState);
	//		m_bStart = false;
	//		m_dWaitTime = 0.0;
	//	}
	//}
	//else 
	//{
		m_pTransformCom->Go_Vector(m_vDir, TimeDelta);
		_float vPosX = XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (16.53f > vPosX || 48.53f < vPosX)
		{
			m_vDir.m128_f32[0] *= -1.f;
			m_pTransformCom->Go_Vector(m_vDir, TimeDelta);
		}

		if (2.0 < m_dWaitTime)
		{
			m_fWaveInterval = 0.35f;
			_uint iPattern = rand() % 2;
			if (m_iBossPhase >= 2 || TYPE_COPY == m_eType)
				iPattern++;

			if (false == m_bFarAttack)
			{
				if (TYPE_REAL == m_eType)
				{
					if (m_tStatus.fCurrentHp >= m_tStatus.fMaxHp * 0.5f)
					{
						m_eState = SWEEP1;
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f));
					}
					else
					{
						if (27.53f > XMVectorGetX(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION)))
						{
							m_eState = SWEEP1;
							m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f));
						}
						else
						{
							m_eState = WATERBALL;
							m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f));
						}
					}
				}
				else
				{
					if (m_pRealStatus->fCurrentHp >= m_pRealStatus->fMaxHp * 0.5f)
					{
						m_eState = SWEEP1;
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f));
					}
					else
					{
						if (27.53f < XMVectorGetX(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION)))
						{
							m_eState = SWEEP1;
							m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f));
						}
						else
						{
							m_eState = WATERBALL;
							m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f));
						}
					}
				}
			}
			else
			{
				m_eState = RUSH1;
				if (TYPE_REAL == m_eType)
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), -4.f));
			}

			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom->SetUp_AnimIndex(m_eState);

			m_pTransformCom->LookAtXZ(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

			m_dWaitTime = 0.0;

			if (m_tStatus.fMaxHp * 0.75f >= m_tStatus.fCurrentHp)
			{
				if (nullptr != g_pGameInstance->Find_Layer(g_eCurrentLevel, TEXT("Layer_Mini")) && 5 > g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_Mini")))
				{
					if (FAILED(g_pGameInstance->ThrowToLayer(g_eCurrentLevel, TEXT("Layer_Mini"))))
					{
						MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CAbyssalSerpent::Waiting");
						return;
					}
				}
			}


		}
	//}
	m_fMoveWaveInterval -= (_float)TimeDelta;
	if (0.f > m_fMoveWaveInterval)
	{
		CWaterMoveWave::MOVEWAVEDESC tMoveWaveDesc;
		tMoveWaveDesc.bIsRight = XMVectorGetX(m_vDir) >= 0.f;
		tMoveWaveDesc.pBossTransform = m_pTransformCom;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_WaterMoveWave"), &tMoveWaveDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Waiting");
			return;
		}
		m_fMoveWaveInterval = 0.2f;
	}
}

void CAbyssalSerpent::Rush1(_double TimeDelta)
{
	m_bRender = true;
	if (false == m_bWaterEffect && 5 < m_pModelCom->Get_PlayAcc())
	{
		_float4	vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_WaterEffect"), &vPos)))
		{
			__debugbreak();
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Rush1(Prototype_GameObject_WaterEffect)");
			return;
		}

		if (g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RushWarning"), m_pTransformCom))
		{
			__debugbreak();
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Rush1(Prototype_GameObject_RushWarning)");
			return;
		}

		m_bWaterEffect = true;
	}
	if (m_pModelCom->Get_EndAnimation(RUSH1))
	{
		m_eState = RUSH2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bCollision = false;

		m_bWaterEffect = false;
		m_bFarAttack = false;
		g_pGameInstance->StopSound(CSoundMgr::EFFECT10);
		g_pGameInstance->LetsPlaySound(TEXT("Boss1Rush.mp3"), CSoundMgr::EFFECT10, 0.7f);
	}
}

void CAbyssalSerpent::Rush2(_double TimeDelta)
{
	if ((TYPE_REAL == m_eType && m_tStatus.fCurrentHp <= m_tStatus.fMaxHp * 0.25f && false == m_bWaterEffect))
	{
		m_bGroupAttack = true;
		m_bWaterEffect = true;
	}
	if (2 < m_pModelCom->Get_PlayAcc())
	{
		CRushWind::RUSHWINDDESC tRushWindDesc;
		tRushWindDesc.pModel = m_pModelCom;
		tRushWindDesc.pTransform = m_pTransformCom;
		tRushWindDesc.pSocketName = "Bone068";
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RushWind"), &tRushWindDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Rush3");
			return;
		}
	}
	if (m_pModelCom->Get_EndAnimation(RUSH2))
	{
		m_eState = RUSH3;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bWaterEffect = false;
	}
}

void CAbyssalSerpent::Rush3(_double TimeDelta)
{
	if (8 < m_pModelCom->Get_PlayAcc())
		m_bCollision = true;
	if (4 > m_pModelCom->Get_PlayAcc())
	{
		CRushWind::RUSHWINDDESC tRushWindDesc;
		tRushWindDesc.pModel = m_pModelCom;
		tRushWindDesc.pTransform = m_pTransformCom;
		tRushWindDesc.pSocketName = "Bone068";
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RushWind"), &tRushWindDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Rush3");
			return;
		}
	}


	if (m_pModelCom->Get_EndAnimation(RUSH3))
	{
		m_eState = RUSH4;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bWaterEffect = false;
	}
}

void CAbyssalSerpent::Rush4(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(RUSH4))
	{
		if (TYPE_ATTACK == m_eType)
		{
			m_bDead = true;
		}
		else
		{
			m_eState = WAITING;
			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_pModelCom->Set_EndPlayAcc();
			m_bRender = false;
			m_fWaveInterval = 0.2f;
		}
	}
}

void CAbyssalSerpent::Sweep1(_double TimeDelta)
{
	if (1 < m_pModelCom->Get_PlayAcc())
		m_bRender = true;

	if (false == m_bWaterEffect && 4 < m_pModelCom->Get_PlayAcc())
	{
		_float4	vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_WaterEffect"), &vPos)))
		{
			__debugbreak();
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::WaterBall(Prototype_GameObject_WaterEffect)");
			return;
		}

		m_bWaterEffect = true;
	}

	if (m_pModelCom->Get_EndAnimation(SWEEP1))
	{
		m_eState = SWEEP2;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bWaterEffect = false;
	}
}

void CAbyssalSerpent::Sweep2(_double TimeDelta)
{
	if (false == m_bWaveSound && 10.0 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT11);
		g_pGameInstance->LetsPlaySound(TEXT("Boss1Wave.mp3"), CSoundMgr::EFFECT11, 0.7f);
		m_bWaveSound = true;
	}
	if (true == m_bMakeWave && 21.0 < m_pModelCom->Get_PlayAcc())
	{
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Wave"), TEXT("Prototype_GameObject_Wave"), &_float4(40.f, 2.6f, 92.f, 1.f))))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Sweep2");
			return;
		}
		m_bMakeWave = false;
	}
	if (m_pModelCom->Get_EndAnimation(SWEEP2))
	{
		m_eState = SWEEP3;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bMakeWave = true;
		m_bWaveSound = false;
	}
}

void CAbyssalSerpent::Sweep3(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(SWEEP3))
	{
		m_eState = WAITING;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pModelCom->Set_EndPlayAcc();
		m_bRender = false;
		if (TYPE_REAL == m_eType)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f));
		else
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), -4.f));
		m_fWaveInterval = 0.2f;

		m_bFarAttack = true;
	}
}

void CAbyssalSerpent::WaterBall(_double TimeDelta)
{

	if (5 < m_pModelCom->Get_PlayAcc())
		m_bRender = true;

	if (false == m_bWaterEffect && 9 < m_pModelCom->Get_PlayAcc())
	{
		_float4	vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_WaterEffect"), &vPos)))
		{
			__debugbreak();
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::WaterBall(Prototype_GameObject_WaterEffect)");
			return;
		}

		m_bWaterEffect = true;
	}

	if (false == m_bFire && 52 < m_pModelCom->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT12);
		g_pGameInstance->LetsPlaySound(TEXT("Boss1WaterBallFire.mp3"), CSoundMgr::EFFECT12, 0.3f);
		CWaterBall::BULLETDESC tBulletDesc = {};
		tBulletDesc.pModel = m_pModelCom;
		tBulletDesc.pTransform = m_pTransformCom;
		tBulletDesc.pSocketName = "Bone067";

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_WaterBall"), TEXT("Prototype_GameObject_WaterBall"), &tBulletDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::WaterBall");
			return;
		}
		m_bFire = true;
	}

	if (44 > m_pModelCom->Get_PlayAcc())
	{
		m_pTransformCom->LookAtXZ(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

		if (false == m_bReadyWaterBall && 24 < m_pModelCom->Get_PlayAcc())
		{
			CReadyWaterBall::AIMDESC tAimDesc;
			tAimDesc.pModel = m_pModelCom;
			tAimDesc.pParent = this;
			tAimDesc.pTransform = m_pTransformCom;
			tAimDesc.pSocketName = "Bone067";
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ReadyWaterBall"), &tAimDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::WaterBall(Prototype_GameObject_ReadyWaterBall)");
				return;
			}
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ReadyWaterBall"), &tAimDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::WaterBall(Prototype_GameObject_ReadyWaterBall)");
				return;
			}
			CReadyWaterBall* pReadyWaterBall = static_cast<CReadyWaterBall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Effect")));
			CTransform* pReadyWaterBallTransform = static_cast<CTransform*>(pReadyWaterBall->Get_Component(TEXT("Com_Transform")));
			pReadyWaterBallTransform->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(60.f));
			pReadyWaterBallTransform->Scaled(XMVectorSet(2.f, 2.f, 2.f, 0.f));
			//pReadyWaterBall->Set_MoveUV(1.5f);

			m_bReadyWaterBall = true;
		}
	}
	else if (false == m_bWaterBallWarning)
	{
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_WaterBallWarning"), &m_pPlayerTransform)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::WaterBall(Prototype_GameObject_WaterBallWarning)");
			return;
		}


		m_bWaterBallWarning = true;
	}

	if (m_pModelCom->Get_EndAnimation(WATERBALL))
	{
		m_eState = WAITING;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_pModelCom->Set_EndPlayAcc();
		m_bRender = false;
		if (TYPE_REAL == m_eType)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f));
		else
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), -4.f));
		m_bFire = false;
		m_bWaterBallWarning = false;
		m_bReadyWaterBall = false;
		m_bFarAttack = true;
		m_fWaveInterval = 0.2f;
		m_bWaterEffect = false;
	}
}

void CAbyssalSerpent::Die(_double TimeDelta)
{
	m_bRender = true;
	if (TYPE_REAL == m_eType)
	{
		if (false == m_bWaterEffect && 55.0 < m_pModelCom->Get_PlayAcc())
		{
			g_pGameInstance->StopSound(CSoundMgr::MONSTER1);
			g_pGameInstance->LetsPlaySound(TEXT("Boss1Die2.mp3"), CSoundMgr::MONSTER1, 0.5f);
			m_bWaterEffect = true;
		}
	}
	if (m_pModelCom->Get_EndAnimation(DIE))
	{
		if (TYPE_REAL == m_eType)
		{
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_BossHpBack"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CAbyssalSerpent::Die");
				return;
			}
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_BossHp"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CAbyssalSerpent::Die");
				return;
			}

			CMonsterWave::GetInstance()->ReleaseWall();

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Portal"), TEXT("Prototype_GameObject_StageWall"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Die");
				return;
			}
			static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Portal")))->WallSetting(22, CStageWall::LINE_BC, true);
			m_bDead = true;
			m_dCameraSlow = 1.0;
			m_bCameraScene = false;
		}
		else
		{
			m_bDead = true;
		}
	}
}

_int CAbyssalSerpent::Hit(_float fAttack, _float fCriticalValue, _bool bHeavy)
{
	if (TYPE_REAL == m_eType)
	{
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

		if (0.f >= m_tStatus.fCurrentHp)
		{
			g_pGameInstance->StopSound(CSoundMgr::BGM);

			g_pGameInstance->StopSound(CSoundMgr::MONSTER1);
			g_pGameInstance->LetsPlaySound(TEXT("Boss1Die.mp3"), CSoundMgr::MONSTER1, 0.5f);
			m_tStatus.fCurrentHp = 0.f;
			m_eState = DIE;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;

			m_bFreeze = false;
			m_fFreezeTime = 0.f;
			m_fCurrentFreezeTime = 0.f;
			m_bChain = false;
			if (true == m_bBurning || true == m_bShock || true == m_bDecay || true == m_bManipulation || true == m_bCombustion || true == m_bMiasma)
				static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_Oil();
		}
		else if (m_tStatus.fMaxHp * 0.5f >= m_tStatus.fCurrentHp)
			m_iBossPhase = 2;
		return (_int)fDamage;
	}
	else
	{
		_float fDamage = fAttack * (1.f - m_pRealStatus->fDefend / (m_pRealStatus->fDefend + 100.f)) * fCriticalValue;
		fDamage += ((rand() % _int(fDamage * 0.2f)) - fDamage * 0.1f);

		if (true == static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_Talents()[0])
			fDamage *= 1.6f;
		if (m_pRealStatus->fCurrentShield > 0)
		{
			fDamage /= 3.f;
			if (m_pRealStatus->fCurrentShield < fDamage)
			{
				fDamage -= m_pRealStatus->fCurrentShield;
				fDamage *= 3.f;

				m_pRealStatus->fCurrentShield = 0;
				m_pRealStatus->fCurrentHp -= fDamage;
			}
			else
			{
				m_pRealStatus->fCurrentShield -= fDamage;
			}
		}
		else
		{
			m_pRealStatus->fCurrentHp -= fDamage;
		}

		if (0.f >= m_pRealStatus->fCurrentHp)
		{
			g_pGameInstance->StopSound(CSoundMgr::BGM);

			g_pGameInstance->StopSound(CSoundMgr::MONSTER1);
			g_pGameInstance->LetsPlaySound(TEXT("Boss1Die.mp3"), CSoundMgr::MONSTER1, 0.5f);

			m_pRealStatus->fCurrentHp = 0.f;
			m_eState = DIE;
			m_pModelCom->SetUp_AnimIndex(m_eState);
			m_iAnimationType = ANIM_ONETIME;

			m_bFreeze = false;
			m_fFreezeTime = 0.f;
			m_fCurrentFreezeTime = 0.f;
			m_bChain = false;
			if (true == m_bBurning || true == m_bShock || true == m_bDecay || true == m_bManipulation || true == m_bCombustion || true == m_bMiasma)
				static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_Oil();
		}
		else if (m_pRealStatus->fMaxHp * 0.5f >= m_pRealStatus->fCurrentHp)
			m_iBossPhase = 2;
		return (_int)fDamage;
	}

}

HRESULT CAbyssalSerpent::StartSetting()
{
	return S_OK;
}

HRESULT CAbyssalSerpent::Add_ElementIcon()
{
	return E_NOTIMPL;
}

HRESULT CAbyssalSerpent::SetUp_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_AbyssalSerpent"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_AbyssalSerpent_Mask"), TEXT("Com_MaskTexure"), (CComponent**)&m_pMaskTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Com_MaskTexure)");
		return E_FAIL;
	}

	m_BodyCollider.resize(15);
	m_WeakCollider.resize(3);

	/* For.Com_Collder */

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body0"), (CComponent**)&m_BodyCollider[0], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body0)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body1"), (CComponent**)&m_BodyCollider[1], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body1)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body2"), (CComponent**)&m_BodyCollider[2], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body2)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body3"), (CComponent**)&m_BodyCollider[3], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body3)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body4"), (CComponent**)&m_BodyCollider[4], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body4)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body5"), (CComponent**)&m_BodyCollider[5], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body5)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body6"), (CComponent**)&m_BodyCollider[6], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body6)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body7"), (CComponent**)&m_BodyCollider[7], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body7)");
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body8"), (CComponent**)&m_BodyCollider[8], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body8)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body9"), (CComponent**)&m_BodyCollider[9], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body9)");
		return E_FAIL;
	}

	tColliderDesc.vScale = _float3(3.5f, 3.5f, 3.5f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body10"), (CComponent**)&m_BodyCollider[10], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body10)");
		return E_FAIL;
	}

	tColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body11"), (CComponent**)&m_BodyCollider[11], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body11)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body12"), (CComponent**)&m_BodyCollider[12], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body12)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body13"), (CComponent**)&m_BodyCollider[13], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body13)");
		return E_FAIL;
	}

	tColliderDesc.vScale = _float3(3.5f, 3.5f, 3.5f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body14"), (CComponent**)&m_BodyCollider[14], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body14)");
		return E_FAIL;
	}



	tColliderDesc.vScale = _float3(4.1f, 4.1f, 4.1f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Weak0"), (CComponent**)&m_WeakCollider[0], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Weak0)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Weak1"), (CComponent**)&m_WeakCollider[1], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Weak1)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Weak2"), (CComponent**)&m_WeakCollider[2], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Weak2)");
		return E_FAIL;
	}

	for (auto& pBodyCollider : m_BodyCollider)
		pBodyCollider->Set_Parent(this);
	for (auto& pWeakCollider : m_WeakCollider)
		pWeakCollider->Set_Parent(this);

	return S_OK;
}

HRESULT CAbyssalSerpent::Collider_Update()
{
	for (auto& pBodyCollider : m_BodyCollider)
	{
		if (nullptr == pBodyCollider)
		{
			MSGBOX("nullptr == pBodyCollider in CAbyssalSerpent::Collider_Update");
			return E_FAIL;
		}
	}
	for (auto& pWeakCollider : m_WeakCollider)
	{
		if (nullptr == pWeakCollider)
		{
			MSGBOX("nullptr == pWeakCollider in CAbyssalSerpent::Collider_Update");
			return E_FAIL;
		}
	}
	CHierarchyNode* pNode = nullptr;



#pragma region Body_Collider
	pNode = m_pModelCom->Find_HierarchyNode("Bone002");
	SelectCollider_Update(pNode, m_BodyCollider, 0);

	pNode = m_pModelCom->Find_HierarchyNode("Bone004");
	SelectCollider_Update(pNode, m_BodyCollider, 1);

	pNode = m_pModelCom->Find_HierarchyNode("Bone006");
	SelectCollider_Update(pNode, m_BodyCollider, 2);

	pNode = m_pModelCom->Find_HierarchyNode("Bone008");
	SelectCollider_Update(pNode, m_BodyCollider, 3);

	pNode = m_pModelCom->Find_HierarchyNode("Bone010");
	SelectCollider_Update(pNode, m_BodyCollider, 4);

	pNode = m_pModelCom->Find_HierarchyNode("Bone012");
	SelectCollider_Update(pNode, m_BodyCollider, 5);

	pNode = m_pModelCom->Find_HierarchyNode("Bone014");
	SelectCollider_Update(pNode, m_BodyCollider, 6);

	pNode = m_pModelCom->Find_HierarchyNode("Bone016");
	SelectCollider_Update(pNode, m_BodyCollider, 7);

	pNode = m_pModelCom->Find_HierarchyNode("Bone024");
	SelectCollider_Update(pNode, m_BodyCollider, 8);

	pNode = m_pModelCom->Find_HierarchyNode("Bone026");
	SelectCollider_Update(pNode, m_BodyCollider, 9);

	pNode = m_pModelCom->Find_HierarchyNode("Bone028");
	SelectCollider_Update(pNode, m_BodyCollider, 10);

	pNode = m_pModelCom->Find_HierarchyNode("Bone030");
	SelectCollider_Update(pNode, m_BodyCollider, 11);

	pNode = m_pModelCom->Find_HierarchyNode("Bone032");
	SelectCollider_Update(pNode, m_BodyCollider, 12);

	pNode = m_pModelCom->Find_HierarchyNode("Bone034");
	SelectCollider_Update(pNode, m_BodyCollider, 13);

	pNode = m_pModelCom->Find_HierarchyNode("Bone068");
	SelectCollider_Update(pNode, m_BodyCollider, 14);
#pragma endregion


#pragma region Weak_Collider
	pNode = m_pModelCom->Find_HierarchyNode("Bone018");
	SelectCollider_Update(pNode, m_WeakCollider, 0);

	pNode = m_pModelCom->Find_HierarchyNode("Bone020");
	SelectCollider_Update(pNode, m_WeakCollider, 1);

	pNode = m_pModelCom->Find_HierarchyNode("Bone022");
	SelectCollider_Update(pNode, m_WeakCollider, 2);
#pragma endregion


	return S_OK;
}

HRESULT CAbyssalSerpent::SelectCollider_Update(CHierarchyNode * pNode, vector<CCollider*> ColliderContainer, _uint iIndex)
{
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CAbyssalSerpent::SelectCollider_Update");
		return E_FAIL;
	}
	_matrix			TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	ColliderContainer[iIndex]->Update_Transform(TransformMatrix);

	return S_OK;
}

HRESULT CAbyssalSerpent::State_Manage(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nullptr in CAbyssalSerpent::State_Manage");
		return E_FAIL;
	}

	switch (m_eState)
	{
	case RUSH4:
		Rush4(TimeDelta);
		break;
	case RUSH3:
		Rush3(TimeDelta);
		break;
	case RUSH2:
		Rush2(TimeDelta);
		break;
	case RUSH1:
		Rush1(TimeDelta);
		break;
	case WATERBALL:
		WaterBall(TimeDelta);
		break;
	case STAND:
		Stand(TimeDelta);
		break;
	case DIE:
		Die(TimeDelta);
		break;
	case WAITING:
		Waiting(TimeDelta);
		break;
	case UP:
		Up(TimeDelta);
		break;
	case SWEEP3:
		Sweep3(TimeDelta);
		break;
	case SWEEP2:
		Sweep2(TimeDelta);
		break;
	case SWEEP1:
		Sweep1(TimeDelta);
		break;
	}

	return S_OK;
}

CAbyssalSerpent * CAbyssalSerpent::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CAbyssalSerpent*	pInstance = new CAbyssalSerpent(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CAbyssalSerpent");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CAbyssalSerpent::Clone(void * pArg)
{
	CAbyssalSerpent*	pInstance = new CAbyssalSerpent(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CAbyssalSerpent");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAbyssalSerpent::Free()
{
	__super::Free();

	Safe_Release(m_pMaskTextureCom);
	for (auto& pBodyCollider : m_BodyCollider)
		Safe_Release(pBodyCollider);
	for (auto& pWeakCollider : m_WeakCollider)
		Safe_Release(pWeakCollider);

	Safe_Release(m_pModelCom);
}



