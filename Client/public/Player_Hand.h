#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)
class CHpBar;
class CHpNumber;
class CPoisonSmoke;
class CBackGround;
class CMonster;
class CBackGround;
class CCamera_Dynamic;
class CPlayer_Hand final : public CGameObject
{
public:
	enum WEAPON { WEAPON_SKILL, WEAPON_ANGELICAURA, WEAPON_TESLA, WEAPON_TIGERCANNON
				, WEAPON_THUNDERCLAPGLOVES, WEAPON_FIREDRAGON
				, WEAPON_PIERCINGFRAME, WEAPON_SWORD, WEAPON_END };

	typedef struct tagWeaponInfo
	{
		_float	fRPM = 0.f;
		_float	fVerticalRecoil = 0.f;
		_float	fHorizontalRecoil = 0.f;
		_float	fSpread = 0.f;
	}WEAPONINFO;
private:
	CPlayer_Hand(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayer_Hand(const CPlayer_Hand& rhs);
	virtual ~CPlayer_Hand() = default;
public:
	WEAPON			Get_Slot1() { return m_eSlot1; }
	WEAPON			Get_Slot2() { return m_eSlot2; }
	_bool*			Get_Talents() { return m_bTalents; }
	_int*			Get_Money() { return &m_iCurrentMoney; }
	_bool			Get_UseEnergyOrb() { return m_bUseEnergyOrb; }
	_bool			EatGreenBulletPossible() { return m_iMaxGreenBullet >= m_iTopMaxGreenBullet; }
	_bool			EatBlueBulletPossible() { return m_iMaxBlueBullet >= m_iTopMaxBlueBullet; }
	_bool			EatYellowBulletPossible() { return m_iMaxYellowBullet >= m_iTopMaxYellowBullet; }
	_bool			EatSmokePossible() { return m_iCurrentSmoke >= m_iMaxSmoke; }

	void			Set_Money(_int iMoney) { m_iCurrentMoney += iMoney; }
	void			Set_Smoke() { m_iCurrentSmoke++; }
	void			Set_MaxSmoke() { m_iMaxSmoke = 9; }
	void			Set_GreenBullet(_int iBullet) { 
		m_iMaxGreenBullet += iBullet; 
		if (m_iMaxGreenBullet > m_iTopMaxGreenBullet)
			m_iMaxGreenBullet = m_iTopMaxGreenBullet;
	}
	void			Set_BlueBullet(_int iBullet) {
		m_iMaxBlueBullet += iBullet; 
		if (m_iMaxBlueBullet > m_iTopMaxBlueBullet)
			m_iMaxBlueBullet = m_iTopMaxBlueBullet;
	}
	void			Set_YellowBullet(_int iBullet) { 
		m_iMaxYellowBullet += iBullet; 
		if (m_iMaxYellowBullet > m_iTopMaxYellowBullet)
			m_iMaxYellowBullet = m_iTopMaxYellowBullet;
	}
	void			Set_Weapon(WEAPON eWeapon, _int iBullet);
	void			Set_Oil() { m_bOil = true; }
	void			Set_CoolTime() { m_fSkillMaxCoolTime = 10; }
	void			Set_UseEnergyOrb() { m_bUseEnergyOrb = false; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
public:
	void	Hit(_float fAttack);
	void	HitAndSlow(_float fAttack, _float fSlow);
	void	HealHp();
	void	FullBullet();
	void	FullSmoke();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom[WEAPON_END] = { nullptr, };
	CModel*				m_pModelArmCom[WEAPON_END] = {nullptr,};
	CCollider*			m_pSwordColliderCom[6] = { nullptr ,};
	CCollider*			m_pColliderCom = nullptr;
private:
	_uint					m_iAnimationType = ANIM_ONETIME;

	CPlayer::PLAYERSTATE*	m_pPlayerState = nullptr;
	_bool*					m_pPlayerJump = nullptr;
	_float*					m_pPlayerJumpTime = nullptr;

	CCamera_Dynamic*		m_pCamera = nullptr;

	_float					m_fBreathCount = 0.f;
	_float					m_fBreathCycle = 15.f;
	_float					m_fBreathPower = 0.01f;

	_float					m_fJumpBreathDegree = 0.f;

	_float					m_fSkillMaxCoolTime = 13.f;
	_float					m_fSkillCurrentCoolTime = 13.f;
	_bool					m_bUseSkill = true;

	WEAPONINFO				m_tCurrentWeaponInfo = {};
	WEAPON					m_eCurrentWeapon = WEAPON_END;
	WEAPON					m_eNextWeapon = WEAPON_END; 
	WEAPON					m_eSlot1 = WEAPON_END;
	WEAPON					m_eSlot2 = WEAPON_END;
	_bool					m_bSlot1Click = false;
	_bool					m_bSlot2Click = false;
	_bool					m_bSwapWeapon = false;
	_uint					m_iSwapTime = 0;
	_uint					m_iCurrentWeaponIndex = 3;

	CTransform*				m_pCameraTransform = nullptr;
	_float					m_fVerticalRecoilValue = 0.f;
	_float					m_fHorizonRecoilValue = 0.f;
	_int					m_iOldHorizonRecoil = 0;

	_bool					m_bZoom = false;
private:
	_int					m_iCurrentShield = 100;
	_int					m_iMaxShield = 100;
	_int					m_iCurrentHp = 100;
	_int					m_iMaxHp = 100;
	CHpBar*					m_pUIBar[2];

	_int					m_iCurrentBullet = 32;
	_int					m_iBulletSize = 32;
	_int					m_iMaxBullet = 99999;


	_int					m_iCurrentGreenBullet = 80;
	_int					m_iGreenBulletSize = 80;
	_int					m_iMaxGreenBullet = 320;
	_int					m_iTopMaxGreenBullet = 675;

	_int					m_iCurrentBlueBullet = 10;
	_int					m_iBlueBulletSize = 10;
	_int					m_iMaxBlueBullet = 80;
	_int					m_iTopMaxBlueBullet = 160;

	_int					m_iCurrentYellowBullet = 2;
	_int					m_iYellowBulletSize = 2;
	_int					m_iMaxYellowBullet = 20;
	_int					m_iTopMaxYellowBullet = 45;

	_int					m_iCurrentSmoke = 3;
	_int					m_iMaxSmoke = 3;


	_int					m_iCurrentMoney = 0;


	CPoisonSmoke*			m_pPoisonSmoke = nullptr;
	CBackGround*			m_pWeaponUI = nullptr;

	_float					m_fBulletDist = 30.f;

	_float					m_fAttack = 0.f;

	_bool					m_bSlow = false;
	_double					m_dSlowTime = 0.0;

	CBackGround*			m_pBulletUI[3] = { nullptr, };
	_int					m_iTeslaBullet = 10;
	_int					m_iTiger_CannonBullet = 1;
	_int					m_iThunderClap_GlovesBullet = 80;
	_int					m_iFire_DragonBullet = 60;
	_int					m_iPiercing_FrameBullet = 5;
	_int					m_iSwordBullet = 1;
private:
	vector<CMonster*>		m_vCollisionMonster;
private:
	_bool					m_bSwordCollision = false;
private:
	_float4x4				m_MuzzleMatrix;
	_float4x4				m_CannonMuzzleMatrix;
	_float4x4				m_GloveLogoMatrix;
	_float4x4				m_GloveMuzzleMatrix;
	_float4x4				m_TeslaMuzzleMatrix;

	_bool					m_bNotGlove = true;
	CGameObject*			m_pGloveLogo = nullptr;
	_bool					m_bScope = false;

	_float4					m_vHitPosition = _float4(0.f, 0.f, 0.f, 1.f);
private:
	_bool					m_bTalents[6] = { false, };
	CBackGround*			m_pWeaponSlot[3] = { nullptr, };
	_bool					m_bOil = false;
	_float					m_fOilTime = 0.f;
	_bool					m_bPlusShield = false;
	_float					m_fPlusShieldTime = 0.f;
	_bool					m_bPowerEnergyOrb = false;
	_bool					m_bUseEnergyOrb = false;

	_float					m_fHitTime = 7.f;
	_float					m_fHealShieldInterval = 0.05f;
	_bool					m_bHeadCollision = false;
	_bool					m_bBodyCollision = false;
	_bool					m_bWeaponReload = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
private:
	void	Breath(_double TimeDelta);
	void	HealingShield(_double TimeDelta);
	void	WeaponKey(_double TimeDelta);
	void	SwapWeapon(_double TimeDelta);

	void	Skill(_double TimeDelta);
	void	Angelic_Aura(_double TimeDelta);
	void	Tesla(_double TimeDelta);
	void	Tiger_Cannon(_double TimeDelta);
	void	ThunderClap_Gloves(_double TimeDelta);
	void	Fire_Dragon(_double TimeDelta);
	void	Piercing_Frame(_double TimeDelta);
	void	Sword(_double TimeDelta);

	void	Weapon_Info();
private:
	HRESULT Ready_Layer_UI_Bar(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI_Bullet(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_UI_BulletNumber();
	HRESULT	Ready_Layer_UI_SmokeNumber();
	HRESULT	Ready_Layer_UI_MoneyNumber();
	HRESULT	Ready_Layer_UI_ShieldNumber();
	HRESULT	Ready_Layer_UI_HpNumber();
	HRESULT	Ready_Layer_UI_Weapon();
	void	SetUp_Bullet();
public:
	static CPlayer_Hand*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END