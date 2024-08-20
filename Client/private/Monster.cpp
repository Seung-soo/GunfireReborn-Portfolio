#include "stdafx.h"
#include "..\public\Monster.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"
#include "MonsterWave.h"
#include "Damage.h"
#include "Effect.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
	, m_pPlayerTransform(rhs.m_pPlayerTransform)
	, m_tStatus(rhs.m_tStatus)
{
}

void CMonster::Set_Buff(_bool bBuff)
{
	if (true == bBuff && m_bBuff == false)
	{
		m_bBuff = true;
		m_tStatus.fDefend *= 3.f;
	}
	else if (false == bBuff && m_bBuff == true)
	{
		m_bBuff = false;
		m_tStatus.fDefend /= 3.f;
	}
}

HRESULT CMonster::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CMonster::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CMonster::NativeConstruct");
		return E_FAIL;
	}

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonster::NativeConstruct(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonster::NativeConstruct(Shader)");
		return E_FAIL;
	}
	/* For.Com_DissolveTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonster::NativeConstruct(Com_DissolveTexture)");
		return E_FAIL;
	}
	
	m_pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

	m_pMonsterList = CMonsterWave::GetInstance()->Get_MonstersList();
	return S_OK;
}

_int CMonster::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CMonster::Tick");
		return -1;
	}

	Element(dTimeDelta);

	if (true == m_bFreeze)
	{
		if (true == m_bDecay)
			m_fCurrentFreezeTime += (_float)dTimeDelta * 2.f;
		else
			m_fCurrentFreezeTime += (_float)dTimeDelta;
		if (true == m_bIsBoss)
		{
			if (m_fCurrentFreezeTime > m_fFreezeTime * 0.25f)
			{
				m_bFreeze = false;
				m_bChain = false;
				m_fCurrentFreezeTime = 0.f;
			}
		}
		else
		{
			if (m_fCurrentFreezeTime > m_fFreezeTime)
			{
				m_bFreeze = false;
				m_bChain = false;
				m_fCurrentFreezeTime = 0.f;
			}
		}
	}

	if (false == m_bIsBoss)
	{
		if (true == m_bDissolve && false == m_bDead)
		{
			m_iPass = 5;
			m_fDissolveAlpha += (_float)dTimeDelta * 0.65f;
			if (m_fDissolveAlpha >= 1.f)
			{
				m_fDissolveAlpha = 1.f;
				m_bDead = true;
			}

			if (false == m_bDropItems)
			{
				g_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT2);
				g_pGameInstance->LetsPlaySound(TEXT("MonsterKill.mp3"), CSoundMgr::SYSTEM_EFFECT2, 0.3f);
				g_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT3);
				g_pGameInstance->LetsPlaySound(TEXT("MonsterDead.mp3"), CSoundMgr::SYSTEM_EFFECT3, 0.2f);

				CItem_Coin::ITEMCOINDESC tItemCoinDesc;
				tItemCoinDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
				for (_uint i = 0; i < 4; i++)
				{
					tItemCoinDesc.vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f + i * 90.f))) * 2.f;
					tItemCoinDesc.iMoney = rand() % 5 + 3;
					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Coin"), &tItemCoinDesc)))
					{
						__debugbreak();
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonster::Tick(Coin)");
						return DEAD;
					}
					if (LEVEL_STAGE3_1 == g_eCurrentLevel)
						static_cast<CItem_Coin*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
					else if (LEVEL_STAGE4_1 == g_eCurrentLevel)
						static_cast<CItem_Coin*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));
				}

				CItem_Bullet::ITEMBULLETDESC tItemBulletDesc;
				tItemBulletDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
				tItemBulletDesc.eType = CItem_Bullet::TYPE_GREEN;
				tItemBulletDesc.vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f))) * 2.f;
				tItemBulletDesc.iBullet = 30 + rand() % 15;
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Bullet"), &tItemBulletDesc)))
				{
					__debugbreak();
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonster::Tick(Bullet)");
					return DEAD;
				}
				if (LEVEL_STAGE3_1 == g_eCurrentLevel)
					static_cast<CItem_Bullet*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
				else if (LEVEL_STAGE4_1 == g_eCurrentLevel)
					static_cast<CItem_Bullet*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));

				tItemBulletDesc.eType = CItem_Bullet::TYPE_BLUE;
				tItemBulletDesc.vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f))) * 2.f;
				tItemBulletDesc.iBullet = 10 + rand() % 8;
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Bullet"), &tItemBulletDesc)))
				{
					__debugbreak();
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonster::Tick(Bullet)");
					return DEAD;
				}
				if (LEVEL_STAGE3_1 == g_eCurrentLevel)
					static_cast<CItem_Bullet*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
				else if (LEVEL_STAGE4_1 == g_eCurrentLevel)
					static_cast<CItem_Bullet*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));


				tItemBulletDesc.eType = CItem_Bullet::TYPE_YELLOW;
				tItemBulletDesc.vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f))) * 2.f;
				tItemBulletDesc.iBullet = 3 + rand() % 3;
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Bullet"), &tItemBulletDesc)))
				{
					__debugbreak();
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonster::Tick(Bullet)");
					return DEAD;
				}
				if (LEVEL_STAGE3_1 == g_eCurrentLevel)
					static_cast<CItem_Bullet*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
				else if (LEVEL_STAGE4_1 == g_eCurrentLevel)
					static_cast<CItem_Bullet*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));

				CItem_Smoke::ITEMSMOKEDESC tItemSmokeDesc;
				tItemSmokeDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
				tItemSmokeDesc.vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(360.f))) * 2.f;
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Smoke"), &tItemSmokeDesc)))
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonster::Tick(Smoke)");
				if (LEVEL_STAGE3_1 == g_eCurrentLevel)
					static_cast<CItem_Smoke*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
				else if (LEVEL_STAGE4_1 == g_eCurrentLevel)
					static_cast<CItem_Smoke*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));

				if (true == m_bDropWeapon)
				{
					CItem_Weapon::ITEMWEAPONDESC tItemWeaponDesc;
					tItemWeaponDesc.eType = (CItem_Weapon::TYPE)m_iDropWeaponIndex;
					tItemWeaponDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
					tItemWeaponDesc.vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Weapon"), &tItemWeaponDesc)))
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonster::Tick(Weapon)");
					if (LEVEL_STAGE3_1 == g_eCurrentLevel)
						static_cast<CItem_Weapon*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
					else if (LEVEL_STAGE4_1 == g_eCurrentLevel)
						static_cast<CItem_Weapon*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));
				}

				m_bDropItems = true;
			}
		}
		else
			m_iPass = 0;
	}


	return NOPROBLEM;
}

_int CMonster::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CMonster::LateTick");
		return -1;
	}
	_float	fDistance = 0.f;
	_vector	vDir;
	for (auto& pMonster : *m_pMonsterList)
	{
		if (this == pMonster)
			continue;
		vDir = static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		fDistance = DISTANCE(vDir);
		vDir = XMVector4Normalize(vDir);
		if (fDistance < 1.2f)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vDir * (1.2f - fDistance));
		}
	}

	return NOPROBLEM;
}

HRESULT CMonster::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pRendererCom)
	{
		MSGBOX("nullptr == m_pShaderCom or nullptr == m_pRendererCom in CMonster::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CMonster::Render");
		return E_FAIL;
	}

	return S_OK;
}

void CMonster::Freeze(_float fFreezeTime)
{
	m_bFreeze = true;
	m_fFreezeTime = fFreezeTime;
}

void CMonster::Element(_double TimeDelta)
{
	if (true == m_bCombustion)
		Combustion(TimeDelta);
	if (true == m_bManipulation)
		Manipulation(TimeDelta);
	if (true == m_bMiasma)
		Miasma(TimeDelta);
	if (true == m_bBurning)
		Burning(TimeDelta);
	if (true == m_bShock)
		Shock(TimeDelta);
	if (true == m_bDecay)
		Decay(TimeDelta);
}

void CMonster::Burning(_double TimeDelta)
{
	if (true == m_bBurningStart)
	{
		m_tStatus.fAttack = m_tStatus.fAttack * 0.8f;
		m_bBurningStart = false;
		if (true == m_bShock)
			m_bManipulation = true;
		if (true == m_bDecay)
			m_bCombustion = true;


	}

	if (false == m_bIsBoss)
	{
		if (m_fBurningTime > (_float)m_iBurningEffect)
		{
			CEffect::EFFECTDESC tEffectDesc;
			tEffectDesc.eMove = CEffect::MOVE_CHASE;
			tEffectDesc.eType = CEffect::TYPE_SPRITE;
			tEffectDesc.eWay = CEffect::WAY_FINAL;
			tEffectDesc.iPass = 20;
			tEffectDesc.iSpriteStartX = 0;
			tEffectDesc.iSpriteStartY = 0;
			tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
			tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_BurningFire");
			tEffectDesc.vScale = XMVectorSet(0.5f, 0.5f, 1.f, 0.f);

			for (_uint i = 0; i < 2; i++)
			{
				tEffectDesc.fSpriteSpeed = 16.f;
				tEffectDesc.iNumTexIndex = 0;
				tEffectDesc.iSpriteNumX = 4;
				tEffectDesc.iSpriteNumY = 2;
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.01f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.01f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					__debugbreak();
					return;
				}

				tEffectDesc.fSpriteSpeed = 32.f;
				tEffectDesc.iNumTexIndex = 1;
				tEffectDesc.iSpriteNumX = 4;
				tEffectDesc.iSpriteNumY = 4;
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.01f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.01f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					__debugbreak();
					return;
				}

				tEffectDesc.fSpriteSpeed = 18.f;
				tEffectDesc.iNumTexIndex = 2;
				tEffectDesc.iSpriteNumX = 3;
				tEffectDesc.iSpriteNumY = 3;
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.01f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.01f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					__debugbreak();
					return;
				}
			}
			m_iBurningEffect++;
		}
	}

	if (5.f < m_fBurningTime)
	{
		m_tStatus.fAttack = m_tStatus.fAttack * 1.25f;
		m_bBurningStart = true;
		m_bBurning = false;
		m_fBurningTime = 0.f;
	}

	m_fBurningTime += (_float)TimeDelta;
}

void CMonster::Shock(_double TimeDelta)
{
	if (true == m_bShockStart)
	{
		m_tStatus.fDefend = m_tStatus.fDefend * 0.8f;
		m_bShockStart = false;
		if (true == m_bBurning)
			m_bManipulation = true;
		if (true == m_bDecay)
			m_bMiasma = true;
	}


	if (false == m_bIsBoss)
	{
		if (m_fShockTime > (_float)m_iShockEffect)
		{
			CEffect::EFFECTDESC tEffectDesc;
			tEffectDesc.eMove = CEffect::MOVE_CHASE;
			tEffectDesc.eType = CEffect::TYPE_SPRITE;
			tEffectDesc.eWay = CEffect::WAY_FINAL;
			tEffectDesc.fSpriteSpeed = 16.f;
			tEffectDesc.iPass = 20;
			tEffectDesc.iSpriteNumX = 2;
			tEffectDesc.iSpriteNumY = 2;
			tEffectDesc.iSpriteStartX = 0;
			tEffectDesc.iSpriteStartY = 0;
			tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
			tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_ShockLightning");
			tEffectDesc.vScale = XMVectorSet(0.5f, 0.5f, 1.f, 0.f);

			for (_uint i = 0; i < 2; i++)
			{
				tEffectDesc.iNumTexIndex = 0;
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.01f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.01f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					__debugbreak();
					return;
				}

				tEffectDesc.iNumTexIndex = 1;
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.01f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.01f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					__debugbreak();
					return;
				}

				tEffectDesc.iNumTexIndex = 2;
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.01f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.01f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					__debugbreak();
					return;
				}

				tEffectDesc.iNumTexIndex = 3;
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.01f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.01f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					__debugbreak();
					return;
				}
			}

			m_iShockEffect++;
		}
	}

	if (5.f < m_fShockTime)
	{
		m_tStatus.fDefend = m_tStatus.fDefend * 1.25f;
		m_bShockStart = true;
		m_bShock = false;
		m_fShockTime = 0.f;
	}

	m_fShockTime += (_float)TimeDelta;
}

void CMonster::Decay(_double TimeDelta)
{
	if (true == m_bDecayStart)
	{
		m_fSpeed = 0.5f;
		m_bDecayStart = false;
		if (true == m_bBurning)
			m_bCombustion = true;
		if (true == m_bShock)
			m_bMiasma = true;
	}


	if (false == m_bIsBoss)
	{
		if (m_fDecayTime * 3.f > (_float)m_iDecayEffect)
		{
			CEffect::EFFECTDESC tEffectDesc;
			tEffectDesc.eMove = CEffect::MOVE_STAND;
			tEffectDesc.eType = CEffect::TYPE_2D;
			tEffectDesc.eWay = CEffect::WAY_SCALEDOWN;
			tEffectDesc.fDownAlphaSpeed = 1.f;
			tEffectDesc.iPass = 24;
			tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.01f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.01f);
			tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
			tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_DecayEffect");
			tEffectDesc.vScale = XMVectorSet(0.5f, 0.5f, 1.f, 0.f);
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				__debugbreak();
				return;
			}

			tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.01f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.01f);
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				__debugbreak();
				return;
			}
			m_iDecayEffect++;
		}
	}

	if (5.f < m_fDecayTime)
	{
		m_fSpeed = 1.f;
		m_bDecayStart = true;
		m_bDecay = false;
		m_fDecayTime = 0.f;
	}

	m_fDecayTime += (_float)TimeDelta;
}

void CMonster::Combustion(_double TimeDelta)
{
	if (true == m_bCombustionStart)
	{
		if (0.f < m_tStatus.fCurrentShield)
		{
			_float fDamage = m_tStatus.fMaxHp * 0.3f - m_tStatus.fCurrentShield;
			if (0.f < fDamage)
			{
				m_tStatus.fCurrentHp -= fDamage;
				m_tStatus.fCurrentShield = 0.f;
			}
			else
				m_tStatus.fCurrentShield -= m_tStatus.fMaxHp * 0.3f;
		}
		else
		{
			if (false == m_bIsBoss)
				m_tStatus.fCurrentHp -= m_tStatus.fMaxHp * 0.3f;
			else
				m_tStatus.fCurrentHp -= m_tStatus.fMaxHp * 0.03f;
		}

		if (0.f >= m_tStatus.fCurrentHp)
			m_bDead = true;


		if (false == m_bIsBoss)
		{
			// Æø¹ß ÀÌÆåÆ® »ý¼º
			CEffect::EFFECTDESC tEffectDesc;
			tEffectDesc.eMove = CEffect::MOVE_STAND;
			tEffectDesc.eType = CEffect::TYPE_SPRITE;
			tEffectDesc.eWay = CEffect::WAY_FINAL;
			tEffectDesc.fSpriteSpeed = 48.f;
			tEffectDesc.iPass = 20;
			tEffectDesc.iSpriteNumX = 4;
			tEffectDesc.iSpriteNumY = 4;
			tEffectDesc.iSpriteStartX = 0;
			tEffectDesc.iSpriteStartY = 0;
			tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
			tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_TigerCannon_Explosion");
			tEffectDesc.vScale = XMVectorSet(8.f, 8.f, 8.f, 0.f);
			tEffectDesc.iNumTexIndex = 0;
			tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
			tEffectDesc.pDead = nullptr;
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				__debugbreak();
				return;
			}
		}



		CDamage::NUMBERDESC tNumberDesc;
		tNumberDesc.eType = CDamage::TYPE_COMB;
		if (false == m_bIsBoss)
			tNumberDesc.iDamage = _int(m_tStatus.fMaxHp * 0.3f);
		else
			tNumberDesc.iDamage = _int(m_tStatus.fMaxHp * 0.03f);

		tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f) + m_pPlayerTransform->Get_State(CTransform::STATE_LOOK) * -1.5f;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonster::Combustion");
			return;
		}

		auto MonsterList = CMonsterWave::GetInstance()->Get_MonstersList();
		for (auto pMonster : *MonsterList)
		{
			_float fDistance = DISTANCE(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION));
			if (2.f > fDistance)
				pMonster->Set_Combustion(true);
		}
		m_bCombustionStart = false;
	}

	if (5.f < m_fCombustionTime)
	{
		m_bCombustionStart = true;
		m_bCombustion = false;
		m_fCombustionTime = 0.f;
	}

	m_fCombustionTime += (_float)TimeDelta;
}

void CMonster::Manipulation(_double TimeDelta)
{
	if (true == m_bIsBoss)
		return;

	if (true == m_bManipulationStart)
	{
		Safe_Release(m_pTargetMonster);
		Safe_Release(m_pPlayerTransform);
		_float fMinDist = 3000.f;
		auto MonsterList = CMonsterWave::GetInstance()->Get_MonstersList();
		for (auto pMonster : *MonsterList)
		{
			if (this == pMonster)
				continue;

			_float fDistance = DISTANCE(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION));
			if (fMinDist > fDistance)
			{
				m_pPlayerTransform = static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")));
				fMinDist = fDistance;
				m_pTargetMonster = pMonster;
			}
		}
		if (nullptr != m_pPlayerTransform)
			Safe_AddRef(m_pPlayerTransform);
		if (nullptr != m_pTargetMonster)
			Safe_AddRef(m_pTargetMonster);


		if (false == m_bIsBoss)
		{
			CEffect::EFFECTDESC tEffectDesc;
			tEffectDesc.eMove = CEffect::MOVE_CHASE;
			tEffectDesc.eType = CEffect::TYPE_MULTITEX;
			tEffectDesc.eWay = CEffect::WAY_ROOFTIME;
			tEffectDesc.fRoofTime = 5.f;
			tEffectDesc.fSpriteSpeed = 12.f;
			tEffectDesc.iPass = 24;
			tEffectDesc.iNumTexIndex = 6;
			tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 2.3f, 0.f);
			tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
			tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_Mani");
			tEffectDesc.pDead = &m_bDead;
			tEffectDesc.vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				__debugbreak();
				return;
			}
		}

		m_bManipulationStart = false;
	}

	if (true == m_pTargetMonster->Get_Dead())
	{
		Safe_Release(m_pPlayerTransform);
		Safe_Release(m_pTargetMonster);
		_float fMinDist = 3000.f;
		auto MonsterList = CMonsterWave::GetInstance()->Get_MonstersList();
		for (auto pMonster : *MonsterList)
		{
			if (true == pMonster->Get_Dead() || this == pMonster)
				continue;
			_float fDistance = DISTANCE(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION));
			if (fMinDist > fDistance)
			{
				m_pPlayerTransform = static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")));
				m_pTargetMonster = pMonster;
				fMinDist = fDistance;
			}
		}
		if (nullptr != m_pPlayerTransform)
			Safe_AddRef(m_pPlayerTransform);
		if (nullptr != m_pTargetMonster)
			Safe_AddRef(m_pTargetMonster);
	}

	if (5.f < m_fManipulationTime)
	{
		m_bManipulationStart = true;
		m_bManipulation = false;
		m_fManipulationTime = 0.f;

		Safe_Release(m_pPlayerTransform);
		m_pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		Safe_AddRef(m_pPlayerTransform);
	}

	m_fManipulationTime += (_float)TimeDelta;
}

void CMonster::Miasma(_double TimeDelta)
{
	if (1.f <= m_fDotDealTime)
	{
		if (0.f < m_tStatus.fCurrentShield)
		{
			_float fDamage = m_tStatus.fMaxHp * 0.1f - m_tStatus.fCurrentShield;
			if (0.f < fDamage)
			{
				m_tStatus.fCurrentHp -= fDamage;
				m_tStatus.fCurrentShield = 0.f;
			}
			else
				m_tStatus.fCurrentShield -= m_tStatus.fMaxHp * 0.1f;
		}
		else
		{
			if (false == m_bIsBoss)
				m_tStatus.fCurrentHp -= m_tStatus.fMaxHp * 0.1f;
			else
				m_tStatus.fCurrentHp -= m_tStatus.fMaxHp * 0.01f;

		}

		if (0.f >= m_tStatus.fCurrentHp)
			m_bDead = true;

		m_fDotDealTime = 0.f;

		CDamage::NUMBERDESC tNumberDesc;
		tNumberDesc.eType = CDamage::TYPE_MIAS;
		if (false == m_bIsBoss)
			tNumberDesc.iDamage = _int(m_tStatus.fMaxHp * 0.1f);
		else
			tNumberDesc.iDamage = _int(m_tStatus.fMaxHp * 0.01f);

		tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f) + m_pPlayerTransform->Get_State(CTransform::STATE_LOOK) * -1.5f;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonster::Miasma");
			return;
		}

		if (false == m_bIsBoss)
		{
			CEffect::EFFECTDESC tEffectDesc;
			tEffectDesc.eMove = CEffect::MOVE_CHASE;
			tEffectDesc.eType = CEffect::TYPE_2D;
			tEffectDesc.eWay = CEffect::WAY_SCALEUP;
			tEffectDesc.fRoofTime = 2.5f;
			tEffectDesc.fDownAlphaSpeed = 15.f;
			tEffectDesc.iPass = 24;
			tEffectDesc.iNumTexIndex = 0;
			tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.002f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.002f);
			tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
			tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_MiasmaPoison");
			tEffectDesc.pDead = &m_bDead;
			tEffectDesc.vScale = XMVectorSet(0.2f, 0.2f, 0.2f, 0.f);
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				__debugbreak();
				return;
			}

			tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) * 0.01f, _float(rand() % 101) * 0.02f, _float(rand() % 101 - 50) * 0.01f);
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				__debugbreak();
				return;
			}
		}
		
	}

	if (true == m_bMiasmaStart)
		m_bMiasmaStart = false;
	
	if (5.f < m_fMiasmaTime)
	{
		m_bMiasmaStart = true;
		m_bMiasma = false;
		m_fMiasmaTime = 0.f;
	}

	m_fDotDealTime += (_float)TimeDelta;
	m_fMiasmaTime += (_float)TimeDelta;
}

HRESULT CMonster::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CPlayer::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CSky::SetUp_ConstantTable");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}


	if (FAILED(m_pDissolveTextureCom->SetUp_OnShader(m_pShaderCom, "g_ThirdTexture", 90)))
	{
		MSGBOX("m_pDissolveTextureCom->SetUp_OnShader returned E_FAIL in CPlayer::SetUp_ConstantTable(g_ThirdTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAlpha", &m_fDissolveAlpha, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer::SetUp_ConstantTable(g_fAlpha)");
		return E_FAIL;
	}


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CMonster::SetUp_OnTerrain()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CMonster::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CMonster::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	// ·ÎÄÃ Æ÷Áö¼Ç ±¸ÇÏ±â
	_matrix	WorldMatrixInverse = pTerrainTransform->Get_WorldMatrixInverse();

	_vector	vLocalPos = XMVector3TransformCoord(vPosition, WorldMatrixInverse);
	_float3 vLocalPosFloat3;
	XMStoreFloat3(&vLocalPosFloat3, vLocalPos);

	vPosition = XMVector3TransformCoord(vLocalPos, pTerrainTransform->Get_WorldMatrix());

	////////////////////////// Á¡ÇÁ ¸ØÃç!!!!!!!!////////////////////////
	_float fTerrainY = pVIBuffer_Terrain->Compute_Y(vLocalPosFloat3);
	_float fMonsterHalfScaleY = XMVectorGetY(m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	//if (fTerrainY > vLocalPosFloat3.y - fPlayerHalfScaleY)
	//{
	//	m_bJump = false;
	//	m_fJumpTime = 0.f;
	//}
	/////////////////////////////////////////////////////////////////////

	// ´ëÀÔ
	//if (false == m_bJump)
	//{
		vPosition = XMVectorSetY(vPosition, fTerrainY/* + fMonsterHalfScaleY*/);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	//}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMonster::Element_Reset()
{
	m_bBuff = false;

	m_bBurning = false;
	m_bShock = false;
	m_bDecay = false;
	m_bCombustion = false;
	m_bManipulation = false;
	m_bMiasma = false;

	m_fBurningTime = 0.f;
	m_fShockTime = 0.f;
	m_fDecayTime = 0.f;
	m_fCombustionTime = 0.f;
	m_fManipulationTime = 0.f;
	m_fMiasmaTime = 0.f;

	m_bBurningStart = true;
	m_bShockStart = true;
	m_bDecayStart = true;
	m_bCombustionStart = true;
	m_bManipulationStart = true;
	m_bMiasmaStart = true;

	m_fDotDealTime = 1.f;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pTargetMonster);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
