#include "stdafx.h"
#include "..\public\Player_Hand.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Camera_Dynamic.h"
#include "HpBar.h"
#include "HpNumber.h"
#include "PoisonSmoke.h"
#include "BackGround.h"
#include "Bullet_Tesla.h"
#include "Bullet_TigerCannon.h"
#include "Bullet_ThunderClapGloves.h"
#include "Bullet_FireDragon.h"
#include "Monster.h"
#include "PlayerHit.h"
#include "NumberUI.h"
#include "Damage.h"
#include "CoolTime.h"
#include "Effect.h"
#include "SwordTrail.h"
#include "Particle_Effect.h"
#include "PoleMonarch.h"

CPlayer_Hand::CPlayer_Hand(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CPlayer_Hand::CPlayer_Hand(const CPlayer_Hand & rhs)
	: CGameObject(rhs)
{
}

void CPlayer_Hand::Set_Weapon(WEAPON eWeapon, _int iBullet)
{
	CItem_Weapon::ITEMWEAPONDESC tItemWeaponDesc;
	tItemWeaponDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	tItemWeaponDesc.vTargetPos = tItemWeaponDesc.vPosition + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 2;
	_int iCurrentBullet = 0;
	if (((1 == m_iCurrentWeaponIndex || 3 == m_iCurrentWeaponIndex) && (WEAPON_END != m_eSlot1 && WEAPON_END != m_eSlot2)))
	{
		switch (m_eSlot1)
		{
		case WEAPON_TESLA:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1211;
			iCurrentBullet = m_iCurrentBlueBullet;
			break;
		case WEAPON_TIGERCANNON:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1401;
			iCurrentBullet = m_iCurrentYellowBullet;
			break;
		case WEAPON_THUNDERCLAPGLOVES:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1410;
			iCurrentBullet = m_iCurrentGreenBullet;
			break;
		case WEAPON_FIREDRAGON:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1412;
			iCurrentBullet = m_iCurrentGreenBullet;
			break;
		case WEAPON_PIERCINGFRAME:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1501;
			iCurrentBullet = m_iCurrentYellowBullet;
			break;
		case WEAPON_SWORD:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1603;
			iCurrentBullet = m_iCurrentBlueBullet;
			break;
		}
		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
		_uint	iLevel = pGameInstance->Get_LevelIndex();
		if (FAILED(pGameInstance->Add_GameObject(iLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Weapon"), &tItemWeaponDesc)))
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Set_Weapon(Weapon)");
		if (5 > iLevel)
			static_cast<CItem_Weapon*>(pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
		else
			static_cast<CItem_Weapon*>(pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));
		static_cast<CItem_Weapon*>(pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Item")))->Set_Bullet(iCurrentBullet);
		RELEASE_INSTANCE(CGameInstance);

	}
	else if(2 == m_iCurrentWeaponIndex && (WEAPON_END != m_eSlot1 && WEAPON_END != m_eSlot2))
	{
		switch (m_eSlot2)
		{
		case WEAPON_TESLA:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1211;
			iCurrentBullet = m_iCurrentBlueBullet;
			break;
		case WEAPON_TIGERCANNON:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1401;
			iCurrentBullet = m_iCurrentYellowBullet;
			break;
		case WEAPON_THUNDERCLAPGLOVES:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1410;
			iCurrentBullet = m_iCurrentGreenBullet;
			break;
		case WEAPON_FIREDRAGON:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1412;
			iCurrentBullet = m_iCurrentGreenBullet;
			break;
		case WEAPON_PIERCINGFRAME:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1501;
			iCurrentBullet = m_iCurrentYellowBullet;
			break;
		case WEAPON_SWORD:
			tItemWeaponDesc.eType = CItem_Weapon::TYPE_1603;
			iCurrentBullet = m_iCurrentBlueBullet;
			break;
		}
		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
		_uint	iLevel = pGameInstance->Get_LevelIndex();
		if (FAILED(pGameInstance->Add_GameObject(iLevel, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Weapon"), &tItemWeaponDesc)))
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Set_Weapon(Weapon)");
		if (5 > iLevel)
			static_cast<CItem_Weapon*>(pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
		else
			static_cast<CItem_Weapon*>(pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Item")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));

		static_cast<CItem_Weapon*>(pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Item")))->Set_Bullet(iCurrentBullet);
		RELEASE_INSTANCE(CGameInstance);
	}


	if (WEAPON_TESLA == eWeapon)
	{
		srand(22);
		m_eNextWeapon = WEAPON_TESLA;
		m_bSwapWeapon = true;
		m_iCurrentBlueBullet = iBullet;
	}
	else if (WEAPON_TIGERCANNON == eWeapon)
	{
		srand(33);
		m_eNextWeapon = WEAPON_TIGERCANNON;
		m_bSwapWeapon = true;
		m_iCurrentYellowBullet = iBullet;
	}
	else if (WEAPON_THUNDERCLAPGLOVES == eWeapon)
	{
		srand(44);
		m_eNextWeapon = WEAPON_THUNDERCLAPGLOVES;
		m_bSwapWeapon = true;
		m_iCurrentGreenBullet = iBullet;
	}
	else if (WEAPON_FIREDRAGON == eWeapon)
	{
		srand(55);
		m_eNextWeapon = WEAPON_FIREDRAGON;
		m_bSwapWeapon = true;
		m_iCurrentGreenBullet = iBullet;
	}
	else if (WEAPON_PIERCINGFRAME == eWeapon)
	{
		srand(66);
		m_eNextWeapon = WEAPON_PIERCINGFRAME;
		m_bSwapWeapon = true;
		m_iCurrentYellowBullet = iBullet;
	}
	else if (WEAPON_SWORD == eWeapon)
	{
		srand(77);
		m_eNextWeapon = WEAPON_SWORD;
		m_bSwapWeapon = true;
		m_iCurrentBlueBullet = iBullet;
	}
}

HRESULT CPlayer_Hand::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CPlayer_Hand::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Hand::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CPlayer_Hand::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CPlayer_Hand::NativeConstruct");
		return E_FAIL;
	}
	m_eCurrentWeapon = WEAPON_ANGELICAURA;
	Weapon_Info();

	m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);
	m_pModelArmCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);

	m_pPlayerState = static_cast<CPlayer*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player")))->Get_PlayerState();
	m_pPlayerJump = static_cast<CPlayer*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player")))->Get_Jump();
	m_pPlayerJumpTime = static_cast<CPlayer*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player")))->Get_JumpTime();

	m_pCameraTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
	m_pCamera = static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")));

	if (FAILED(Ready_Layer_UI_Bar(TEXT("Layer_UI_Bar"))))
	{
		MSGBOX("Ready_Layer_UI_Bar returned E_FAIL in CPlayer_Hand::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI_SmokeNumber()))
	{
		MSGBOX("Ready_Layer_UI_SmokeNumber returned E_FAIL in CPlayer_Hand::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI_MoneyNumber()))
	{
		MSGBOX("Ready_Layer_UI_MoneyNumber returned E_FAIL in CPlayer_Hand::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI_ShieldNumber()))
	{
		MSGBOX("Ready_Layer_UI_ShieldNumber returned E_FAIL in CPlayer_Hand::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI_HpNumber()))
	{
		MSGBOX("Ready_Layer_UI_HpNumber returned E_FAIL in CPlayer_Hand::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI_Bullet(TEXT("Layer_UI_Bullet"))))
	{
		MSGBOX("Ready_Layer_UI_Bullet returned E_FAIL in CPlayer_Hand::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI_BulletNumber()))
	{
		MSGBOX("Ready_Layer_UI_BulletNumber returned E_FAIL in CPlayer_Hand::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI_Weapon()))
	{
		MSGBOX("Ready_Layer_UI_Weapon returned E_FAIL in CPlayer_Hand::NativeConstruct");
		return E_FAIL;
	}



	return S_OK;
}

_int CPlayer_Hand::Tick(_double dTimeDelta)
{
	if (true == g_bCameraAction)
		return NOPROBLEM;

	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CPlayer_Hand::Tick");
		return -1;
	}

	if (FAILED(g_pGameInstance->Add_Collider(TEXT("Body"), CCollision_Manager::TARGET_PLAYER, m_pColliderCom)))
	{
		MSGBOX("g_pGameInstance->Add_Collider returned E_FAIL in CPlayer_Hand::Tick");
		return -1;
	}

	if (true == m_bSlow)
	{
		m_dSlowTime += dTimeDelta;
		if (4.0 < m_dSlowTime)
		{
			static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_Speed(10.f);
			m_dSlowTime = 0.0;
			m_bSlow = false;
		}
	}

	if (true == m_bOil && true == m_bTalents[1])
	{
		m_fOilTime += (_float)dTimeDelta;
		if (6.f < m_fOilTime)
		{
			m_fOilTime = 0.f;
			m_bOil = false;
		}
	}

	if (true == m_bPlusShield && 6.f > m_fPlusShieldTime)
	{
		m_fPlusShieldTime += (_float)dTimeDelta;
		if (6.f <= m_fPlusShieldTime)
		{
			m_bPlusShield = false;
			m_iMaxShield -= 60;
			if (m_iCurrentShield > m_iMaxShield)
				m_iCurrentShield = m_iMaxShield;
		}
	}

	if (true == m_bTalents[4] && false == m_bPowerEnergyOrb)
	{
		m_bPowerEnergyOrb = true;
		m_fSkillMaxCoolTime = 10;
	}

	// 쉴드 회복
	HealingShield(dTimeDelta);

	return _int();
}

_int CPlayer_Hand::LateTick(_double dTimeDelta)
{
	if (true == g_bCameraAction)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		return NOPROBLEM;
	}

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CPlayer_Hand::LateTick");
		return -1;
	}

	if (g_pGameInstance->Get_DIKeyState(DIK_9) & 0x80 && 0 < m_iCurrentHp)
		m_iCurrentHp--;

	if (g_pGameInstance->Get_DIKeyState(DIK_0) & 0x80 && m_iMaxHp > m_iCurrentHp)
		m_iCurrentHp++;

	if (g_pGameInstance->Get_DIKeyState(DIK_7) & 0x80 && 0 < m_iCurrentShield)
		m_iCurrentShield--;

	if (g_pGameInstance->Get_DIKeyState(DIK_8) & 0x80 && m_iMaxShield > m_iCurrentShield)
		m_iCurrentShield++;

	m_pUIBar[0]->Set_Hp(m_iMaxHp, m_iCurrentHp);
	m_pUIBar[1]->Set_Hp(m_iMaxShield, m_iCurrentShield);

	_float4x4 vCamWorldMatrix;
	XMStoreFloat4x4(&vCamWorldMatrix, XMMatrixInverse(nullptr, g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)));
	m_pTransformCom->Set_WorldMatrix(vCamWorldMatrix);

	//m_pCamera->Render();

	// 숨쉬기
	Breath(dTimeDelta);

	// 무기 키 입력
	if (false == m_bSwapWeapon)
		WeaponKey(dTimeDelta);

	// 무기 교체
	SwapWeapon(dTimeDelta);

	if (m_eCurrentWeapon != WEAPON_SKILL)
	{
		if (true == m_bOil && true == m_bTalents[1])
			m_pModelCom[m_eCurrentWeapon]->Update_CombinedTransformationMatrices(m_iAnimationType, dTimeDelta * 1.6f);
		else
			m_pModelCom[m_eCurrentWeapon]->Update_CombinedTransformationMatrices(m_iAnimationType, dTimeDelta);

		vector<CHierarchyNode*>* pHiererchyNodesGun = m_pModelCom[m_eCurrentWeapon]->Get_Nodes();

		for (size_t i = 0; i < pHiererchyNodesGun->size(); i++)
		{
			CHierarchyNode* pGunNode = m_pModelArmCom[m_eCurrentWeapon]->Find_HierarchyNode((*pHiererchyNodesGun)[i]->Get_Name());
			if (nullptr != pGunNode)
			{
				pGunNode->Set_CombineTransformationMatrix((*pHiererchyNodesGun)[i]->Get_CombinedTransformationMatrix());
			}
		}
	}
	else
	{
		m_iAnimationType = ANIM_ONETIME;
		m_pModelArmCom[m_eCurrentWeapon]->Update_CombinedTransformationMatrices(m_iAnimationType, dTimeDelta);
	}


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CCollider*>* pWeakColliderList = pGameInstance->Get_ColliderList(TEXT("Weakness"), CCollision_Manager::TARGET_ENEMY);
	list<CCollider*>* pBodyColliderList = pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
	if (nullptr != pWeakColliderList && nullptr != pBodyColliderList && LEVEL_LOADING != g_eCurrentLevel)
	{
		_float Dist;
		_bool bColl = false;
		for (auto& pWeakCollider : *pWeakColliderList)
			if (pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pWeakCollider, &Dist))
			{
				m_fBulletDist = Dist;
				bColl = true;
				static_cast<CMonster*>(pWeakCollider->Get_Parent())->Set_UIRender(true);
			}


		for (auto& pBodyCollider : *pBodyColliderList)
			if (pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pBodyCollider, &Dist))
			{
				m_fBulletDist = Dist;
				bColl = true;
				static_cast<CMonster*>(pBodyCollider->Get_Parent())->Set_UIRender(true);
			}

		if (false == bColl)
			m_fBulletDist = 30.f;
	}
	else
		m_fBulletDist = 30.f;
	RELEASE_INSTANCE(CGameInstance);

	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return _int();
}




HRESULT CPlayer_Hand::Render()
{
	if ((m_eCurrentWeapon != WEAPON_SKILL && nullptr == m_pModelCom[m_eCurrentWeapon]) ||
		nullptr == m_pModelArmCom[m_eCurrentWeapon])
	{
		MSGBOX("nullptr == m_pVIBufferCom in CPlayer_Hand::Render");
		return E_FAIL;
	}
	
	if (true == m_bZoom || true == g_bCameraAction)
		return S_OK;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CPlayer_Hand::Render");
		return E_FAIL;
	}

	if (m_eCurrentWeapon != WEAPON_SKILL)
	{
		_uint iNumMaterials = m_pModelCom[m_eCurrentWeapon]->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; i++)
		{
			m_pModelCom[m_eCurrentWeapon]->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom[m_eCurrentWeapon]->Render(m_pShaderCom, 6, i, "g_BoneMatrices", true);
		}
	}

	_uint iNumMaterialss = m_pModelArmCom[m_eCurrentWeapon]->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterialss; i++)
	{
		m_pModelArmCom[m_eCurrentWeapon]->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelArmCom[m_eCurrentWeapon]->Render(m_pShaderCom, 0, i, "g_BoneMatrices");
	}
#ifdef _DEBUG
	if (true == m_bSwordCollision)
		for (_uint i = 0; i < 6; i++)
		{
			m_pSwordColliderCom[i]->Render();
		}
	//m_pColliderCom->Render();
#endif
	return S_OK;
}

void CPlayer_Hand::Hit(_float fAttack)
{
	g_pGameInstance->StopSound(CSoundMgr::EFFECT6);
	g_pGameInstance->LetsPlaySound(TEXT("hit1.mp3"), CSoundMgr::EFFECT6, 0.5f);
	g_pGameInstance->StopSound(CSoundMgr::EFFECT7);
	g_pGameInstance->LetsPlaySound(TEXT("hit2.mp3"), CSoundMgr::EFFECT7, 0.5f);
	m_pCamera->Set_HorizonShake(0.2f, 1.f);

	_float fDamage = fAttack / 4.f;
	if (m_iCurrentShield > 0)
	{
		if (m_iCurrentShield < (_int)fDamage)
		{
			fDamage -= (_float)m_iCurrentShield;

			m_iCurrentShield = 0;
			m_iCurrentHp -= (_int)fDamage;
		}
		else
			m_iCurrentShield -= (_int)fDamage;
	}
	else
		m_iCurrentHp -= (_int)fDamage;

	if (0 > m_iCurrentHp)
		m_iCurrentHp = 0;

	// 체력바 배경
	INFO_UI tInfoUI;
	ZeroMemory(&tInfoUI, sizeof(INFO_UI));

	tInfoUI.fScaleX = 1344.f;
	tInfoUI.fScaleY = 756.f;
	tInfoUI.fPositionX = 640.f;
	tInfoUI.fPositionY = 360.f;
	tInfoUI.fDepth = 5.f;

	if (nullptr == g_pGameInstance->Find_Layer(g_eCurrentLevel, TEXT("Layer_PlayerHit")) || 0 == g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_PlayerHit")))
	{
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_PlayerHit"), TEXT("Prototype_GameObject_PlayerHit"), &tInfoUI)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Hit");
			return;
		}
	}
	else
		static_cast<CPlayerHit*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_PlayerHit")))->Set_Alpha();

	m_fHitTime = 0.f;
}

void CPlayer_Hand::HitAndSlow(_float fAttack, _float fSlow)
{
	g_pGameInstance->StopSound(CSoundMgr::EFFECT6);
	g_pGameInstance->LetsPlaySound(TEXT("hit1.mp3"), CSoundMgr::EFFECT6, 0.5f);
	g_pGameInstance->StopSound(CSoundMgr::EFFECT7);
	g_pGameInstance->LetsPlaySound(TEXT("hit2.mp3"), CSoundMgr::EFFECT7, 0.5f);
	m_pCamera->Set_HorizonShake(0.2f, 1.f);

	_float fDamage = fAttack / 2.f;
	if (m_iCurrentShield > 0)
	{
		if (m_iCurrentShield < (_int)fDamage)
		{
			fDamage -= (_float)m_iCurrentShield;

			m_iCurrentShield = 0;
			m_iCurrentHp -= (_int)fDamage;
		}
		else
			m_iCurrentShield -= (_int)fDamage;
	}
	else
		m_iCurrentHp -= (_int)fDamage;

	if (0 > m_iCurrentHp)
		m_iCurrentHp = 0;

	// 체력바 배경
	INFO_UI tInfoUI;
	ZeroMemory(&tInfoUI, sizeof(INFO_UI));

	tInfoUI.fScaleX = 1344.f;
	tInfoUI.fScaleY = 756.f;
	tInfoUI.fPositionX = 640.f;
	tInfoUI.fPositionY = 360.f;
	tInfoUI.fDepth = 5.f;

	if (nullptr == g_pGameInstance->Find_Layer(g_eCurrentLevel, TEXT("Layer_PlayerHit")) || 0 == g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_PlayerHit")))
	{
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_PlayerHit"), TEXT("Prototype_GameObject_PlayerHit"), &tInfoUI)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Hit");
			return;
		}
	}
	else
		static_cast<CPlayerHit*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_PlayerHit")))->Set_Alpha();

	m_bSlow = true;
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_Speed(6.5f * fSlow);

	m_fHitTime = 0.f;
}

void CPlayer_Hand::HealHp()
{
	m_iCurrentHp += _int(m_iMaxHp * 0.3f);
	if (m_iMaxHp < m_iCurrentHp)
		m_iCurrentHp = m_iMaxHp;
}

void CPlayer_Hand::FullBullet()
{
	m_iMaxBlueBullet = m_iTopMaxBlueBullet;
	m_iMaxGreenBullet = m_iTopMaxGreenBullet;
	m_iMaxYellowBullet = m_iTopMaxYellowBullet;
}

void CPlayer_Hand::FullSmoke()
{
	m_iCurrentSmoke = m_iMaxSmoke;
}

HRESULT CPlayer_Hand::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model_AngelicAura */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_AngelicAura"), TEXT("Com_Model_AngelicAura"), (CComponent**)&m_pModelCom[WEAPON_ANGELICAURA])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_AngelicAura)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_AngelicAura"), TEXT("Com_Model_Player_AngelicAura"), (CComponent**)&m_pModelArmCom[WEAPON_ANGELICAURA])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_Player_AngelicAura)");
		return E_FAIL;
	}

	/* For.Com_Model_Tesla */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tesla"), TEXT("Com_Model_Tesla"), (CComponent**)&m_pModelCom[WEAPON_TESLA])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_Tesla)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Tesla"), TEXT("Com_Model_Player_Tesla"), (CComponent**)&m_pModelArmCom[WEAPON_TESLA])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_Player_Tesla)");
		return E_FAIL;
	}

	/* For.Com_Model_TigerCannon*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_TigerCannon"), TEXT("Com_Model_TigerCannon"), (CComponent**)&m_pModelCom[WEAPON_TIGERCANNON])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_TigerCannon)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_TigerCannon"), TEXT("Com_Model_Player_TigerCannon"), (CComponent**)&m_pModelArmCom[WEAPON_TIGERCANNON])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_Player_TigerCannon)");
		return E_FAIL;
	}

	/* For.Com_Model_ThunderClapGloves*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ThunderClapGloves"), TEXT("Com_Model_ThunderClapGloves"), (CComponent**)&m_pModelCom[WEAPON_THUNDERCLAPGLOVES])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_ThunderClapGloves)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_ThunderClapGloves"), TEXT("Com_Model_Player_ThunderClapGloves"), (CComponent**)&m_pModelArmCom[WEAPON_THUNDERCLAPGLOVES])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_Player_ThunderClapGloves)");
		return E_FAIL;
	}

	/* For.Com_Model_FireDragon*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireDragon"), TEXT("Com_Model_FireDragon"), (CComponent**)&m_pModelCom[WEAPON_FIREDRAGON])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_FireDragon)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_FireDragon"), TEXT("Com_Model_Player_FireDragon"), (CComponent**)&m_pModelArmCom[WEAPON_FIREDRAGON])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_Player_FireDragon)");
		return E_FAIL;
	}

	/* For.Com_Model_PiercingFrame*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_PiercingFrame"), TEXT("Com_Model_PiercingFrame"), (CComponent**)&m_pModelCom[WEAPON_PIERCINGFRAME])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_PiercingFrame)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_PiercingFrame"), TEXT("Com_Model_Player_PiercingFrame"), (CComponent**)&m_pModelArmCom[WEAPON_PIERCINGFRAME])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_Player_PiercingFrame)");
		return E_FAIL;
	}

	/* For.Com_Model_Sword*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sword"), TEXT("Com_Model_Sword"), (CComponent**)&m_pModelCom[WEAPON_SWORD])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_Sword)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Sword"), TEXT("Com_Model_Player_Sword"), (CComponent**)&m_pModelArmCom[WEAPON_SWORD])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_Player_Sword)");
		return E_FAIL;
	}

	/* For.Com_Model_Skill*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Skill"), TEXT("Com_Model_Player_Skill"), (CComponent**)&m_pModelArmCom[WEAPON_SKILL])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Model_Player_Skill)");
		return E_FAIL;
	}

	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.5f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider0"), (CComponent**)&m_pSwordColliderCom[0], &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Collider)");
		return E_FAIL;
	}

	ColliderDesc.vTranslation = _float4(0.f, 0.f, 1.5f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider1"), (CComponent**)&m_pSwordColliderCom[1], &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Collider)");
		return E_FAIL;
	}

	ColliderDesc.vTranslation = _float4(0.f, 0.f, 2.5f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider2"), (CComponent**)&m_pSwordColliderCom[2], &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Collider)");
		return E_FAIL;
	}

	ColliderDesc.vTranslation = _float4(0.f, 0.f, 3.5f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider3"), (CComponent**)&m_pSwordColliderCom[3], &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Collider)");
		return E_FAIL;
	}

	ColliderDesc.vTranslation = _float4(0.f, 0.f, 4.5f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider4"), (CComponent**)&m_pSwordColliderCom[4], &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Collider)");
		return E_FAIL;
	}

	ColliderDesc.vTranslation = _float4(0.f, 0.f, 5.5f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider5"), (CComponent**)&m_pSwordColliderCom[5], &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Collider)");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer_Hand::SetUp_Components(Collider_Body)");
		return E_FAIL;
	}
	m_pColliderCom->Set_Parent(this);

	return S_OK;
}

HRESULT CPlayer_Hand::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CPlayer_Hand::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CPlayer_Hand::SetUp_ConstantTable");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer_Hand::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer_Hand::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in CPlayer_Hand::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer_Hand::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer_Hand::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer_Hand::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer_Hand::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer_Hand::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CPlayer_Hand::Breath(_double TimeDelta)
{
	if (true == *m_pPlayerJump)				// 점프 상태일 때 회전
	{
		if (1.8f < m_fBreathCycle)					// 점점 숨이 돌아옴
			m_fBreathCycle -= 0.4f;

		if (TimeDelta > TimeDelta * (*m_pPlayerJumpTime) * 2.5f)
		{
			m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fJumpBreathDegree));
			if (m_fJumpBreathDegree < 10.f)
				m_fJumpBreathDegree += 0.1f;
		}
		else
		{
			m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fJumpBreathDegree));
			if (m_fJumpBreathDegree > -10.f)
				m_fJumpBreathDegree -= 0.2f;
			else
				m_fJumpBreathDegree -= 0.01f;
		}
	}
	
	else if (CPlayer::PLAYER_WALK != *m_pPlayerState)	// 걷는 상태가 아닐 때
	{
		if (1.8f < m_fBreathCycle)					// 점점 숨이 돌아옴
			m_fBreathCycle -= 0.4f;

		if (m_fJumpBreathDegree != 0.f)
		{
			if (m_fJumpBreathDegree < 0.f)
				m_fJumpBreathDegree += 1.f;
			else
				m_fJumpBreathDegree = 0.f;

			m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fJumpBreathDegree));
		}
	}
	else
	{
		m_fBreathCycle = 15.f;						// 걷는 상태일 때 숨을 빠르게 쉼

		if (m_fJumpBreathDegree != 0.f)
		{
			if (m_fJumpBreathDegree < 0.f)
				m_fJumpBreathDegree += 1.f;
			else
				m_fJumpBreathDegree = 0.f;
			m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fJumpBreathDegree));
		}

	}

	// 숨의 파워만큼 상하로 움직여줌
	_vector vBreathPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vBreathPos.m128_f32[1] += m_fBreathPower * cosf(XMConvertToRadians(m_fBreathCount));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBreathPos);						

	m_fBreathCount += m_fBreathCycle;
}

void CPlayer_Hand::HealingShield(_double TimeDelta)
{
	m_fHitTime += (_float)TimeDelta;
	m_fHealShieldInterval -= (_float)TimeDelta;
	if (6.f < m_fHitTime && 0.f > m_fHealShieldInterval)
	{
		if (m_iMaxShield > m_iCurrentShield)
			++m_iCurrentShield;
		m_fHealShieldInterval = 0.085f;
	}
}

void CPlayer_Hand::WeaponKey(_double TimeDelta)
{
	// 1번 슬롯이 비어있지 않고 현재 무기가 1번 슬롯과 다르고 1이 눌렸을 때
	if (WEAPON_END != m_eSlot1 && 1 != m_iCurrentWeaponIndex && g_pGameInstance->Get_KeyEnter(DIK_1))
	{
		srand(8);
		m_eNextWeapon = m_eSlot1;
		m_bSwapWeapon = true;
		m_bSlot1Click = true;
	}
	// 2번 슬롯이 비어있지 않고 현재 무기가 1번 슬롯과 다르고 2가 눌렸을 때
	if (WEAPON_END != m_eSlot2 && 2 != m_iCurrentWeaponIndex && g_pGameInstance->Get_KeyEnter(DIK_2))
	{
		srand(5);
		m_eNextWeapon = m_eSlot2;
		m_bSwapWeapon = true;
		m_bSlot2Click = true;
	}
	// 3이 눌리고 현재 무기가 3번이 아닐 때
	if (3 != m_iCurrentWeaponIndex && g_pGameInstance->Get_KeyEnter(DIK_3))
	{
		srand(11);
		m_eNextWeapon = WEAPON_ANGELICAURA;
		m_bSwapWeapon = true;
	}
	else if (g_pGameInstance->Get_KeyEnter(DIK_4))
	{
		srand(22);
		m_eNextWeapon = WEAPON_TESLA;
		m_bSwapWeapon = true;
	}
	else if (g_pGameInstance->Get_KeyEnter(DIK_5))
	{
		srand(33);
		m_eNextWeapon = WEAPON_TIGERCANNON;
		m_bSwapWeapon = true;
	}
	else if (g_pGameInstance->Get_KeyEnter(DIK_6))
	{
		srand(44);
		m_eNextWeapon = WEAPON_THUNDERCLAPGLOVES;
		m_bSwapWeapon = true;
	}
	else if (g_pGameInstance->Get_KeyEnter(DIK_7))
	{
		srand(55);
		m_eNextWeapon = WEAPON_FIREDRAGON;
		m_bSwapWeapon = true;
	}
	else if (g_pGameInstance->Get_KeyEnter(DIK_8))
	{
		srand(66);
		m_eNextWeapon = WEAPON_PIERCINGFRAME;
		m_bSwapWeapon = true;
	}
	else if (g_pGameInstance->Get_KeyEnter(DIK_9))
	{
		srand(77);
		m_eNextWeapon = WEAPON_SWORD;
		m_bSwapWeapon = true;
	}
	else if ((0 < m_iCurrentSmoke && g_pGameInstance->Get_KeyEnter(DIK_Q)) || (g_pGameInstance->Get_KeyEnter(DIK_E)))
	{
		m_eNextWeapon = m_eCurrentWeapon;
		m_eCurrentWeapon = WEAPON_SKILL;
		if (true == m_bTalents[3] && g_pGameInstance->Get_KeyEnter(DIK_Q))
		{
			if (false == m_bPlusShield)
			{
				m_bPlusShield = true;
				m_iMaxShield += 60;
			}
			m_fPlusShieldTime = 0.f;
		}
		else if (true == m_bTalents[5] && g_pGameInstance->Get_KeyEnter(DIK_E))
			m_bUseEnergyOrb = true;
	}

	if (false == m_bUseSkill)
	{
		if (0.f >= m_fSkillCurrentCoolTime)
		{
			m_bUseSkill = true;
			m_fSkillCurrentCoolTime = m_fSkillMaxCoolTime;
		}
		else
			m_fSkillCurrentCoolTime -= (_float)TimeDelta;
	}


	switch (m_eCurrentWeapon)
	{
	case WEAPON_SKILL:
		Skill(TimeDelta);
		break;
	case WEAPON_ANGELICAURA:
		Angelic_Aura(TimeDelta);
		break;
	case WEAPON_TESLA:
		Tesla(TimeDelta);
		break;
	case WEAPON_TIGERCANNON:
		Tiger_Cannon(TimeDelta);
		break;
	case WEAPON_THUNDERCLAPGLOVES:
		ThunderClap_Gloves(TimeDelta);
		break;
	case WEAPON_FIREDRAGON:
		Fire_Dragon(TimeDelta);
		break;
	case WEAPON_PIERCINGFRAME:
		Piercing_Frame(TimeDelta);
		break;
	case WEAPON_SWORD:
		Sword(TimeDelta);
		break;
	}
}

void CPlayer_Hand::SwapWeapon(_double TimeDelta)
{
	if (true == m_bSwapWeapon)		// 스왑한다는 명령이 들어왔을 때
	{
		static _bool bSwapStart = true;
		if (true == bSwapStart && nullptr != m_pGloveLogo)
		{
			g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_GloveLogo"));
			m_pGloveLogo = nullptr;
			bSwapStart = false;
		}

		++m_iSwapTime;				// 스왑 시간 매 프레임 증가
		_vector SwapPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);	// 현재 포지션 받아옴.
		if (15 >= m_iSwapTime)		// 15 프레임 전에는 팔을 아래로 이동
		{
			SwapPos = XMVectorSetY(SwapPos, XMVectorGetY(SwapPos) - m_iSwapTime * 0.1f);
			if (15 == m_iSwapTime)	// 15 프레임 일때
			{
				if (true == m_bSlot2Click)		// 2번 슬롯을 눌렀을 때
				{
					// 현재 무기가 1번 무기와 같거나 3번일 때 2번 무기로 바꿔줌
					if (1 == m_iCurrentWeaponIndex || 3 == m_iCurrentWeaponIndex)
					{
						m_eCurrentWeapon = m_eSlot2;
						m_pWeaponUI->Set_TexIndex(m_eCurrentWeapon + 53);
						m_iCurrentWeaponIndex = 2;
						m_pWeaponSlot[0]->Set_Scale(128.f, 128.f);
						m_pWeaponSlot[1]->Set_Scale(160.f, 160.f);
						m_pWeaponSlot[2]->Set_Scale(128.f, 128.f);
						m_pWeaponSlot[0]->Set_Pass(12);
						m_pWeaponSlot[1]->Set_Pass(3);
						m_pWeaponSlot[2]->Set_Pass(12);
					}
					m_bSlot2Click = false;
				}
				else if (true == m_bSlot1Click)	// 1번 슬롯을 눌렀을 때
				{
					// 현재 무기가 2번이나 3번일 때 1번 슬롯으로 바꿔줌
					if (2== m_iCurrentWeaponIndex || 3 == m_iCurrentWeaponIndex)
					{
						m_eCurrentWeapon = m_eSlot1;
						m_pWeaponUI->Set_TexIndex(m_eCurrentWeapon + 53);
						m_iCurrentWeaponIndex = 1;
						if (nullptr != m_pWeaponSlot[1])
						{
							m_pWeaponSlot[1]->Set_Scale(128.f, 128.f);
							m_pWeaponSlot[1]->Set_Pass(12);
						}

						if (nullptr != m_pWeaponSlot[2])
						{
							m_pWeaponSlot[2]->Set_Scale(128.f, 128.f);
							m_pWeaponSlot[2]->Set_Pass(12);
						}
						m_pWeaponSlot[0]->Set_Scale(160.f, 160.f);
						m_pWeaponSlot[0]->Set_Pass(3);
					}
					m_bSlot1Click = false;
				}
				else if (WEAPON_ANGELICAURA == m_eNextWeapon)	// 3번 슬롯을 눌렀을 때
				{
					// 현재 무기가 2번이나 1번일 때 3번 슬롯으로 바꿔줌
					if (2 == m_iCurrentWeaponIndex || 1 == m_iCurrentWeaponIndex)
					{
						m_eCurrentWeapon = WEAPON_ANGELICAURA;
						m_pWeaponUI->Set_TexIndex(m_eCurrentWeapon + 53);
						m_iCurrentWeaponIndex = 3;

						if (nullptr != m_pWeaponSlot[0])
						{
							m_pWeaponSlot[0]->Set_Scale(128.f, 128.f);
							m_pWeaponSlot[0]->Set_Pass(12);
						}

						if (nullptr != m_pWeaponSlot[1])
						{
							m_pWeaponSlot[1]->Set_Scale(128.f, 128.f);
							m_pWeaponSlot[1]->Set_Pass(12);
						}

						m_pWeaponSlot[2]->Set_Scale(160.f, 160.f);
						m_pWeaponSlot[2]->Set_Pass(3);
					}
				}
				// 다음 무기가 1,2,3번 슬롯이 눌린게 아니라 다른걸 먹었을 때
				else if (WEAPON_TESLA <= m_eNextWeapon && WEAPON_END > m_eNextWeapon)
				{
					// 내가 2번 슬롯일 때
					if (2 == m_iCurrentWeaponIndex)
					{
						// 1번 슬롯이 비어있다면 1번 슬롯에 무기를 채워주고 1번 슬롯으로 이동
						if (WEAPON_END == m_eSlot1)
						{
							m_eSlot1 = m_eNextWeapon;
							m_eCurrentWeapon = m_eSlot1;
							m_pWeaponUI->Set_TexIndex(m_eCurrentWeapon + 53);
							m_iCurrentWeaponIndex = 1;

							INFO_UI tInfoUI = {};
							tInfoUI.fScaleX = 128.f;
							tInfoUI.fScaleY = 128.f;
							tInfoUI.fPositionX = 1120.f;
							tInfoUI.fPositionY = 700.f;
							tInfoUI.iTextureIndex = 85;
							tInfoUI.fDepth = 3.f;
							if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
							{
								MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::SwapWeapon(Weapon)");
								return;
							}
							m_pWeaponSlot[0] = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI")));

							m_pWeaponSlot[0]->Set_Scale(160.f, 160.f);
							m_pWeaponSlot[1]->Set_Scale(128.f, 128.f);
							m_pWeaponSlot[0]->Set_Pass(3);
							m_pWeaponSlot[1]->Set_Pass(12);
						}
						// 그게 아니라면 2번 슬롯에 무기를 교체
						else
						{
							m_eSlot2 = m_eNextWeapon;
							m_eCurrentWeapon = m_eSlot2;
							m_pWeaponUI->Set_TexIndex(m_eCurrentWeapon + 53);
							m_iCurrentWeaponIndex = 2;
						}
					}
					// 내가 3번 슬롯일 때
					else if (3 == m_iCurrentWeaponIndex)
					{
						// 1번 슬롯이 비어있거나 1번과 2번 모두 차있을 때는 1번 슬롯에 무기를 교체해주고 1번 슬롯으로 이동.
						if (WEAPON_END == m_eSlot1 || (WEAPON_END != m_eSlot1 && WEAPON_END != m_eSlot2))
						{
							m_eSlot1 = m_eNextWeapon;
							m_eCurrentWeapon = m_eSlot1;
							m_pWeaponUI->Set_TexIndex(m_eCurrentWeapon + 53);
							m_iCurrentWeaponIndex = 1;

							INFO_UI tInfoUI = {};
							tInfoUI.fScaleX = 128.f;
							tInfoUI.fScaleY = 128.f;
							tInfoUI.fPositionX = 1120.f;
							tInfoUI.fPositionY = 700.f;
							tInfoUI.iTextureIndex = 85;
							tInfoUI.fDepth = 3.f;
							if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
							{
								MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::SwapWeapon(Weapon)");
								return;
							}
							m_pWeaponSlot[0] = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI")));

							m_pWeaponSlot[0]->Set_Scale(160.f, 160.f);
							m_pWeaponSlot[2]->Set_Scale(128.f, 128.f);
							m_pWeaponSlot[0]->Set_Pass(3);
							m_pWeaponSlot[2]->Set_Pass(12);
						}
						// 그 외에 2번 슬롯이 비어있다면 2번 슬롯에 무기를 채워주고 2번 슬롯으로 이동.
						else if (WEAPON_END == m_eSlot2)
						{
							m_eSlot2 = m_eNextWeapon;
							m_eCurrentWeapon = m_eSlot2;
							m_pWeaponUI->Set_TexIndex(m_eCurrentWeapon + 53);
							m_iCurrentWeaponIndex = 2;

							INFO_UI tInfoUI = {};
							tInfoUI.fScaleX = 128.f;
							tInfoUI.fScaleY = 128.f;
							tInfoUI.fDepth = 3.f;
							tInfoUI.fPositionX = 1175.f;
							tInfoUI.fPositionY = 700.f;
							tInfoUI.iTextureIndex = 86;

							if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
							{
								MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::SwapWeapon(Weapon)");
								return;
							}
							m_pWeaponSlot[1] = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI")));

							m_pWeaponSlot[0]->Set_Scale(128.f, 128.f);
							m_pWeaponSlot[1]->Set_Scale(160.f, 160.f);
							m_pWeaponSlot[2]->Set_Scale(128.f, 128.f);
							m_pWeaponSlot[0]->Set_Pass(12);
							m_pWeaponSlot[1]->Set_Pass(3);
							m_pWeaponSlot[2]->Set_Pass(12);
						}
					}
					// 내가 1번 슬롯일 때
					else if (1 == m_iCurrentWeaponIndex)
					{
						// 2번 슬롯이 비어있다면 2번 슬롯에 무기를 채워주고 2번 슬롯으로 이동.
						if (WEAPON_END == m_eSlot2)
						{
							m_eSlot2 = m_eNextWeapon;
							m_eCurrentWeapon = m_eSlot2;
							m_pWeaponUI->Set_TexIndex(m_eCurrentWeapon + 53);
							m_iCurrentWeaponIndex = 2;

							INFO_UI tInfoUI = {};
							tInfoUI.fScaleX = 128.f;
							tInfoUI.fScaleY = 128.f;
							tInfoUI.fDepth = 3.f;
							tInfoUI.fPositionX = 1175.f;
							tInfoUI.fPositionY = 700.f;
							tInfoUI.iTextureIndex = 86;

							if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
							{
								MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::SwapWeapon(Weapon)");
								return;
							}
							m_pWeaponSlot[1] = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI")));

							m_pWeaponSlot[0]->Set_Scale(128.f, 128.f);
							m_pWeaponSlot[1]->Set_Scale(160.f, 160.f);
							m_pWeaponSlot[0]->Set_Pass(12);
							m_pWeaponSlot[1]->Set_Pass(3);
						}
						// 그 외엔 1번 슬롯에 무기를 교체
						else
						{
							m_eSlot1 = m_eNextWeapon;
							m_eCurrentWeapon = m_eSlot1;
							m_pWeaponUI->Set_TexIndex(m_eCurrentWeapon + 53);
							m_iCurrentWeaponIndex = 1;
						}
					}
				}

				Weapon_Info();

				if (FAILED(g_pGameInstance->Release_LayerObject(LEVEL_STATIC, TEXT("Layer_BulletNumber"))))
				{
					MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CPlayer_Hand::SwapWeapon");
					return;
				}

				if (FAILED(g_pGameInstance->Release_LayerObject(LEVEL_STATIC, TEXT("Layer_BulletNumberUI"))))
				{
					MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CPlayer_Hand::SwapWeapon");
					return;
				}

				if (FAILED(Ready_Layer_UI_BulletNumber()))
				{
					MSGBOX("Ready_Layer_UI_BulletNumber returned E_FAIL in CPlayer_Hand::SwapWeapon");
					return;
				}
			}
		}
		else if (15 < m_iSwapTime && 30 >= m_iSwapTime)
		{
			SwapPos = XMVectorSetY(SwapPos, XMVectorGetY(SwapPos) - (30 - m_iSwapTime) * 0.1f);
		}
		else if (30 < m_iSwapTime)
		{
			m_bSwapWeapon = false;
			m_iSwapTime = 0;
			bSwapStart = true;
			g_pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
			g_pGameInstance->LetsPlaySound(TEXT("SwapWeapon.mp3"), CSoundMgr::PLAYER_SHOT, 0.5f);
		}
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, SwapPos);
	}
}

void CPlayer_Hand::Skill(_double TimeDelta)
{
	if (0 < m_iCurrentSmoke && g_pGameInstance->Get_KeyEnter(DIK_Q))
	{
		m_pModelArmCom[m_eCurrentWeapon]->SetUp_AnimIndexChange(1);
		m_pModelArmCom[m_eCurrentWeapon]->Set_PlayAcc(0.05f);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_PoisonSmoke"), TEXT("Prototype_GameObject_PoisonSmoke"))))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Skill");
			return;
		}

		if (nullptr != m_pGloveLogo)
		{
			g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_GloveLogo"));
			m_pGloveLogo = nullptr;
		}

		m_iCurrentSmoke--;
	}

	if (true == m_bUseSkill && g_pGameInstance->Get_KeyEnter(DIK_E))
	{
		m_pModelArmCom[m_eCurrentWeapon]->SetUp_AnimIndexChange(2);
		m_pModelArmCom[m_eCurrentWeapon]->Set_PlayAcc(0.25f);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_EnergyOrb"), TEXT("Prototype_GameObject_EnergyOrb"))))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Skill");
			return;
		}

		CCoolTime::COOLTIMEDESC tCoolTimeDesc;
		tCoolTimeDesc.tInfoUI.fPositionX = 640.f;
		tCoolTimeDesc.tInfoUI.fPositionY = 600.f;
		tCoolTimeDesc.tInfoUI.fScaleX = 64.f;
		tCoolTimeDesc.tInfoUI.fScaleY = 64.f;
		tCoolTimeDesc.tInfoUI.fDepth = -0.5f;
		tCoolTimeDesc.tInfoUI.iTextureIndex = 0;
		tCoolTimeDesc.eSkill_Type = CCoolTime::TYPE_ENERGE_ORB;
		tCoolTimeDesc.pCurrentCoolTime = &m_fSkillCurrentCoolTime;
		tCoolTimeDesc.pMaxCoolTime = &m_fSkillMaxCoolTime;

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_CoolTime"), TEXT("Prototype_GameObject_CoolTime"), &tCoolTimeDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Skill");
			return;
		}

		if (nullptr != m_pGloveLogo)
		{
			g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_GloveLogo"));
			m_pGloveLogo = nullptr;
		}
		g_pGameInstance->StopSound(CSoundMgr::PLAYER);
		g_pGameInstance->LetsPlaySound(TEXT("EnergyOrb.mp3"), CSoundMgr::PLAYER, 0.5f);
		m_bUseSkill = false;
	}

	if (1 == m_pModelArmCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelArmCom[m_eCurrentWeapon]->Get_EndAnimation(1))
	{
		if (m_eNextWeapon == WEAPON_THUNDERCLAPGLOVES)
			if (nullptr == g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_GloveLogo")) || 0 == g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_GloveLogo")))
			{
				CEffect::EFFECTDESC tEffectDesc;
				tEffectDesc.eMove = CEffect::MOVE_CHASE;
				tEffectDesc.eType = CEffect::TYPE_2D;

				tEffectDesc.eWay = CEffect::WAY_END;
				tEffectDesc.iPass = 24;
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, -0.012f);
				tEffectDesc.pDead = &m_bDead;
				tEffectDesc.pParentWorldMatrix = &m_GloveLogoMatrix;
				tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_GloveLogo");
				tEffectDesc.vScale = XMVectorSet(0.13f, 0.13f, 1.f, 0.f);
				tEffectDesc.bNotLookCamera = true;
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_GloveLogo"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Skill");
					return;
				}
				m_pGloveLogo = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_GloveLogo"));
			}
		m_eCurrentWeapon = m_eNextWeapon;
	}
	else if (2 == m_pModelArmCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelArmCom[m_eCurrentWeapon]->Get_EndAnimation(2))
	{
		if (m_eNextWeapon == WEAPON_THUNDERCLAPGLOVES)
			if (nullptr == g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_GloveLogo")) || 0 == g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_GloveLogo")))
			{
				CEffect::EFFECTDESC tEffectDesc;
				tEffectDesc.eMove = CEffect::MOVE_CHASE;
				tEffectDesc.eType = CEffect::TYPE_2D;
				tEffectDesc.eWay = CEffect::WAY_END;
				tEffectDesc.iPass = 24;
				tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, -0.012f);
				tEffectDesc.pDead = &m_bDead;
				tEffectDesc.pParentWorldMatrix = &m_GloveLogoMatrix;
				tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_GloveLogo");
				tEffectDesc.vScale = XMVectorSet(0.13f, 0.13f, 1.f, 0.f);
				tEffectDesc.bNotLookCamera = true;
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_GloveLogo"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Skill");
					return;
				}
				m_pGloveLogo = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_GloveLogo"));
			}
		m_eCurrentWeapon = m_eNextWeapon;
	}
}

void CPlayer_Hand::Angelic_Aura(_double TimeDelta)
{
	static bool bReload = false;
	if ((g_pGameInstance->Get_KeyEnter(DIK_R) || 0 >= m_iCurrentBullet) && m_iCurrentBullet < m_iBulletSize && 0 < m_iMaxBullet)
	{
		if (false == bReload)
		{
			g_pGameInstance->StopSound(CSoundMgr::EFFECT4);
			g_pGameInstance->LetsPlaySound(TEXT("AngelicAuraReload.mp3"), CSoundMgr::EFFECT4, 0.6f);
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(2);
			m_iAnimationType = ANIM_ONETIME;
			bReload = true;
		}

	}

	static _float iShootInterval = 1.f;
	if (true == m_bOil && true == m_bTalents[1])
		iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * 1.6f * (_float)TimeDelta;
	else
		iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * (_float)TimeDelta;

	if (2 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) && 0 < m_iCurrentBullet)
	{
		m_iAnimationType = ANIM_ONETIME;

		if (1.f <= iShootInterval)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
			pGameInstance->LetsPlaySound(TEXT("AngelicAuraShot.mp3"), CSoundMgr::PLAYER_SHOT, 0.6f);
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
				_float Dist = 0.f;
				_float MinDist = 3000.f;
				CCollider* pCollision = nullptr;
				for (auto& pWeakCollider : *pWeakColliderList)
				{
					if (true == pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pWeakCollider, &Dist))
					{
						if (MinDist > Dist)
						{
							pCollision = pWeakCollider;
							MinDist = Dist;
							bColl = true;
						}
					}
				}

				static _int iSoundCount = 0;
				if (true == bColl && nullptr != pCollision)
				{
					switch (iSoundCount)
					{
					case 0:
						pGameInstance->StopSound(CSoundMgr::EFFECT);
						pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT, 0.7f);
						break;
					case 1:
						pGameInstance->StopSound(CSoundMgr::EFFECT1);
						pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT1, 0.7f);
						break;
					case 2:
						pGameInstance->StopSound(CSoundMgr::EFFECT2);
						pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT2, 0.7f);
						break;
					case 3:
						pGameInstance->StopSound(CSoundMgr::EFFECT3);
						pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT3, 0.7f);
						break;
					}

					CDamage::NUMBERDESC tNumberDesc;
					tNumberDesc.eType = CDamage::TYPE_CRITICAL;
					if (false == m_bUseEnergyOrb)
						tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 3.f);
					else
					{
						tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack * 4.f, 3.f);
						m_bUseEnergyOrb = false;
					}

					tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * (MinDist - 1.f);
					XMStoreFloat4(&m_vHitPosition, tNumberDesc.vPosition);
					if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pCollision->Get_Parent())->Get_Strong()))
						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Anglic_Aura");
							return;
						}

					CParticle_Effect::PARTICLEDESC tParticleDesc;
					tParticleDesc.iTexIndex = 41;
					tParticleDesc.fMinSpeed = 20.f;
					tParticleDesc.fMaxSpeed = 30.f;
					tParticleDesc.fOriginScale = 0.12f;
					tParticleDesc.fScaleSpeed = 0.f;
					tParticleDesc.fGravityPower = 0.f/*6.02f*/;
					tParticleDesc.fLiveTime = 0.33f;
					tParticleDesc.fMakeRange = 0.f;
					tParticleDesc.fMaxDistance = 5.48f;
					tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
					tParticleDesc.pParentPosition = &m_vHitPosition;
					tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
					tParticleDesc.bUseEasing = true;
					tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
					tParticleDesc.bChase = false;
					tParticleDesc.bRoof = false;
					tParticleDesc.bTrail = false;
					tParticleDesc.pDead = nullptr;
					if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Angelic_Aura");
						return;
					}
				}

				if (false == bColl)
				{
					for (auto& pBodyCollider : *pBodyColliderList)
					{
						if (true == pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pBodyCollider, &Dist))
						{
							if (MinDist > Dist)
							{
								pCollision = pBodyCollider;
								MinDist = Dist;
								bColl = true;
							}
						}
					}
					if (true == bColl && nullptr != pCollision)
					{
						switch (iSoundCount)
						{
						case 0:
							pGameInstance->StopSound(CSoundMgr::EFFECT);
							pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT, 0.7f);
							break;
						case 1:
							pGameInstance->StopSound(CSoundMgr::EFFECT1);
							pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT1, 0.7f);
							break;
						case 2:
							pGameInstance->StopSound(CSoundMgr::EFFECT2);
							pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT2, 0.7f);
							break;
						case 3:
							pGameInstance->StopSound(CSoundMgr::EFFECT3);
							pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT3, 0.7f);
							break;
						}
						CDamage::NUMBERDESC tNumberDesc;
						tNumberDesc.eType = CDamage::TYPE_ORIGIN;
						if (false == m_bUseEnergyOrb)
							tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 1.f);
						else
						{
							tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack * 4.f, 1.f);
							m_bUseEnergyOrb = false;
						}

						tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * (MinDist - 1.f);
						XMStoreFloat4(&m_vHitPosition, tNumberDesc.vPosition);

						if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pCollision->Get_Parent())->Get_Strong()))
							if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
							{
								MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Anglic_Aura");
								return;
							}

						CParticle_Effect::PARTICLEDESC tParticleDesc;
						tParticleDesc.iTexIndex = 41;
						tParticleDesc.fMinSpeed = 20.f;
						tParticleDesc.fMaxSpeed = 30.f;
						tParticleDesc.fOriginScale = 0.12f;
						tParticleDesc.fScaleSpeed = 0.f;
						tParticleDesc.fGravityPower = /*6.02f*/0.f;
						tParticleDesc.fLiveTime = 0.33f;
						tParticleDesc.fMakeRange = 0.f;
						tParticleDesc.fMaxDistance = 5.48f;
						tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
						tParticleDesc.pParentPosition = &m_vHitPosition;
						tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
						tParticleDesc.bUseEasing = true;
						tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
						tParticleDesc.bChase = false;
						tParticleDesc.bRoof = false;
						tParticleDesc.bTrail = false;
						tParticleDesc.pDead = nullptr;
						if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Angelic_Aura");
							return;
						}
					}
				}
				iSoundCount++;
				if (3 < iSoundCount)
					iSoundCount = 0;
				
			}
			RELEASE_INSTANCE(CGameInstance);

			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndexChange(0);
			m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -(_float)TimeDelta * m_tCurrentWeaponInfo.fVerticalRecoil);

			_int iHorizonRecoil = rand() % 101 - 50;
			while (abs(m_iOldHorizonRecoil - iHorizonRecoil) > 50)
				iHorizonRecoil = rand() % 101 - 50;

			m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(_float)TimeDelta * _float(iHorizonRecoil) * 0.1f * m_tCurrentWeaponInfo.fHorizontalRecoil);
			m_iOldHorizonRecoil = iHorizonRecoil;
			m_fHorizonRecoilValue += (_float)TimeDelta * _float(iHorizonRecoil) * 0.1f * m_tCurrentWeaponInfo.fHorizontalRecoil;
			m_fVerticalRecoilValue += (_float)TimeDelta * m_tCurrentWeaponInfo.fVerticalRecoil;

			iShootInterval = 0.f;
			m_iCurrentBullet--;

			CHierarchyNode* pNode = m_pModelCom[WEAPON_ANGELICAURA]->Find_HierarchyNode("muzzle");
			_matrix TransformationMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix())* pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom[WEAPON_ANGELICAURA]->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
			XMStoreFloat4x4(&m_MuzzleMatrix, TransformationMatrix);

			CEffect::EFFECTDESC tEffectDesc;
			tEffectDesc.eMove = CEffect::MOVE_CHASE;
			tEffectDesc.eType = CEffect::TYPE_SPRITE;
			tEffectDesc.eWay = CEffect::WAY_FINAL;
			tEffectDesc.fSpriteSpeed = 40.f;
			tEffectDesc.iPass = 0;
			tEffectDesc.iSpriteNumX = 1;
			tEffectDesc.iSpriteNumY = 1;
			tEffectDesc.iSpriteStartX = 0;
			tEffectDesc.iSpriteStartY = 0;
			tEffectDesc.MyWorldMatrix = /*XMMatrixIdentity();*/XMMatrixTranslation(0.17f,0.f,-0.02f);
			tEffectDesc.pDead = nullptr;
			tEffectDesc.pParentWorldMatrix = &m_MuzzleMatrix;
			tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_AngelMuzzle");
			tEffectDesc.vScale = XMVectorSet(0.5f, 0.5f, 1.f, 0.f);
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Anglic_Aura");
				return;
			}
		}
	}
	else if (0.f < m_fVerticalRecoilValue || 0.f < fabsf(m_fHorizonRecoilValue))	// 총 쏘는 중이 아니고 반동에 의해 에임이 위로 간 상태일 때 복구
	{
		if (0.f < m_fVerticalRecoilValue)
		{
			if ((_float)TimeDelta * 0.2f <= m_fVerticalRecoilValue)
			{
				m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (_float)TimeDelta * 0.2f);
				m_fVerticalRecoilValue -= (_float)TimeDelta * 0.2f;
			}
			else
			{
				m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fVerticalRecoilValue);
				m_fVerticalRecoilValue = 0.f;
			}
		}
		
		if (0.f < fabsf(m_fHorizonRecoilValue))
		{
			if ((_float)TimeDelta * 0.05f <= fabsf(m_fHorizonRecoilValue))
			{
				if (m_fHorizonRecoilValue > 0.f)
				{
					m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (_float)TimeDelta * 0.05f);
					m_fHorizonRecoilValue -= (_float)TimeDelta * 0.05f;
				}
				else
				{
					m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(_float)TimeDelta * 0.05f);
					m_fHorizonRecoilValue += (_float)TimeDelta * 0.05f;
				}
			}
			else
			{
				m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fHorizonRecoilValue);
				m_fHorizonRecoilValue = 0.f;
			}
		}
	}
	else if (0.f > m_fVerticalRecoilValue)
	{
		if (0.f > m_fVerticalRecoilValue)
			m_fVerticalRecoilValue = 0.f;
	}



	if (2 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(2))
	{
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(1);

		if (m_iBulletSize > m_iMaxBullet)
		{
			m_iCurrentBullet = m_iMaxBullet;
			m_iMaxBullet = 0;
		}
		else
		{
			m_iMaxBullet -= (m_iBulletSize - m_iCurrentBullet);
			m_iCurrentBullet = m_iBulletSize;
		}
		bReload = false;
	}
}

void CPlayer_Hand::Tesla(_double TimeDelta)
{
	static _float iShootInterval = 1.f;

	CHierarchyNode* pNode = m_pModelCom[WEAPON_TESLA]->Find_HierarchyNode("muzzle");
	_matrix TransformationMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix())* pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom[WEAPON_TESLA]->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	XMStoreFloat4x4(&m_TeslaMuzzleMatrix, TransformationMatrix);

	if (1.f > iShootInterval && 0.f < m_fVerticalRecoilValue)	// 총 쏘는 중이 아니고 반동에 의해 에임이 위로 간 상태일 때 복구
	{
		if ((_float)TimeDelta * 0.04f <= m_fVerticalRecoilValue)
		{
			m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (_float)TimeDelta * 0.04f);
			m_fVerticalRecoilValue -= (_float)TimeDelta * 0.04f;
		}
		else
		{
			m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fVerticalRecoilValue);
			m_fVerticalRecoilValue = 0.f;
		}
	}
	iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * (_float)TimeDelta;


	if ((g_pGameInstance->Get_KeyEnter(DIK_R) || 0 >= m_iCurrentBlueBullet) && m_iCurrentBlueBullet < m_iBlueBulletSize && 0 < m_iMaxBlueBullet)
	{
		if (false == m_bWeaponReload)
		{
			g_pGameInstance->StopSound(CSoundMgr::EFFECT4);
			g_pGameInstance->LetsPlaySound(TEXT("TeslaReload.mp3"), CSoundMgr::EFFECT4, 0.5f);
			m_bWeaponReload = true;
		}
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(1);
		m_iAnimationType = ANIM_ONETIME;
	}
	else if (0 < m_iCurrentBlueBullet && 1 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
	{
		if (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
		{
			if (1.f <= iShootInterval)
			{
				g_pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
				g_pGameInstance->LetsPlaySound(TEXT("tesla.mp3"), CSoundMgr::PLAYER_SHOT, 0.7f);

				list<CCollider*>* pWeakColliderList = g_pGameInstance->Get_ColliderList(TEXT("Weakness"), CCollision_Manager::TARGET_ENEMY);
				list<CCollider*>* pBodyColliderList = g_pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
				list<CCollider*>* pBulletColliderList = g_pGameInstance->Get_ColliderList(TEXT("Monster_Bullet"), CCollision_Manager::TARGET_ENEMY);
				_float MinDist = 3000.f;
				if (nullptr != pBulletColliderList)
				{
					for (auto& pBulletCollider : *pBulletColliderList)
					{
						_float Dist = 0.f;
						if (true == g_pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pBulletCollider, &Dist))
						{
							if (MinDist > Dist)
								MinDist = Dist;
						}
					}
				}
				if (nullptr != pWeakColliderList && nullptr != pBodyColliderList)
				{
					_bool bColl = false;
					_float Dist = 0.f;
		
					CCollider* pCollision = nullptr;
					for (auto& pWeakCollider : *pWeakColliderList)
					{
						if (true == g_pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pWeakCollider, &Dist))
						{
							if (MinDist > Dist)
							{
								pCollision = pWeakCollider;
								MinDist = Dist;
							}
						}
					}

					if (false == bColl)
					{
						for (auto& pBodyCollider : *pBodyColliderList)
						{
							if (true == g_pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pBodyCollider, &Dist))
							{
								if (MinDist > Dist)
								{
									pCollision = pBodyCollider;
									MinDist = Dist;
								}
							}
						}
					}
				}
				CBullet_Tesla::BULLETDESC tBulletDesc;
				tBulletDesc.pModel = m_pModelCom[WEAPON_TESLA];
				tBulletDesc.pSocketName = "muzzle";
				tBulletDesc.pTransform = m_pTransformCom;
				if (300.f > MinDist)
					tBulletDesc.fDist = MinDist;
				else
					tBulletDesc.fDist = 20.f;

				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Bullet_Tesla"), TEXT("Prototype_GameObject_Bullet_Tesla"), &tBulletDesc)))
				{
					MSGBOX("Add_GameObject returned E_FAIL in CPlayer_Hand::Tesla");
					return;
				}

				m_iAnimationType = ANIM_ONETIME;
				m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndexChange(5);
				m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -(_float)TimeDelta * m_tCurrentWeaponInfo.fVerticalRecoil);
				m_fVerticalRecoilValue += (_float)TimeDelta * m_tCurrentWeaponInfo.fVerticalRecoil;

				iShootInterval = 0.f;



				CEffect::EFFECTDESC tEffectDesc;
				tEffectDesc.bNotLookCamera = false;
				tEffectDesc.eMove = CEffect::MOVE_CHASE;
				tEffectDesc.eType = CEffect::TYPE_SPRITE;
				tEffectDesc.eWay = CEffect::WAY_FINAL;
				tEffectDesc.fSpriteSpeed = 40.f;
				tEffectDesc.iNumTexIndex = 0;
				tEffectDesc.iPass = 20;
				tEffectDesc.iSpriteNumX = 2;
				tEffectDesc.iSpriteNumY = 2;
				tEffectDesc.iSpriteStartX = 0;
				tEffectDesc.iSpriteStartY = 0;
				tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
				tEffectDesc.pDead = nullptr;
				tEffectDesc.pParentWorldMatrix = &m_TeslaMuzzleMatrix;
				tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_TeslaMuzzleFlash");
				tEffectDesc.vScale = XMVectorSet(0.8f, 0.8f, 0.8f, 0.f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Tesla");
					return;
				}

				tEffectDesc.fSpriteSpeed = 40.f;
				tEffectDesc.iSpriteNumX = 4;
				tEffectDesc.iSpriteNumY = 3;
				tEffectDesc.iPass = 36;
				tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
				tEffectDesc.pParentWorldMatrix = &m_TeslaMuzzleMatrix;
				tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_TeslaMuzzleLightning");
				tEffectDesc.vScale = XMVectorSet(0.8f, 0.8f, 0.8f, 0.f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Tesla");
					return;
				}
				
				// 총구 파티클
				_float4	vMuzzlePos;
				memcpy(&vMuzzlePos, &m_TeslaMuzzleMatrix._41, sizeof(_float4));

				CParticle_Effect::PARTICLEDESC tParticleDesc;
				tParticleDesc.iTexIndex = 32;
				tParticleDesc.fMinSpeed = 3.5f;
				tParticleDesc.fMaxSpeed = 5.f;
				tParticleDesc.fOriginScale = 0.07f;
				tParticleDesc.fScaleSpeed = 0.f;
				tParticleDesc.fGravityPower = 0.f;
				tParticleDesc.fLiveTime = 0.1f;
				tParticleDesc.fMakeRange = 0.f;
				tParticleDesc.fMaxDistance = 2.f;
				tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
				tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
				tParticleDesc.pParentPosition = &vMuzzlePos;
				tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
				tParticleDesc.bUseEasing = false;
				tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
				tParticleDesc.bChase = false;
				tParticleDesc.bRoof = false;
				tParticleDesc.bTrail = false;
				tParticleDesc.fTrailInterval = 0.f;
				tParticleDesc.pDead = nullptr;
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Tesla");
					return;
				}

				m_iCurrentBlueBullet--;
				m_iTeslaBullet = m_iCurrentBlueBullet;
			}
		}
		else if (g_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_RBUTTON))
		{
			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(2);
		}
		else if (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON))
		{
			if (2 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(2))
			{
				m_iAnimationType = ANIM_ROOF;
				m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(4);
			}
		}
		else if (g_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_RBUTTON))
		{
			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(3);
			m_iCurrentBlueBullet--;
			m_iTeslaBullet = m_iCurrentBlueBullet;
		}
		else if (3 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(3))
		{
			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(6);
		}
		else if (6 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(6))
		{
			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);
		}
	}
	

	if (1 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(1))
	{
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);

		if (m_iBlueBulletSize > m_iMaxBlueBullet)
		{
			m_iCurrentBlueBullet = m_iMaxBlueBullet;
			m_iMaxBlueBullet = 0;
		}
		else
		{
			m_iMaxBlueBullet -= (m_iBlueBulletSize - m_iCurrentBlueBullet);
			m_iCurrentBlueBullet = m_iBlueBulletSize;
		}
		m_bWeaponReload = false;
	}
}

void CPlayer_Hand::Tiger_Cannon(_double TimeDelta)
{
	static bool bReload0 = false;
	static bool bReload1 = false;
	CHierarchyNode* pNode = m_pModelCom[WEAPON_TIGERCANNON]->Find_HierarchyNode("muzzle");
	_matrix TransformationMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix())* pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom[WEAPON_TIGERCANNON]->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	XMStoreFloat4x4(&m_CannonMuzzleMatrix, TransformationMatrix);

	if ((g_pGameInstance->Get_KeyEnter(DIK_R) || 0 >= m_iCurrentYellowBullet) && m_iCurrentYellowBullet < m_iYellowBulletSize && 0 < m_iMaxYellowBullet)
	{
		if (m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(0) || 0 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
		{
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(2);
			m_iAnimationType = ANIM_ONETIME;
		}
	}
	if (2 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
	{
		if (false == bReload0 && 0.466 < m_pModelCom[m_eCurrentWeapon]->Get_PlayAcc())
		{
			g_pGameInstance->StopSound(CSoundMgr::EFFECT4);
			g_pGameInstance->LetsPlaySound(TEXT("TigerCannonReload0.mp3"), CSoundMgr::EFFECT4, 0.7f);
			bReload0 = true;
		}
		if (false == bReload1 && 0.833 < m_pModelCom[m_eCurrentWeapon]->Get_PlayAcc())
		{
			g_pGameInstance->StopSound(CSoundMgr::EFFECT4);
			g_pGameInstance->LetsPlaySound(TEXT("TigerCannonReload1.mp3"), CSoundMgr::EFFECT4, 0.7f);
			bReload1 = true;
		}
	}

	else if (2 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && g_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON) && 0 < m_iCurrentYellowBullet)
	{
		g_pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
		g_pGameInstance->LetsPlaySound(TEXT("TigerCannonShot.mp3"), CSoundMgr::PLAYER_SHOT, 0.7f);
		CBullet_TigerCannon::BULLETDESC tBulletDesc;
		tBulletDesc.pModel = m_pModelCom[WEAPON_TIGERCANNON];
		tBulletDesc.pSocketName = "muzzle";
		tBulletDesc.pTransform = m_pTransformCom;
		tBulletDesc.fDist = m_fBulletDist;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Bullet_TigerCannon"), TEXT("Prototype_GameObject_Bullet_TigerCannon"), &tBulletDesc)))
		{
			MSGBOX("Add_GameObject returned E_FAIL in CPlayer_Hand::TigerCannon");
			return;
		}

		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);

		m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -(_float)TimeDelta * m_tCurrentWeaponInfo.fVerticalRecoil);

		_int iHorizonRecoil = rand() % 101 - 50;
		while (abs(m_iOldHorizonRecoil - iHorizonRecoil) > 50)
			iHorizonRecoil = rand() % 101 - 50;

		m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(_float)TimeDelta * _float(iHorizonRecoil) * 0.1f * m_tCurrentWeaponInfo.fHorizontalRecoil);
		m_iOldHorizonRecoil = iHorizonRecoil;
		m_fHorizonRecoilValue += (_float)TimeDelta * _float(iHorizonRecoil) * 0.1f * m_tCurrentWeaponInfo.fHorizontalRecoil;
		m_fVerticalRecoilValue += (_float)TimeDelta * m_tCurrentWeaponInfo.fVerticalRecoil;

		m_iCurrentYellowBullet--;
		m_iTiger_CannonBullet = m_iCurrentYellowBullet;


		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.eMove = CEffect::MOVE_CHASE;
		tEffectDesc.eType = CEffect::TYPE_SPRITE;
		tEffectDesc.eWay = CEffect::WAY_FINAL;
		tEffectDesc.fSpriteSpeed = 12.f;
		tEffectDesc.iPass = 20;
		tEffectDesc.iSpriteNumX = 2;
		tEffectDesc.iSpriteNumY = 1;
		tEffectDesc.iSpriteStartX = 0;
		tEffectDesc.iSpriteStartY = 0;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(-0.1f, 0.1f, 0.f);
		tEffectDesc.pDead = &m_bDead;
		tEffectDesc.pParentWorldMatrix = &m_CannonMuzzleMatrix;
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_CannonMuzzle");
		tEffectDesc.vScale = XMVectorSet(1.5f, 1.5f, 1.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::TigerCannon");
			return;
		}

	}
	if (!g_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON) && (0.f < m_fVerticalRecoilValue || 0.f < fabsf(m_fHorizonRecoilValue)))	// 총 쏘는 중이 아니고 반동에 의해 에임이 위로 간 상태일 때 복구
	{
		if (0.f < m_fVerticalRecoilValue)
		{
			if ((_float)TimeDelta * 0.4f <= m_fVerticalRecoilValue)
			{
				m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (_float)TimeDelta * 0.4f);
				m_fVerticalRecoilValue -= (_float)TimeDelta * 0.4f;
			}
			else
			{
				m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fVerticalRecoilValue);
				m_fVerticalRecoilValue = 0.f;
			}
		}

		if (0.f < fabsf(m_fHorizonRecoilValue))
		{
			if ((_float)TimeDelta * 0.1f <= fabsf(m_fHorizonRecoilValue))
			{
				if (m_fHorizonRecoilValue > 0.f)
				{
					m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (_float)TimeDelta * 0.1f);
					m_fHorizonRecoilValue -= (_float)TimeDelta * 0.1f;
				}
				else
				{
					m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(_float)TimeDelta * 0.1f);
					m_fHorizonRecoilValue += (_float)TimeDelta * 0.1f;
				}
			}
			else
			{
				m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fHorizonRecoilValue);
				m_fHorizonRecoilValue = 0.f;
			}
		}
	}

	if (0.f > m_fVerticalRecoilValue)
	{
		if (0.f > m_fVerticalRecoilValue)
			m_fVerticalRecoilValue = 0.f;
	}

	if (2 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(2))
	{
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(1);

		if (m_iYellowBulletSize > m_iMaxYellowBullet)
		{
			m_iCurrentYellowBullet = m_iMaxYellowBullet;
			m_iMaxYellowBullet = 0;
			m_iTiger_CannonBullet = m_iCurrentYellowBullet;
		}
		else
		{
			m_iMaxYellowBullet -= (m_iYellowBulletSize - m_iCurrentYellowBullet);
			m_iCurrentYellowBullet = m_iYellowBulletSize;
			m_iTiger_CannonBullet = m_iCurrentYellowBullet;
		}

		bReload0 = false;
		bReload1 = false;
	}
}

void CPlayer_Hand::ThunderClap_Gloves(_double TimeDelta)
{
	static _float iShootInterval = 1.f;

	if (true == m_bOil && true == m_bTalents[1])
		iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * 1.6f * (_float)TimeDelta;
	else
		iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * (_float)TimeDelta;
	CHierarchyNode* pNode = m_pModelCom[WEAPON_THUNDERCLAPGLOVES]->Find_HierarchyNode("logo_01");
	_matrix TransformationMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix())* pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom[WEAPON_THUNDERCLAPGLOVES]->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	XMStoreFloat4x4(&m_GloveLogoMatrix, TransformationMatrix);

	if (nullptr == g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_GloveLogo")) || 0 == g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_GloveLogo")))
	{
		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.eMove = CEffect::MOVE_CHASE;
		tEffectDesc.eType = CEffect::TYPE_2D;
		tEffectDesc.eWay = CEffect::WAY_END;
		tEffectDesc.iPass = 24;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, -0.012f);
		tEffectDesc.pDead = &m_bDead;
		tEffectDesc.pParentWorldMatrix = &m_GloveLogoMatrix;
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_GloveLogo");
		tEffectDesc.vScale = XMVectorSet(0.13f, 0.13f, 1.f, 0.f);
		tEffectDesc.bNotLookCamera = true;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_GloveLogo"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::ThunderClap_Gloves");
			return;
		}
		m_pGloveLogo = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_GloveLogo"));
	}

	if ((g_pGameInstance->Get_KeyEnter(DIK_R) || 0 >= m_iCurrentGreenBullet) && m_iCurrentGreenBullet < m_iGreenBulletSize && 0 < m_iMaxGreenBullet)
	{
		if (false == m_bWeaponReload)
		{
			g_pGameInstance->StopSound(CSoundMgr::EFFECT4);
			g_pGameInstance->LetsPlaySound(TEXT("ThunderGloveReload.mp3"), CSoundMgr::EFFECT4, 0.5f);
			g_pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
			m_bWeaponReload = true;
		}
		g_pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
		g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Bullet_ThunderClapGloves"));
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(1);
	}
	else if (1 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
	{
		if (g_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON) && 0 < m_iCurrentGreenBullet)
		{
			g_pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
			g_pGameInstance->PlayRoofSound(TEXT("laser.mp3"), CSoundMgr::PLAYER_SHOT, 0.4f);
			CBullet_ThunderClapGloves::BULLETDESC tBulletDesc;
			tBulletDesc.pModel = m_pModelCom[WEAPON_THUNDERCLAPGLOVES];
			tBulletDesc.pSocketName = "1402_Bone002";
			tBulletDesc.pTransform = m_pTransformCom;
			tBulletDesc.pDist = &m_fBulletDist;

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Bullet_ThunderClapGloves"), TEXT("Prototype_GameObject_Bullet_ThunderClapGloves"), &tBulletDesc)))
			{
				MSGBOX("Add_GameObject returned E_FAIL in CPlayer_Hand::ThunderClap_Gloves");
				return;
			}

			m_iAnimationType = ANIM_ROOF;
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(2);
		}
		else if (g_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
		{
			g_pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
			m_iAnimationType = ANIM_ONETIME;
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);
			g_pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
			m_bHeadCollision = false;
			m_bBodyCollision = false;
		}
		if (nullptr != g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_Bullet_ThunderClapGloves")) && 0 < g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_Bullet_ThunderClapGloves")) && g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) && 0 < m_iCurrentGreenBullet)
		{
			if (1.f < iShootInterval)
			{
				iShootInterval = 0.f;
				m_iCurrentGreenBullet--;
				m_iThunderClap_GlovesBullet = m_iCurrentGreenBullet;
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
					_float Dist = 0.f;
					_float MinDist = 3000.f;
					CCollider* pCollision = nullptr;
					for (auto& pWeakCollider : *pWeakColliderList)
					{
						if (true == pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pWeakCollider, &Dist))
						{
							if (MinDist > Dist)
							{
								pCollision = pWeakCollider;
								MinDist = Dist;
								bColl = true;
							}
						}
					}
					static _int iSoundCount = 0;

					if (true == bColl && nullptr != pCollision)
					{
						if (false == m_bHeadCollision)
						{
							switch (iSoundCount)
							{
							case 0:
								pGameInstance->StopSound(CSoundMgr::EFFECT);
								pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT, 0.7f);
								break;
							case 1:
								pGameInstance->StopSound(CSoundMgr::EFFECT1);
								pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT1, 0.7f);
								break;
							case 2:
								pGameInstance->StopSound(CSoundMgr::EFFECT2);
								pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT2, 0.7f);
								break;
							case 3:
								pGameInstance->StopSound(CSoundMgr::EFFECT3);
								pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT3, 0.7f);
								break;
							}
							m_bHeadCollision = true;
						}


						CDamage::NUMBERDESC tNumberDesc;
						tNumberDesc.eType = CDamage::TYPE_CRITICAL;
						if (false == m_bUseEnergyOrb)
							tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 2.f);
						else
						{
							tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack * 4.f, 2.f);
							m_bUseEnergyOrb = false;
						}
						tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * (MinDist - 1.f);
						_int iElement = rand() % 20;
						if (0 == iElement)
							static_cast<CMonster*>(pCollision->Get_Parent())->Set_Shock(true);

						if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pCollision->Get_Parent())->Get_Strong()))
							if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
							{
								MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::ThunderClap_Gloves");
								return;
							}
					}
					else
						m_bHeadCollision = false;

					if (false == bColl)
					{
						for (auto& pBodyCollider : *pBodyColliderList)
						{
							if (true == pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pBodyCollider, &Dist))
							{
								if (MinDist > Dist)
								{
									pCollision = pBodyCollider;
									MinDist = Dist;
									bColl = true;
								}
							}
						}
						if (true == bColl && nullptr != pCollision)
						{
							if (false == m_bBodyCollision)
							{
								switch (iSoundCount)
								{
								case 0:
									pGameInstance->StopSound(CSoundMgr::EFFECT);
									pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT, 0.7f);
									break;
								case 1:
									pGameInstance->StopSound(CSoundMgr::EFFECT1);
									pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT1, 0.7f);
									break;
								case 2:
									pGameInstance->StopSound(CSoundMgr::EFFECT2);
									pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT2, 0.7f);
									break;
								case 3:
									pGameInstance->StopSound(CSoundMgr::EFFECT3);
									pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT3, 0.7f);
									break;
								}
								m_bBodyCollision = true;
							}


							CDamage::NUMBERDESC tNumberDesc;
							tNumberDesc.eType = CDamage::TYPE_ORIGIN;
							if (false == m_bUseEnergyOrb)
								tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 1.f);
							else
							{
								tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack * 4.f, 1.f);
								m_bUseEnergyOrb = false;
							}

							tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * (MinDist - 1.f);
							_int iElement = rand() % 20;
							if (0 == iElement)
								static_cast<CMonster*>(pCollision->Get_Parent())->Set_Shock(true);

							if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pCollision->Get_Parent())->Get_Strong()))
								if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
								{
									MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::ThunderClap_Gloves");
									return;
								}
						}
						else
							m_bBodyCollision = false;
					}

					iSoundCount++;
					if (3 < iSoundCount)
						iSoundCount = 0;
				}
				RELEASE_INSTANCE(CGameInstance);
			}
			
		}
	}
	

	if (1 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(1))
	{
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);

		if (m_iGreenBulletSize > m_iMaxGreenBullet)
		{
			m_iCurrentGreenBullet = m_iMaxGreenBullet;
			m_iMaxGreenBullet = 0;
		}
		else
		{
			m_iMaxGreenBullet -= (m_iGreenBulletSize - m_iCurrentGreenBullet);
			m_iCurrentGreenBullet = m_iGreenBulletSize;
		}
		m_bWeaponReload = false;
		g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Bullet_ThunderClapGloves"));
	}
}

void CPlayer_Hand::Fire_Dragon(_double TimeDelta)
{
	static _float iShootInterval = 1.f;
	if (true == m_bOil && true == m_bTalents[1])
		iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * 1.6f * (_float)TimeDelta;
	else
		iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * (_float)TimeDelta;

	if ((g_pGameInstance->Get_KeyEnter(DIK_R) || 0 >= m_iCurrentGreenBullet) && m_iCurrentGreenBullet < m_iGreenBulletSize && 0 < m_iMaxGreenBullet)
	{
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(1);
		if (nullptr != g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_Bullet_FireDragon")) && 0 < g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_Bullet_FireDragon")))
		{
			g_pGameInstance->StopSound(CSoundMgr::EFFECT4);
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Bullet_FireDragon"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CPlayer_Hand::Fire_Dragon");
				return;
			}
		}

	}
	else if (1 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
	{
		if (0 < m_iCurrentGreenBullet && g_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
		{
			m_iAnimationType = ANIM_ROOF;
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(3);

			if (nullptr == g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_Bullet_FireDragon")) || 0 == g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_Bullet_FireDragon")))
			{
				g_pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
				g_pGameInstance->LetsPlaySound(TEXT("FireDragonShot.mp3"), CSoundMgr::PLAYER_SHOT, 0.5f);
				g_pGameInstance->StopSound(CSoundMgr::EFFECT4);
				g_pGameInstance->PlayRoofSound(TEXT("FireDragonShooting.mp3"), CSoundMgr::EFFECT4, 1.f);

				CBullet_FireDragon::BULLETDESC tBulletDesc;
				tBulletDesc.pModel = m_pModelCom[WEAPON_FIREDRAGON];
				tBulletDesc.pTransform = m_pTransformCom;
				tBulletDesc.pSocketName = "muzzle";
				tBulletDesc.pDist = &m_fBulletDist;

				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Bullet_FireDragon"), TEXT("Prototype_GameObject_Bullet_FireDragon"), &tBulletDesc)))
				{
					MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Fire_Dragon");
					return;
				}
			}
		}
		else if (g_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
		{
			g_pGameInstance->StopSound(CSoundMgr::EFFECT4);
			m_iAnimationType = ANIM_ROOF;
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);

			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Bullet_FireDragon"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CPlayer_Hand::Fire_Dragon");
				return;
			}
			m_bHeadCollision = false;
			m_bBodyCollision = false;
		}
		if (nullptr != g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_Bullet_FireDragon")) && 0 < g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_Bullet_FireDragon")) && 0 < m_iCurrentGreenBullet && g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
		{
			if (1.f <= iShootInterval)
			{
				iShootInterval = 0.f;
				m_iCurrentGreenBullet--;
				m_iFire_DragonBullet = m_iCurrentGreenBullet;
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
							if (12.f > Dist)
								pBulletCollider->Get_Parent()->Set_Dead(true);
						}
					}
				}
				if (nullptr != pWeakColliderList && nullptr != pBodyColliderList)
				{
					_bool bColl = false;
					_float Dist = 0.f;
					_float MinDist = 3000.f;
					CCollider* pCollision = nullptr;
					for (auto& pWeakCollider : *pWeakColliderList)
					{
						if (true == pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pWeakCollider, &Dist))
						{
							if (12.f > Dist)
							{
								if (MinDist > Dist)
								{
									pCollision = pWeakCollider;
									MinDist = Dist;
									bColl = true;
								}
							}

						}
					}
					static int iSoundCount = 0;
					if (true == bColl && nullptr != pCollision)
					{
						if (false == m_bHeadCollision)
						{
							switch (iSoundCount)
							{
							case 0:
								pGameInstance->StopSound(CSoundMgr::EFFECT);
								pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT, 0.7f);
								break;
							case 1:
								pGameInstance->StopSound(CSoundMgr::EFFECT1);
								pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT1, 0.7f);
								break;
							case 2:
								pGameInstance->StopSound(CSoundMgr::EFFECT2);
								pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT2, 0.7f);
								break;
							case 3:
								pGameInstance->StopSound(CSoundMgr::EFFECT3);
								pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT3, 0.7f);
								break;
							}
							m_bHeadCollision = true;
						}


						CDamage::NUMBERDESC tNumberDesc;
						tNumberDesc.eType = CDamage::TYPE_CRITICAL;
						if (false == m_bUseEnergyOrb)
							tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 2.f);
						else
						{
							tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 2.f);
							m_bUseEnergyOrb = false;
						}
						tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * (MinDist - 1.f);
						_int iElement = rand() % 5;
						if (0 == iElement)
							static_cast<CMonster*>(pCollision->Get_Parent())->Set_Burning(true);

						if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pCollision->Get_Parent())->Get_Strong()))
							if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
							{
								MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Fire_Dragon");
								return;
							}

						_vector vHitPoint = XMLoadFloat4(&_float4(pCollision->Get_Sphere()->Center, 1.f)) - m_pTransformCom->Get_State(CTransform::STATE_LOOK) * pCollision->Get_Sphere()->Radius;

						_float4x4 ParentMatrix;
						XMStoreFloat4x4(&ParentMatrix, XMMatrixTranslation(vHitPoint.m128_f32[0], vHitPoint.m128_f32[1], vHitPoint.m128_f32[2]));
						CEffect::EFFECTDESC tEffectDesc;
						tEffectDesc.bNotLookCamera = false;
						tEffectDesc.eMove = CEffect::MOVE_STAND;
						tEffectDesc.eType = CEffect::TYPE_SPRITE;
						tEffectDesc.eWay = CEffect::WAY_ALPHADOWN;
						tEffectDesc.fDownAlphaSpeed = 510.f;
						tEffectDesc.fSpriteSpeed = 32.f;
						tEffectDesc.iPass = 22;
						tEffectDesc.iSpriteNumX = 4;
						tEffectDesc.iSpriteNumY = 4;
						tEffectDesc.iSpriteStartX = 0;
						tEffectDesc.iSpriteStartY = 0;
						tEffectDesc.MyWorldMatrix = XMMatrixTranslation((_float(rand() % 11) - 5.f) / 30.f, (_float(rand() % 11) - 5.f) / 30.f, 0.f);
						tEffectDesc.pDead = nullptr;
						tEffectDesc.pParentWorldMatrix = &ParentMatrix;
						tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_FireDragonBomb");
						tEffectDesc.vScale = XMVectorSet(2.f, 2.f, 2.f, 0.f);
						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Fire_Dragon");
							return;
						}
					}
					else
						m_bHeadCollision = false;

					if (false == bColl)
					{
						for (auto& pBodyCollider : *pBodyColliderList)
						{
							if (true == pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pBodyCollider, &Dist))
							{
								if (12.f > Dist)
								{
									if (MinDist > Dist)
									{
										pCollision = pBodyCollider;
										MinDist = Dist;
										bColl = true;
									}
								}
							}
						}
						if (true == bColl && nullptr != pCollision)
						{
							if (false == m_bBodyCollision)
							{
								switch (iSoundCount)
								{
								case 0:
									pGameInstance->StopSound(CSoundMgr::EFFECT);
									pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT, 0.7f);
									break;
								case 1:
									pGameInstance->StopSound(CSoundMgr::EFFECT1);
									pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT1, 0.7f);
									break;
								case 2:
									pGameInstance->StopSound(CSoundMgr::EFFECT2);
									pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT2, 0.7f);
									break;
								case 3:
									pGameInstance->StopSound(CSoundMgr::EFFECT3);
									pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT3, 0.7f);
									break;
								}
								m_bBodyCollision = true;
							}


							CDamage::NUMBERDESC tNumberDesc;
							tNumberDesc.eType = CDamage::TYPE_ORIGIN;
							if (false == m_bUseEnergyOrb)
								tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 1.f);
							else
							{
								tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 1.f);
								m_bUseEnergyOrb = false;
							}
							tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * (MinDist - 1.f);
							_int iElement = rand() % 5;
							if (0 == iElement)
								static_cast<CMonster*>(pCollision->Get_Parent())->Set_Burning(true);
							if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pCollision->Get_Parent())->Get_Strong()))
								if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
								{
									MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Fire_Dragon");
									return;
								}

							_vector vHitPoint = XMLoadFloat4(&_float4(pCollision->Get_Sphere()->Center, 1.f)) - m_pTransformCom->Get_State(CTransform::STATE_LOOK) * pCollision->Get_Sphere()->Radius;

							_float4x4 ParentMatrix;
							XMStoreFloat4x4(&ParentMatrix, XMMatrixTranslation(vHitPoint.m128_f32[0], vHitPoint.m128_f32[1], vHitPoint.m128_f32[2]));
							CEffect::EFFECTDESC tEffectDesc;
							tEffectDesc.bNotLookCamera = false;
							tEffectDesc.eMove = CEffect::MOVE_STAND;
							tEffectDesc.eType = CEffect::TYPE_SPRITE;
							tEffectDesc.eWay = CEffect::WAY_ALPHADOWN;
							tEffectDesc.fDownAlphaSpeed = 510.f;
							tEffectDesc.fSpriteSpeed = 32.f;
							tEffectDesc.iPass = 22;
							tEffectDesc.iSpriteNumX = 4;
							tEffectDesc.iSpriteNumY = 4;
							tEffectDesc.iSpriteStartX = 0;
							tEffectDesc.iSpriteStartY = 0;
							tEffectDesc.MyWorldMatrix = XMMatrixTranslation((_float(rand() % 11) - 5.f) / 30.f, (_float(rand() % 11) - 5.f) / 30.f, 0.f);
							tEffectDesc.pDead = nullptr;
							tEffectDesc.pParentWorldMatrix = &ParentMatrix;
							tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_FireDragonBomb");
							tEffectDesc.vScale = XMVectorSet(2.f, 2.f, 2.f, 0.f);
							if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
							{
								MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Fire_Dragon");
								return;
							}
						}
						else
							m_bBodyCollision = false;
					}
					iSoundCount++;
					if (3 < iSoundCount)
						iSoundCount = 0;
				}
				RELEASE_INSTANCE(CGameInstance);
			}
		}
	}


	if (1 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(1))
	{
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);

		if (m_iGreenBulletSize > m_iMaxGreenBullet)
		{
			m_iCurrentGreenBullet = m_iMaxGreenBullet;
			m_iMaxGreenBullet = 0;
		}
		else
		{
			m_iMaxGreenBullet -= (m_iGreenBulletSize - m_iCurrentGreenBullet);
			m_iCurrentGreenBullet = m_iGreenBulletSize;
		}
	}
}

void CPlayer_Hand::Piercing_Frame(_double TimeDelta)
{
	static _float iShootInterval = 1.f;
	if (true == m_bOil && true == m_bTalents[1])
		iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * 1.6f * (_float)TimeDelta;
	else
		iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * (_float)TimeDelta;

	if (2 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && false == m_bWeaponReload && 1.2 < m_pModelCom[m_eCurrentWeapon]->Get_PlayAcc())
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT4);
		g_pGameInstance->LetsPlaySound(TEXT("SnipeReload.mp3"), CSoundMgr::EFFECT4, 0.5f);
		m_bWeaponReload = true;
	}
	if ((g_pGameInstance->Get_KeyEnter(DIK_R) || 0 == m_iCurrentYellowBullet) && m_iCurrentYellowBullet < m_iYellowBulletSize && 0 < m_iMaxYellowBullet)
	{

		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(2);
		m_iAnimationType = ANIM_ONETIME;
		if (true == m_bZoom)
		{
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Snipe"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
				return;
			}
		}
		m_bZoom = false;

	}
	else if (0 < m_iCurrentYellowBullet && 2 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
	{
		m_iAnimationType = ANIM_ONETIME;

		if (1.f <= iShootInterval)
		{
			g_pGameInstance->StopSound(CSoundMgr::PLAYER_SHOT);
			g_pGameInstance->LetsPlaySound(TEXT("SnipeShot.mp3"), CSoundMgr::PLAYER_SHOT, 0.5f);
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
				_float Dist = 0.f;
				_float MinDist = 3000.f;
				CCollider* pCollision = nullptr;
				for (auto& pWeakCollider : *pWeakColliderList)
				{
					if (true == pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pWeakCollider, &Dist))
					{
						if (MinDist > Dist)
						{
							pCollision = pWeakCollider;
							MinDist = Dist;
							bColl = true;
						}
					}
				}

				if (true == bColl && nullptr != pCollision)
				{
					pGameInstance->StopSound(CSoundMgr::EFFECT);
					pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT, 0.7f);
					CDamage::NUMBERDESC tNumberDesc;
					tNumberDesc.eType = CDamage::TYPE_CRITICAL;
					if (false == m_bUseEnergyOrb)
						tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 4.f, true);
					else
					{
						tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack * 4.f, 4.f, true);
						m_bUseEnergyOrb = false;
					}
					tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * (MinDist - 1.f);
					_int iElement = rand() % 5;
					if (0 == iElement || 1 == iElement)
						static_cast<CMonster*>(pCollision->Get_Parent())->Set_Burning(true);
					if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pCollision->Get_Parent())->Get_Strong()))
						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
							return;
						}

					_vector vHitPoint = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * MinDist;
					CHierarchyNode* pNode = m_pModelCom[WEAPON_PIERCINGFRAME]->Find_HierarchyNode("muzzle");
					_vector vMuzzlePoint = (XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom[WEAPON_PIERCINGFRAME]->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix()).r[3];

					_float4x4 ParentMatrix;

					CEffect::EFFECTDESC tEffectDesc;
					tEffectDesc.eMove = CEffect::MOVE_STAND;
					tEffectDesc.eType = CEffect::TYPE_SPRITE;
					tEffectDesc.eWay = CEffect::WAY_FINAL;
					tEffectDesc.fSpriteSpeed = 16.f;
					tEffectDesc.fDownAlphaSpeed = 255.f;
					tEffectDesc.iPass = 22;
					tEffectDesc.iSpriteNumX = 4;
					tEffectDesc.iSpriteNumY = 4;
					tEffectDesc.iSpriteStartX = 0;
					tEffectDesc.iSpriteStartY = 0;
					tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
					tEffectDesc.pDead = nullptr;
					//for (_float i = 0.f; i < MinDist; i += 0.5f)
					//{					
					//	_vector vPosition = vMuzzlePoint + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * i;
					//	XMStoreFloat4x4(&ParentMatrix, XMMatrixTranslation(XMVectorGetX(vPosition), XMVectorGetY(vPosition), XMVectorGetZ(vPosition)));
					//	tEffectDesc.pParentWorldMatrix = &ParentMatrix;
					//	tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_SnipeSmoke");
					//	tEffectDesc.vScale = XMVectorSet(0.3f, 0.3f, 1.f, 0.f);
					//	if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
					//	{
					//		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand""Piercing_Frame");
					//		return;
					//	}
					//}

					XMStoreFloat4x4(&ParentMatrix, XMMatrixTranslation(vHitPoint.m128_f32[0], vHitPoint.m128_f32[1], vHitPoint.m128_f32[2]));
					/*CEffect::EFFECTDESC tEffectDesc;
					tEffectDesc.eMove = CEffect::MOVE_STAND;
					tEffectDesc.eType = CEffect::TYPE_SPRITE;
					tEffectDesc.eWay = CEffect::WAY_FINAL;
					tEffectDesc.fSpriteSpeed = 16.f;
					tEffectDesc.fDownAlphaSpeed = 255.f;
					tEffectDesc.iPass = 22;
					tEffectDesc.iSpriteNumX = 4;
					tEffectDesc.iSpriteNumY = 4;
					tEffectDesc.iSpriteStartX = 0;
					tEffectDesc.iSpriteStartY = 0;
					tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
					tEffectDesc.pDead = nullptr;*/
					tEffectDesc.pParentWorldMatrix = &ParentMatrix;
					tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_Hit_Yellow");
					tEffectDesc.vScale = XMVectorSet(3.f, 3.f, 3.f, 0.f);
					if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
					{
						MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
						return;
					}

					XMStoreFloat4(&m_vHitPosition, tNumberDesc.vPosition);
					// 피격 파티클
					CParticle_Effect::PARTICLEDESC tParticleDesc;
					tParticleDesc.iTexIndex = 33;
					tParticleDesc.fMinSpeed = 7.5f;
					tParticleDesc.fMaxSpeed = 10.f;
					tParticleDesc.fOriginScale = 0.12f;
					tParticleDesc.fScaleSpeed = 0.f;
					tParticleDesc.fGravityPower = 12.f;
					tParticleDesc.fLiveTime = 0.33f;
					tParticleDesc.fMakeRange = 0.f;
					tParticleDesc.fMaxDistance = 5.48f;
					tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
					tParticleDesc.pParentPosition = &m_vHitPosition;
					tParticleDesc.eParticleNumber = CParticle_Effect::NUM_50;
					tParticleDesc.bUseEasing = false;
					tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
					tParticleDesc.bChase = false;
					tParticleDesc.bRoof = false;
					tParticleDesc.bTrail = false;
					tParticleDesc.pDead = nullptr;
					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
						return;
					}
				}

				if (false == bColl)
				{
					for (auto& pBodyCollider : *pBodyColliderList)
					{
						if (true == pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pBodyCollider, &Dist))
						{
							if (MinDist > Dist)
							{
								pCollision = pBodyCollider;
								MinDist = Dist;
								bColl = true;
							}
						}
					}
					if (true == bColl && nullptr != pCollision)
					{
						pGameInstance->StopSound(CSoundMgr::EFFECT);
						pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT, 0.7f);
						CDamage::NUMBERDESC tNumberDesc;
						tNumberDesc.eType = CDamage::TYPE_ORIGIN;
						if (false == m_bUseEnergyOrb)
							tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 1.f, true);
						else
						{
							tNumberDesc.iDamage = static_cast<CMonster*>(pCollision->Get_Parent())->Hit(m_fAttack, 1.f, true);
							m_bUseEnergyOrb = false;
						}
						tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * (MinDist - 1.f);
						_int iElement = rand() % 5;
						if (0 == iElement || 1 == iElement)
							static_cast<CMonster*>(pCollision->Get_Parent())->Set_Burning(true);
						if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pCollision->Get_Parent())->Get_Strong()))
							if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
							{
								MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
								return;
							}

						_vector vHitPoint = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * MinDist;
						CHierarchyNode* pNode = m_pModelCom[WEAPON_PIERCINGFRAME]->Find_HierarchyNode("muzzle");
						_vector vMuzzlePoint = (XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom[WEAPON_PIERCINGFRAME]->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix()).r[3];

						CEffect::EFFECTDESC tEffectDesc;
						tEffectDesc.eMove = CEffect::MOVE_STAND;
						tEffectDesc.eType = CEffect::TYPE_SPRITE;
						tEffectDesc.eWay = CEffect::WAY_FINAL;
						tEffectDesc.fSpriteSpeed = 16.f;
						tEffectDesc.fDownAlphaSpeed = 255.f;
						tEffectDesc.iPass = 22;
						tEffectDesc.iSpriteNumX = 4;
						tEffectDesc.iSpriteNumY = 4;
						tEffectDesc.iSpriteStartX = 0;
						tEffectDesc.iSpriteStartY = 0;
						tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
						tEffectDesc.pDead = nullptr;
						tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_SnipeSmoke");
						tEffectDesc.vScale = XMVectorSet(0.3f, 0.3f, 1.f, 0.f);
						for (_float i = 0.f; i < MinDist; i += 0.5f)
						{
							_vector vPosition = vMuzzlePoint + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * i;
							_float4x4 ParentMatrix;
							XMStoreFloat4x4(&ParentMatrix, XMMatrixTranslation(XMVectorGetX(vPosition), XMVectorGetY(vPosition), XMVectorGetZ(vPosition)));					
							tEffectDesc.pParentWorldMatrix = &ParentMatrix;
							if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
							{
								MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand""Piercing_Frame");
								return;
							}
						}

						_float4x4 ParentMatrix;
						XMStoreFloat4x4(&ParentMatrix, XMMatrixTranslation(vHitPoint.m128_f32[0], vHitPoint.m128_f32[1], vHitPoint.m128_f32[2]));
						/*CEffect::EFFECTDESC tEffectDesc;
						tEffectDesc.eMove = CEffect::MOVE_STAND;
						tEffectDesc.eType = CEffect::TYPE_SPRITE;
						tEffectDesc.eWay = CEffect::WAY_FINAL;
						tEffectDesc.fSpriteSpeed = 16.f;
						tEffectDesc.fDownAlphaSpeed = 255.f;
						tEffectDesc.iPass = 22;
						tEffectDesc.iSpriteNumX = 4;
						tEffectDesc.iSpriteNumY = 4;
						tEffectDesc.iSpriteStartX = 0;
						tEffectDesc.iSpriteStartY = 0;
						tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
						tEffectDesc.pDead = nullptr;*/
						tEffectDesc.pParentWorldMatrix = &ParentMatrix;
						tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_Hit_Yellow");
						tEffectDesc.vScale = XMVectorSet(2.f, 2.f, 2.f, 0.f);
						if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
						{
							MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
							return;
						}


						XMStoreFloat4(&m_vHitPosition, tNumberDesc.vPosition);
						// 피격 파티클
						CParticle_Effect::PARTICLEDESC tParticleDesc;
						tParticleDesc.iTexIndex = 33;
						tParticleDesc.fMinSpeed = 7.5f;
						tParticleDesc.fMaxSpeed = 10.f;
						tParticleDesc.fOriginScale = 0.12f;
						tParticleDesc.fScaleSpeed = 0.f;
						tParticleDesc.fGravityPower = 12.f;
						tParticleDesc.fLiveTime = 0.33f;
						tParticleDesc.fMakeRange = 0.f;
						tParticleDesc.fMaxDistance = 5.48f;
						tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
						tParticleDesc.pParentPosition = &m_vHitPosition;
						tParticleDesc.eParticleNumber = CParticle_Effect::NUM_50;
						tParticleDesc.bUseEasing = false;
						tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
						tParticleDesc.bChase = false;
						tParticleDesc.bRoof = false;
						tParticleDesc.bTrail = false;
						tParticleDesc.pDead = nullptr;
						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
							return;
						}				
					}
				}


			}
			RELEASE_INSTANCE(CGameInstance);

			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndexChange(0);
			m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -(_float)TimeDelta * m_tCurrentWeaponInfo.fVerticalRecoil);

			_int iHorizonRecoil = rand() % 101 - 50;
			while (abs(m_iOldHorizonRecoil - iHorizonRecoil) > 50)
				iHorizonRecoil = rand() % 101 - 50;

			m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(_float)TimeDelta * _float(iHorizonRecoil) * 0.1f * m_tCurrentWeaponInfo.fHorizontalRecoil);
			m_iOldHorizonRecoil = iHorizonRecoil;
			m_fHorizonRecoilValue += (_float)TimeDelta * _float(iHorizonRecoil) * 0.1f * m_tCurrentWeaponInfo.fHorizontalRecoil;
			m_fVerticalRecoilValue += (_float)TimeDelta * m_tCurrentWeaponInfo.fVerticalRecoil;

			iShootInterval = 0.f;
			m_iCurrentYellowBullet--;
			m_iPiercing_FrameBullet = m_iCurrentYellowBullet;
		}
	}
	else if (2 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON))
	{
		m_iAnimationType = ANIM_ONETIME;

		m_bZoom = static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Camera_ZoomIn(20.f, TimeDelta * 90.0);

		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(4);
		if (false == m_bScope && true == m_bZoom)
		{
			INFO_UI tInfoUI;
			tInfoUI.fPositionX = 640.f;
			tInfoUI.fPositionY = 360.f;
			tInfoUI.fScaleX = 1000.f;
			tInfoUI.fScaleY = 1000.f;
			tInfoUI.fDepth = 5.f;
			tInfoUI.iTextureIndex = 88;
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Snipe"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
				return;
			}
			g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Snipe"))->Set_Pass(16);

			tInfoUI.fPositionX = 640.f;
			tInfoUI.fPositionY = 360.f;
			tInfoUI.fScaleX = 1280.f;
			tInfoUI.fScaleY = 720.f;
			tInfoUI.fDepth = 4.f;
			tInfoUI.iTextureIndex = 89;
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Snipe"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
				return;
			}
			g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Snipe"))->Set_Pass(16);
			m_bScope = true;
		}
	}
	else if (2 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && g_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_RBUTTON))
	{
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(1);
		if (true == m_bZoom)
		{
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Snipe"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
				return;
			}
		}
		m_bScope = false;
		m_bZoom = false;
	}

	if (!g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON))
	{
		static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Camera_ZoomOut(60.f, TimeDelta * 90.0);
		if (true == m_bZoom)
		{
			if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Snipe"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CPlayer_Hand::Piercing_Frame");
				return;
			}
		}
		m_bScope = false;
		m_bZoom = false;
	}

	if (0.f < m_fVerticalRecoilValue || 0.f < fabsf(m_fHorizonRecoilValue))	// 총 쏘는 중이 아니고 반동에 의해 에임이 위로 간 상태일 때 복구
	{
		if (0.f < m_fVerticalRecoilValue)
		{
			if ((_float)TimeDelta * 0.3f <= m_fVerticalRecoilValue)
			{
				m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (_float)TimeDelta * 0.3f);
				m_fVerticalRecoilValue -= (_float)TimeDelta * 0.3f;
			}
			else
			{
				m_pCameraTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fVerticalRecoilValue);
				m_fVerticalRecoilValue = 0.f;
			}
		}

		if (0.f < fabsf(m_fHorizonRecoilValue))
		{
			if ((_float)TimeDelta * 0.05f <= fabsf(m_fHorizonRecoilValue))
			{
				if (m_fHorizonRecoilValue > 0.f)
				{
					m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (_float)TimeDelta * 0.05f);
					m_fHorizonRecoilValue -= (_float)TimeDelta * 0.05f;
				}
				else
				{
					m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(_float)TimeDelta * 0.05f);
					m_fHorizonRecoilValue += (_float)TimeDelta * 0.05f;
				}
			}
			else
			{
				m_pCameraTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fHorizonRecoilValue);
				m_fHorizonRecoilValue = 0.f;
			}
		}
	}
	else if (0.f > m_fVerticalRecoilValue)
	{
		if (0.f > m_fVerticalRecoilValue)
			m_fVerticalRecoilValue = 0.f;
	}

	if (2 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(2))
	{
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(1);

		if (m_iYellowBulletSize > m_iMaxYellowBullet)
		{
			m_iCurrentYellowBullet = m_iMaxYellowBullet;
			m_iMaxYellowBullet = 0;
		}
		else
		{
			m_iMaxYellowBullet -= (m_iYellowBulletSize - m_iCurrentYellowBullet);
			m_iCurrentYellowBullet = m_iYellowBulletSize;
		}
		m_bWeaponReload = false;
	}
}

void CPlayer_Hand::Sword(_double TimeDelta)
{
	static _float iShootInterval = 1.f;
	if (true == m_bOil && true == m_bTalents[1])
		iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * 1.6f * (_float)TimeDelta;
	else
		iShootInterval += m_tCurrentWeaponInfo.fRPM / 60.f * (_float)TimeDelta;

	if (0 < m_iCurrentBlueBullet && 6 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
	{
		m_iAnimationType = ANIM_ONETIME;

		if (1.f <= iShootInterval)
		{
			if (1 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && 2 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
			{
				m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(1);

				CSwordTrail::SWORDTRAIL tSwordTrail;
				tSwordTrail.fAngle = 5.f;
				tSwordTrail.vAxis = XMVectorSet(0.f, 0.f, 1.f, 0.f);
				tSwordTrail.vTranslation = _float3(0.f, -0.15f, 0.f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_SwordTrail"), TEXT("Prototype_GameObject_SwordTrail"), &tSwordTrail)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in  CPlayer_Hand::Sword");
					return;
				}
				g_pGameInstance->StopSound(CSoundMgr::EFFECT1);
				g_pGameInstance->LetsPlaySound(TEXT("SwordAttack2.mp3"), CSoundMgr::EFFECT1, 0.5f);
			}
			else if (1 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
			{
				m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(2);

				CSwordTrail::SWORDTRAIL tSwordTrail;
				tSwordTrail.fAngle = 195.f;
				tSwordTrail.vAxis = XMVectorSet(0.f, 0.f, 1.f, 0.f);
				tSwordTrail.vTranslation = _float3(0.f, -0.15f, 0.f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_SwordTrail"), TEXT("Prototype_GameObject_SwordTrail"), &tSwordTrail)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in  CPlayer_Hand::Sword");
					return;
				}
				g_pGameInstance->StopSound(CSoundMgr::EFFECT2);
				g_pGameInstance->LetsPlaySound(TEXT("SwordAttack2.mp3"), CSoundMgr::EFFECT2, 0.5f);
			}
			else if (2 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
			{
				m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(7);

				CSwordTrail::SWORDTRAIL tSwordTrail;
				tSwordTrail.fAngle = 70.f;
				tSwordTrail.vAxis = XMVectorSet(0.f, 0.f, 1.f, 0.f);
				tSwordTrail.vTranslation = _float3(0.1f, -0.15f, 0.f);
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_SwordTrail"), TEXT("Prototype_GameObject_SwordTrail"), &tSwordTrail)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in  CPlayer_Hand::Sword");
					return;
				}
				g_pGameInstance->StopSound(CSoundMgr::EFFECT3);
				g_pGameInstance->LetsPlaySound(TEXT("SwordAttack3.mp3"), CSoundMgr::EFFECT3, 0.5f);
			}
			m_vCollisionMonster.clear();
			iShootInterval = 0.f;
			m_iSwordBullet = m_iCurrentBlueBullet;
		}
	}
	else if (g_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON) || 0 >= m_iCurrentBlueBullet)
	{
		if (1 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(3);
		else if (2 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(4);
		else if (7 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex())
			m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(8);
	}

	if (6 != m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && g_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_RBUTTON))
	{
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(5);
	}
	else if (5 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(5))
	{
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);
	}

	if (6 == m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex() && true == m_pModelCom[m_eCurrentWeapon]->Get_EndAnimation(6))
	{
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(0);

		if (m_iBlueBulletSize > m_iMaxBlueBullet)
		{
			m_iCurrentBlueBullet = m_iMaxBlueBullet;
			m_iMaxBlueBullet = 0;
		}
		else
		{
			m_iMaxBlueBullet -= (m_iBlueBulletSize - m_iCurrentBlueBullet);
			m_iCurrentBlueBullet = m_iBlueBulletSize;
		}
	}

	_uint iAnimIndex = m_pModelCom[m_eCurrentWeapon]->Get_CurrentAnimIndex();
	if (1 == iAnimIndex && (0.133 <= m_pModelCom[m_eCurrentWeapon]->Get_PlayAcc() && 0.2 >= m_pModelCom[m_eCurrentWeapon]->Get_PlayAcc()))
	{
		m_bSwordCollision = true;
	}
	else if (2 == iAnimIndex && (0.2 <= m_pModelCom[m_eCurrentWeapon]->Get_PlayAcc() && 0.266 >= m_pModelCom[m_eCurrentWeapon]->Get_PlayAcc()))
	{
		m_bSwordCollision = true;
	}
	else if (7 == iAnimIndex && (0.233 <= m_pModelCom[m_eCurrentWeapon]->Get_PlayAcc() && 0.3 >= m_pModelCom[m_eCurrentWeapon]->Get_PlayAcc()))
	{
		m_bSwordCollision = true;
	}
	else
		m_bSwordCollision = false;

	if (true == m_bSwordCollision)
	{

		_matrix TransformMatrix = m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		for (_uint i = 0; i < 6; i++)
		{
			m_pSwordColliderCom[i]->Update_Transform(TransformMatrix);

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
			for (auto& pWeakCollider : *pWeakColliderList)
			{
				_bool bCollisionMonster = false;
				for (auto& pCollisionMonster : m_vCollisionMonster)
				{
					if (nullptr == pCollisionMonster)
						continue;

					if (pWeakCollider->Get_Parent() == pCollisionMonster)
					{
						bCollisionMonster = true;
						break;
					}
				}
				if (true == bCollisionMonster)
					continue;

				for (_uint i = 0; i < 6; i++)
				{
					if (true == pGameInstance->Collision_Sphere(m_pSwordColliderCom[i], pWeakCollider))
					{
						pGameInstance->StopSound(CSoundMgr::EFFECT);
						pGameInstance->LetsPlaySound(TEXT("HeadShot.mp3"), CSoundMgr::EFFECT, 0.7f);
						CDamage::NUMBERDESC tNumberDesc;
						tNumberDesc.eType = CDamage::TYPE_CRITICAL;
						if (false == m_bUseEnergyOrb)
							tNumberDesc.iDamage = static_cast<CMonster*>(pWeakCollider->Get_Parent())->Hit(m_fAttack, 3.f);
						else
						{
							tNumberDesc.iDamage = static_cast<CMonster*>(pWeakCollider->Get_Parent())->Hit(m_fAttack, 3.f);
							m_bUseEnergyOrb = false;
						}
						tNumberDesc.vPosition = XMLoadFloat3(&pWeakCollider->Get_Sphere()->Center) + pWeakCollider->Get_Sphere()->Radius * -m_pTransformCom->Get_State(CTransform::STATE_LOOK) + XMVectorSet(0.f, 0.f, 0.f, 1.f);
						_int iElement = rand() % 10;
						if (0 <= iElement && 3 > iElement)
							static_cast<CMonster*>(pWeakCollider->Get_Parent())->Set_Decay(true);

						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Sword");
							return;
						}

						_vector vHitPoint = XMLoadFloat4(&_float4(pWeakCollider->Get_Sphere()->Center, 1.f)) - m_pTransformCom->Get_State(CTransform::STATE_LOOK) * pWeakCollider->Get_Sphere()->Radius;

						_float4x4 ParentMatrix;
						XMStoreFloat4x4(&ParentMatrix, XMMatrixTranslation(vHitPoint.m128_f32[0], vHitPoint.m128_f32[1], vHitPoint.m128_f32[2]));
						CEffect::EFFECTDESC tEffectDesc;
						tEffectDesc.bNotLookCamera = true;
						tEffectDesc.eMove = CEffect::MOVE_STAND;
						tEffectDesc.eType = CEffect::TYPE_2D;
						tEffectDesc.eWay = CEffect::WAY_ALPHADOWN;
						tEffectDesc.fDownAlphaSpeed = 255.f;
						tEffectDesc.iPass = 23;
						_matrix MyMatrix = m_pTransformCom->Get_WorldMatrix();
						MyMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
						if (1 == m_pModelCom[WEAPON_SWORD]->Get_CurrentAnimIndex())
							tEffectDesc.MyWorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-85.f)) * MyMatrix;
						else if (2 == m_pModelCom[WEAPON_SWORD]->Get_CurrentAnimIndex())
							tEffectDesc.MyWorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-55.f)) * MyMatrix;
						else if (7 == m_pModelCom[WEAPON_SWORD]->Get_CurrentAnimIndex())
							tEffectDesc.MyWorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-5.f)) * MyMatrix;
						//tEffectDesc.MyWorldMatrix = MyMatrix;
						tEffectDesc.pDead = nullptr;
						tEffectDesc.pParentWorldMatrix = &ParentMatrix;
						tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_HitSword");
						tEffectDesc.vScale = XMVectorSet(1.f, 6.f, 1.f, 0.f);
						if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
						{
							MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Sword");
							return;
						}

						XMStoreFloat4(&m_vHitPosition, tNumberDesc.vPosition);
						// 피격 파티클
						CParticle_Effect::PARTICLEDESC tParticleDesc;
						tParticleDesc.iTexIndex = 16;
						tParticleDesc.fMinSpeed = 7.5f;
						tParticleDesc.fMaxSpeed = 10.f;
						tParticleDesc.fOriginScale = 0.12f;
						tParticleDesc.fScaleSpeed = 0.f;
						tParticleDesc.fGravityPower = 12.f;
						tParticleDesc.fLiveTime = 0.33f;
						tParticleDesc.fMakeRange = 0.f;
						tParticleDesc.fMaxDistance = 5.48f;
						tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
						tParticleDesc.pParentPosition = &m_vHitPosition;
						tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
						tParticleDesc.bUseEasing = false;
						tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
						tParticleDesc.bChase = false;
						tParticleDesc.bRoof = false;
						tParticleDesc.bTrail = false;
						tParticleDesc.pDead = nullptr;
						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Sword");
							return;
						}

						m_vCollisionMonster.push_back(static_cast<CMonster*>(pWeakCollider->Get_Parent()));
						break;
					}
				}
			}

			for (auto& pBodyCollider : *pBodyColliderList)
			{
				_bool bCollisionMonster = false;
				for (auto& pCollisionMonster : m_vCollisionMonster)
				{
					if (nullptr == pCollisionMonster)
						continue;

					if (pBodyCollider->Get_Parent() == pCollisionMonster)
					{
						bCollisionMonster = true;
						break;
					}
				}
				if (true == bCollisionMonster)
					continue;

				for (_uint i = 0; i < 6; i++)
				{
					if (true == pGameInstance->Collision_Sphere(m_pSwordColliderCom[i], pBodyCollider))
					{
						pGameInstance->StopSound(CSoundMgr::EFFECT);
						pGameInstance->LetsPlaySound(TEXT("BodyShot.mp3"), CSoundMgr::EFFECT, 0.7f);
						CDamage::NUMBERDESC tNumberDesc;
						tNumberDesc.eType = CDamage::TYPE_ORIGIN;
						if (false == m_bUseEnergyOrb)
							tNumberDesc.iDamage = static_cast<CMonster*>(pBodyCollider->Get_Parent())->Hit(m_fAttack, 1.f);
						else
						{
							tNumberDesc.iDamage = static_cast<CMonster*>(pBodyCollider->Get_Parent())->Hit(m_fAttack * 4.f, 1.f);
							m_bUseEnergyOrb = false;
						}
						tNumberDesc.vPosition = XMLoadFloat3(&pBodyCollider->Get_Sphere()->Center) + pBodyCollider->Get_Sphere()->Radius * -m_pTransformCom->Get_State(CTransform::STATE_LOOK) + XMVectorSet(0.f, 0.f, 0.f, 1.f);
						_int iElement = rand() % 10;
						if (0 <= iElement && 3 > iElement)
							static_cast<CMonster*>(pBodyCollider->Get_Parent())->Set_Decay(true);

						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Sword");
							return;
						}

						_vector vHitPoint = XMLoadFloat4(&_float4(pBodyCollider->Get_Sphere()->Center, 1.f)) - m_pTransformCom->Get_State(CTransform::STATE_LOOK) * pBodyCollider->Get_Sphere()->Radius;

						_float4x4 ParentMatrix;
						XMStoreFloat4x4(&ParentMatrix, XMMatrixTranslation(vHitPoint.m128_f32[0], vHitPoint.m128_f32[1], vHitPoint.m128_f32[2]));
						CEffect::EFFECTDESC tEffectDesc;
						tEffectDesc.bNotLookCamera = true;
						tEffectDesc.eMove = CEffect::MOVE_STAND;
						tEffectDesc.eType = CEffect::TYPE_2D;
						tEffectDesc.eWay = CEffect::WAY_ALPHADOWN;
						tEffectDesc.fDownAlphaSpeed = 255.f;
						tEffectDesc.iPass = 23;
						_matrix MyMatrix = m_pTransformCom->Get_WorldMatrix();
						MyMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
						if (1 == m_pModelCom[WEAPON_SWORD]->Get_CurrentAnimIndex())
							tEffectDesc.MyWorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f,0.f,1.f,0.f),XMConvertToRadians(-40.f)) * MyMatrix;
						else if (2 == m_pModelCom[WEAPON_SWORD]->Get_CurrentAnimIndex())
							tEffectDesc.MyWorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-35.f)) * MyMatrix;
						else if (7 == m_pModelCom[WEAPON_SWORD]->Get_CurrentAnimIndex())
							tEffectDesc.MyWorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-5.f)) * MyMatrix;
						//tEffectDesc.MyWorldMatrix = MyMatrix;
						tEffectDesc.pDead = nullptr;
						tEffectDesc.pParentWorldMatrix = &ParentMatrix;
						tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_HitSword");
						tEffectDesc.vScale = XMVectorSet(1.f, 6.f, 1.f, 0.f);
						if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
						{
							MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Sword");
							return;
						}

						XMStoreFloat4(&m_vHitPosition, tNumberDesc.vPosition);
						// 피격 파티클
						CParticle_Effect::PARTICLEDESC tParticleDesc;
						tParticleDesc.iTexIndex = 16;
						tParticleDesc.fMinSpeed = 7.5f;
						tParticleDesc.fMaxSpeed = 10.f;
						tParticleDesc.fOriginScale = 0.12f;
						tParticleDesc.fScaleSpeed = 0.f;
						tParticleDesc.fGravityPower = 12.f;
						tParticleDesc.fLiveTime = 0.33f;
						tParticleDesc.fMakeRange = 0.f;
						tParticleDesc.fMaxDistance = 5.48f;
						tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
						tParticleDesc.pParentPosition = &m_vHitPosition;
						tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
						tParticleDesc.bUseEasing = false;
						tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
						tParticleDesc.bChase = false;
						tParticleDesc.bRoof = false;
						tParticleDesc.bTrail = false;
						tParticleDesc.pDead = nullptr;
						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Sword");
							return;
						}

						m_vCollisionMonster.push_back(static_cast<CMonster*>(pBodyCollider->Get_Parent()));
						break;
					}
				}
			}

			if (2 <= m_vCollisionMonster.size())
				int a = 0;
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CPlayer_Hand::Weapon_Info()
{
	switch (m_eCurrentWeapon)

	{
	case WEAPON_ANGELICAURA:
		m_tCurrentWeaponInfo.fVerticalRecoil = 0.2f;
		m_tCurrentWeaponInfo.fHorizontalRecoil = 0.03f;
		m_tCurrentWeaponInfo.fSpread = 0.2f;
		m_tCurrentWeaponInfo.fRPM = 1000.f;
		m_fAttack = 60.f;
		break;
	case WEAPON_TESLA:
		m_tCurrentWeaponInfo.fVerticalRecoil = 0.5f;
		m_tCurrentWeaponInfo.fHorizontalRecoil = 0.f;
		m_tCurrentWeaponInfo.fSpread = 0.03f;
		m_tCurrentWeaponInfo.fRPM = 144.f;
		m_fAttack = 500.f;
		break;
	case WEAPON_TIGERCANNON:
		m_tCurrentWeaponInfo.fVerticalRecoil = 10.f;
		m_tCurrentWeaponInfo.fHorizontalRecoil = 1.f;
		m_tCurrentWeaponInfo.fSpread = 0.02f;
		m_tCurrentWeaponInfo.fRPM = 300.f;
		m_fAttack = 1000.f;
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(1);
		break;
	case WEAPON_THUNDERCLAPGLOVES:
		m_tCurrentWeaponInfo.fVerticalRecoil = 0.1f;
		m_tCurrentWeaponInfo.fHorizontalRecoil = 0.05f;
		m_tCurrentWeaponInfo.fSpread = 0.02f;
		m_tCurrentWeaponInfo.fRPM = 1200.f;
		m_fAttack = 50.f;
		break;
	case WEAPON_FIREDRAGON:
		m_tCurrentWeaponInfo.fVerticalRecoil = 0.1f;
		m_tCurrentWeaponInfo.fHorizontalRecoil = 0.05f;
		m_tCurrentWeaponInfo.fSpread = 0.02f;
		m_tCurrentWeaponInfo.fRPM = 900.f;
		m_fAttack = 70.f;
		break;
	case WEAPON_PIERCINGFRAME:
		m_tCurrentWeaponInfo.fVerticalRecoil = 10.f;
		m_tCurrentWeaponInfo.fHorizontalRecoil = 0.3f;
		m_tCurrentWeaponInfo.fSpread = 0.02f;
		m_tCurrentWeaponInfo.fRPM = 60.f;
		m_fAttack = 1000.f;
		m_pModelCom[m_eCurrentWeapon]->SetUp_AnimIndex(1);
		break;
	case WEAPON_SWORD:
		m_tCurrentWeaponInfo.fVerticalRecoil = 0.f;
		m_tCurrentWeaponInfo.fHorizontalRecoil = 0.f;
		m_tCurrentWeaponInfo.fSpread = 0.f;
		m_tCurrentWeaponInfo.fRPM = 90.f;
		m_fAttack = 800.f;
		break;
	}
}


HRESULT CPlayer_Hand::Ready_Layer_UI_Bar(const _tchar * pLayerTag)
{
	// 체력바
	INFO_UI tInfoUI_HpBar;
	ZeroMemory(&tInfoUI_HpBar, sizeof(INFO_UI));

	tInfoUI_HpBar.fScaleX = 216.f;
	tInfoUI_HpBar.fScaleY = 128.f;
	tInfoUI_HpBar.fPositionX = 280.f;
	tInfoUI_HpBar.fPositionY = 630.f;
	tInfoUI_HpBar.fDepth = 3.f;
	tInfoUI_HpBar.iTextureIndex = 16;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_HpBar"), &tInfoUI_HpBar)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_Bar(tInfoUI_HpBar)");
		return E_FAIL;
	}

	m_pUIBar[0] = static_cast<CHpBar*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI_Bar")));
	m_pUIBar[0]->Set_Type(CHpBar::TYPE_HP);
	// 쉴드바
	INFO_UI tInfoUI_ShieldBar;
	ZeroMemory(&tInfoUI_ShieldBar, sizeof(INFO_UI));

	tInfoUI_ShieldBar.fScaleX = 216.f;
	tInfoUI_ShieldBar.fScaleY = 128.f;
	tInfoUI_ShieldBar.fPositionX = 290.f;
	tInfoUI_ShieldBar.fPositionY = 610.f;
	tInfoUI_ShieldBar.fDepth = 2.f;
	tInfoUI_ShieldBar.iTextureIndex = 16;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_HpBar"), &tInfoUI_ShieldBar)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_Bar(tInfoUI_ShieldBar)");
		return E_FAIL;
	}

	m_pUIBar[1] = static_cast<CHpBar*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI_Bar")));
	m_pUIBar[1]->Set_Type(CHpBar::TYPE_SHIELD);

	return S_OK;
}

HRESULT CPlayer_Hand::Ready_Layer_UI_Bullet(const _tchar * pLayerTag)
{
	// 초록탄
	INFO_UI tInfoUI_GreenBullet;
	ZeroMemory(&tInfoUI_GreenBullet, sizeof(INFO_UI));

	tInfoUI_GreenBullet.fScaleX = 24.f;
	tInfoUI_GreenBullet.fScaleY = 24.f;
	tInfoUI_GreenBullet.fPositionX = 1070.f;
	tInfoUI_GreenBullet.fPositionY = 570.f;
	tInfoUI_GreenBullet.fDepth = 3.f;
	tInfoUI_GreenBullet.iTextureIndex = 26;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_GreenBullet)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Readu_Layer_UI_Bullet(tInfoUI_GreenBullet)");
		return E_FAIL;
	}

	m_pBulletUI[0] = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, pLayerTag));

	// 파랑탄
	INFO_UI tInfoUI_BlueBullet;
	ZeroMemory(&tInfoUI_BlueBullet, sizeof(INFO_UI));

	tInfoUI_BlueBullet.fScaleX = 24.f;
	tInfoUI_BlueBullet.fScaleY = 24.f;
	tInfoUI_BlueBullet.fPositionX = 1090.f;
	tInfoUI_BlueBullet.fPositionY = 570.f;
	tInfoUI_BlueBullet.fDepth = 3.f;
	tInfoUI_BlueBullet.iTextureIndex = 25;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_BlueBullet)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Readu_Layer_UI_Bullet(tInfoUI_BlueBullet)");
		return E_FAIL;
	}
	m_pBulletUI[1] = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, pLayerTag));

	// 노랑탄
	INFO_UI tInfoUI_YellowBullet;
	ZeroMemory(&tInfoUI_YellowBullet, sizeof(INFO_UI));

	tInfoUI_YellowBullet.fScaleX = 24.f;
	tInfoUI_YellowBullet.fScaleY = 24.f;
	tInfoUI_YellowBullet.fPositionX = 1110.f;
	tInfoUI_YellowBullet.fPositionY = 570.f;
	tInfoUI_YellowBullet.fDepth = 3.f;
	tInfoUI_YellowBullet.iTextureIndex = 24;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_YellowBullet)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Readu_Layer_UI_Bullet(tInfoUI_YellowBullet)");
		return E_FAIL;
	}
	m_pBulletUI[2] = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, pLayerTag));

	return S_OK;
}

HRESULT CPlayer_Hand::Ready_Layer_UI_BulletNumber()
{
	CNumberUI::NUMBERDESC tNumberDesc;
	tNumberDesc.fX = 1190.f;
	tNumberDesc.fY = 580.f;

	if (WEAPON_ANGELICAURA == m_eCurrentWeapon)
	{
		tNumberDesc.eNumberType = CNumberUI::TYPE_WHITEBULLET;
		tNumberDesc.pCurrent = &m_iCurrentBullet;
		tNumberDesc.pMax = &m_iMaxBullet;
	}
	else if (WEAPON_THUNDERCLAPGLOVES == m_eCurrentWeapon || WEAPON_FIREDRAGON == m_eCurrentWeapon)
	{
		tNumberDesc.eNumberType = CNumberUI::TYPE_GREENBULLET;
		tNumberDesc.pCurrent = &m_iCurrentGreenBullet;
		tNumberDesc.pMax = &m_iMaxGreenBullet;
	}
	else if (WEAPON_TESLA == m_eCurrentWeapon || WEAPON_SWORD == m_eCurrentWeapon)
	{
		tNumberDesc.eNumberType = CNumberUI::TYPE_BLUEBULLET;
		tNumberDesc.pCurrent = &m_iCurrentBlueBullet;
		tNumberDesc.pMax = &m_iMaxBlueBullet;
	}
	else if (WEAPON_PIERCINGFRAME == m_eCurrentWeapon || WEAPON_TIGERCANNON == m_eCurrentWeapon)
	{
		tNumberDesc.eNumberType = CNumberUI::TYPE_YELLOWBULLET;
		tNumberDesc.pCurrent = &m_iCurrentYellowBullet;
		tNumberDesc.pMax = &m_iMaxYellowBullet;
	}

	SetUp_Bullet();

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_BulletNumberUI"), TEXT("Prototype_GameObject_NumberUI"), &tNumberDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_BulletNumber");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer_Hand::Ready_Layer_UI_SmokeNumber()
{
	CNumberUI::NUMBERDESC tNumberDesc;
	tNumberDesc.eNumberType = CNumberUI::TYPE_SKILL;
	tNumberDesc.fX = 940.f;
	tNumberDesc.fY = 595.f;
	tNumberDesc.pCurrent = &m_iCurrentSmoke;
	tNumberDesc.pMax = nullptr;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_SmokeNumberUI"), TEXT("Prototype_GameObject_NumberUI"), &tNumberDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_SmokeNumber");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Hand::Ready_Layer_UI_MoneyNumber()
{
	CNumberUI::NUMBERDESC tNumberDesc;
	tNumberDesc.eNumberType = CNumberUI::TYPE_COIN;
	tNumberDesc.fX = 1040.f;
	tNumberDesc.fY = 20.f;
	tNumberDesc.pCurrent = &m_iCurrentMoney;
	tNumberDesc.pMax = nullptr;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_CoinNumberUI"), TEXT("Prototype_GameObject_NumberUI"), &tNumberDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_MoneyNumber");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Hand::Ready_Layer_UI_ShieldNumber()
{
	CNumberUI::NUMBERDESC tNumberDesc;
	tNumberDesc.eNumberType = CNumberUI::TYPE_SHIELD;
	tNumberDesc.fX = 465.f;
	tNumberDesc.fY = 610.f;
	tNumberDesc.pCurrent = &m_iCurrentShield;
	tNumberDesc.pMax = &m_iMaxShield;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ShieldNumberUI"), TEXT("Prototype_GameObject_NumberUI"), &tNumberDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_ShieldNumber");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer_Hand::Ready_Layer_UI_HpNumber()
{
	CNumberUI::NUMBERDESC tNumberDesc;
	tNumberDesc.eNumberType = CNumberUI::TYPE_HP;
	tNumberDesc.fX = 462.f;
	tNumberDesc.fY = 630.f;
	tNumberDesc.pCurrent = &m_iCurrentHp;
	tNumberDesc.pMax = &m_iMaxHp;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_HpNumberUI"), TEXT("Prototype_GameObject_NumberUI"), &tNumberDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_HpNumber");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer_Hand::Ready_Layer_UI_Weapon()
{
	INFO_UI tInfoUI;
	ZeroMemory(&tInfoUI, sizeof(INFO_UI));

	tInfoUI.fScaleX = 160.f;
	tInfoUI.fScaleY = 160.f;
	tInfoUI.fPositionX = 1180.f;
	tInfoUI.fPositionY = 640.f;
	tInfoUI.fDepth = 3.f;
	tInfoUI.iTextureIndex = 54;
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_Weapon(Weapon)");
		return E_FAIL;
	}
	m_pWeaponUI = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI")));

	tInfoUI.fScaleX = 128.f;
	tInfoUI.fScaleY = 128.f;
	//tInfoUI.fPositionX = 1120.f;
	//tInfoUI.fPositionY = 700.f;
	//tInfoUI.iTextureIndex = 85;
	tInfoUI.fDepth = 3.f;
	//if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
	//{
	//	MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_Weapon(Weapon)");
	//	return E_FAIL;
	//}
	//m_pWeaponSlot[0] = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI")));

	//tInfoUI.fPositionX = 1175.f;
	//tInfoUI.fPositionY = 700.f;
	//tInfoUI.iTextureIndex = 86;
	//if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
	//{
	//	MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_Weapon(Weapon)");
	//	return E_FAIL;
	//}
	//m_pWeaponSlot[1] = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI")));

	tInfoUI.fPositionX = 1230.f;
	tInfoUI.fPositionY = 700.f;
	tInfoUI.iTextureIndex = 87;
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer_Hand::Ready_Layer_UI_Weapon(Weapon)");
		return E_FAIL;
	}
	m_pWeaponSlot[2] = static_cast<CBackGround*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_UI")));

	m_pWeaponSlot[2]->Set_Scale(160.f, 160.f);
	return S_OK;
}

void CPlayer_Hand::SetUp_Bullet()
{
	if (WEAPON_ANGELICAURA == m_eCurrentWeapon)
	{
		for (_uint i = 0; i < 3; ++i)
			m_pBulletUI[i]->Set_Scale(24.f, 24.f);
		m_iBulletSize = 32;
	}
	else if (WEAPON_THUNDERCLAPGLOVES == m_eCurrentWeapon || WEAPON_FIREDRAGON == m_eCurrentWeapon)
	{
		m_pBulletUI[0]->Set_Scale(40.f, 40.f);
		m_pBulletUI[1]->Set_Scale(24.f, 24.f);
		m_pBulletUI[2]->Set_Scale(24.f, 24.f);

		if (WEAPON_THUNDERCLAPGLOVES == m_eCurrentWeapon)
		{
			m_iGreenBulletSize = 80;
			m_iCurrentGreenBullet = m_iThunderClap_GlovesBullet;
		}
		else if (WEAPON_FIREDRAGON == m_eCurrentWeapon)
		{
			m_iGreenBulletSize = 60;
			m_iCurrentGreenBullet = m_iFire_DragonBullet;
		}
	}
	else if (WEAPON_TESLA == m_eCurrentWeapon || WEAPON_SWORD == m_eCurrentWeapon)
	{
		m_pBulletUI[0]->Set_Scale(24.f, 24.f);
		m_pBulletUI[1]->Set_Scale(40.f, 40.f);
		m_pBulletUI[2]->Set_Scale(24.f, 24.f);
		m_iBlueBulletSize = 10;
		if (WEAPON_TESLA == m_eCurrentWeapon)
			m_iCurrentBlueBullet = m_iTeslaBullet;
		else
			m_iCurrentBlueBullet = m_iSwordBullet;
	}
	else if (WEAPON_PIERCINGFRAME == m_eCurrentWeapon || WEAPON_TIGERCANNON == m_eCurrentWeapon)
	{
		m_pBulletUI[0]->Set_Scale(24.f, 24.f);
		m_pBulletUI[1]->Set_Scale(24.f, 24.f);
		m_pBulletUI[2]->Set_Scale(40.f, 40.f);
		if (WEAPON_PIERCINGFRAME == m_eCurrentWeapon)
		{
			m_iYellowBulletSize = 5;
			m_iCurrentYellowBullet = m_iPiercing_FrameBullet;
		}
		else if (WEAPON_TIGERCANNON == m_eCurrentWeapon)
		{
			m_iYellowBulletSize = 1;
			m_iCurrentYellowBullet = m_iTiger_CannonBullet;
		}
	}
}

CPlayer_Hand * CPlayer_Hand::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer_Hand*	pInstance = new CPlayer_Hand(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPlayer_Hand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Hand::Clone(void * pArg)
{
	CPlayer_Hand*	pInstance = new CPlayer_Hand(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPlayer_Hand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Hand::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	for (_uint i = 0; i < WEAPON_END; i++)
	{
		Safe_Release(m_pModelArmCom[i]);
	}
	for (_uint i = 0; i < WEAPON_END; i++)
	{
		Safe_Release(m_pModelCom[i]);

	}
	Safe_Release(m_pShaderCom);
	for (_uint i = 0; i < 6; i++)
	{
		Safe_Release(m_pSwordColliderCom[i]);

	}
	Safe_Release(m_pColliderCom);
}
