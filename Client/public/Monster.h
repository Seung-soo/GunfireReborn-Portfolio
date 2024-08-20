#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Item_Bullet.h"
#include "Item_Coin.h"
#include "Item_Weapon.h"
#include "Item_Smoke.h"
#include "MiniMap_Icon.h"
#include "ElementIcon.h"
#include "Chain.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CMonster abstract : public CGameObject
{
public:
	typedef struct tagStatus
	{
		_float fMaxHp = 3000.f;
		_float fCurrentHp = 3000.f;
		_float fMaxShield = 0.f;
		_float fCurrentShield = 0.f;

		_float fAttack = 1.f;
		_float fDefend = 0.f;
	}STATUS;
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;
public:
	void			Set_UIRender(_bool bUIRender) { m_bUIRender = bUIRender; }
	void			Set_Buff(_bool bBuff);
	void			Set_Burning(_bool bBurning) { m_bBurning = bBurning; }
	void			Set_Shock(_bool bShock) { m_bShock = bShock; }
	void			Set_Decay(_bool bDecay) { m_bDecay = bDecay; }
	void			Set_Combustion(_bool bCombution) { m_bCombustion = bCombution; }
	void			Set_Manipulation(_bool bManipulation) { m_bManipulation = bManipulation; }
	void			Set_Miasma(_bool bMiasma) { m_bMiasma = bMiasma; }
	void			Set_DropWeapon(_bool bDropWeapon, _uint iWeaponIndex) { m_bDropWeapon = bDropWeapon; m_iDropWeaponIndex = iWeaponIndex; }

	virtual void	Set_Dead(_bool bDead) override { m_bDissolve = bDead; }
	virtual _bool	Get_Dead() override { return m_bDissolve; }

	STATUS*			Get_Status() { return &m_tStatus; }
	_bool			Get_Buff() { return m_bBuff; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
public:
	virtual _int	Hit(_float fAttack, _float fCriticalValue, _bool bHeavy = false) = 0;
	virtual HRESULT	StartSetting() = 0;
	void			Freeze(_float fFreezeTime);
	void			Element(_double TimeDelta);
	void			Burning(_double TimeDelta);
	void			Shock(_double TimeDelta);
	void			Decay(_double TimeDelta);
	void			Combustion(_double TimeDelta);
	void			Manipulation(_double TimeDelta);
	void			Miasma(_double TimeDelta);
	virtual HRESULT Add_ElementIcon() = 0;
protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pDissolveTextureCom = nullptr;
protected:
	CTransform*			m_pPlayerTransform = nullptr;
	CMonster*			m_pTargetMonster = nullptr;
	_bool				m_pTargetFind = false;
	_uint				m_iAnimationType = ANIM_ONETIME;
	_bool				m_bUIRender = false;

	_bool				m_bBuff = false;
	_bool				m_bIsBoss = false;
protected:
	_bool				m_bBurning = false;
	_bool				m_bShock = false;
	_bool				m_bDecay = false;
	_bool				m_bCombustion = false;
	_bool				m_bManipulation = false;
	_bool				m_bMiasma = false;

	_float				m_fBurningTime = 0.f;
	_float				m_fShockTime = 0.f;
	_float				m_fDecayTime = 0.f;
	_float				m_fCombustionTime = 0.f;
	_float				m_fManipulationTime = 0.f;
	_float				m_fMiasmaTime = 0.f;

	_bool				m_bBurningStart = true;
	_bool				m_bShockStart = true;
	_bool				m_bDecayStart = true;
	_bool				m_bCombustionStart = true;
	_bool				m_bManipulationStart = true;
	_bool				m_bMiasmaStart = true;

	_float				m_fDotDealTime = 1.f;
	_bool				m_bFreeze = false;
	_float				m_fFreezeTime = 0.f;
	_float				m_fCurrentFreezeTime = 0.f;
	_bool				m_bChain = false;

	_float				m_fSpeed = 1.f;
	_int				m_iBurningEffect = 0;
	_int				m_iShockEffect = 0;
	_int				m_iDecayEffect = 0;
	_bool				m_bDissolve = false;
	_float				m_fDissolveAlpha = 0.f;

	_bool				m_bDropItems = false;
	_uint				m_iDropWeaponIndex = 0;
	_bool				m_bDropWeapon = false;
protected:
	STATUS				m_tStatus = {};
	CMiniMap_Icon*		m_pMiniMap_Icon = nullptr;
	list<CMonster*>*	m_pMonsterList = nullptr;
protected:
	HRESULT SetUp_ConstantTable();
	HRESULT	SetUp_OnTerrain();
	void	Element_Reset();
protected:
	virtual HRESULT	State_Manage(_double TimeDelta) = 0;
public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END