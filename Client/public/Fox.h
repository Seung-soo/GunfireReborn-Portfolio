#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CFox final : public CMonster
{
	enum STATE {
		STAND_COMBAT, FIRE_UP, PATROL, RUN, STAND, HIT_BODY,  
		JUMP_UP, JUMP_DOWN, WALK_RIGHT, WALK_BACK, WALK_LEFT, WALK_FRONT, 
		HIDE_LEFT, HIDE_RIGHT, FIRE, RELOAD, X1, HIT_HEAD, STATE_END
	};
private:
	CFox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CFox(const CFox& rhs);
	virtual ~CFox() = default;
public:
	HRESULT			Set_Navigation(const _tchar* pPrototypeTag);
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
public:
	virtual _int	Hit(_float fAttack, _float fCriticalValue, _bool bHeavy = false);
	virtual HRESULT StartSetting();
	virtual HRESULT Add_ElementIcon();
private:
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pBodyCollider = nullptr;
	CCollider*			m_pWeakCollider = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
private:
	STATE				m_eState = JUMP_DOWN;
	_float				m_fPatrolRadius = 0.f;
	_vector				m_vPatrolPoint = {};
	_float				m_fTargetDistance = 0.f;

	_double				m_dFireInterval = 0.0;
	_double				m_dWalk = 0.0;
	_int				m_iBullet = 6;
	_vector				m_vWalkDir = {};
	_float4x4			m_MuzzleMatrix;
private:
	class CHpBack*			m_pHpBack = nullptr;
	class CMonsterHpBar*	m_pHpBar = nullptr;
	class CMonsterHpBar*	m_pShieldBar = nullptr;
private:
	HRESULT SetUp_Components();
private:
	virtual HRESULT	State_Manage(_double TimeDelta);

	void			Stand_Combat(_double TimeDelta);
	void			Walk_Front(_double TimeDelta);
	void			Walk_Right(_double TimeDelta);
	void			Walk_Back(_double TimeDelta);
	void			Walk_Left(_double TimeDelta);
	void			Hide_Left(_double TimeDelta);
	void			Hide_Right(_double TimeDelta);
	void			Patrol(_double TimeDelta);
	void			Stand(_double TimeDelta);
	void			Hit_Head(_double TimeDelta);
	void			Run(_double TimeDelta);
	void			Jump_Up(_double TimeDelta);
	void			Jump_Down(_double TimeDelta);
	void			Fire_Up(_double TimeDelta);
	void			Reload(_double TimeDelta);
	void			Fire(_double TimeDelta);
	void			Hit_Body(_double TimeDelta);
private:
	HRESULT			Collider_Update();
public:
	static CFox*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END