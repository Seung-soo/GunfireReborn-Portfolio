#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CBear final : public CMonster
{
	enum STATE {
		STAND_COMBAT, PATROL, RUN, STAND, HIT_BODY, JUMP_UP, 
		JUMP_DOWN, STAND_ATTACK, RUN_ATTACK, HIDE_LEFT, HIDE_RIGHT, HIT_HEAD,
		STATE_END
	};
private:
	CBear(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBear(const CBear& rhs);
	virtual ~CBear() = default;
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
	CCollider*			m_pAttackCollider[2] = { nullptr,};
	CNavigation*		m_pNavigationCom = nullptr;
private:
	STATE				m_eState = JUMP_DOWN;
	_float				m_fPatrolRadius = 0.f;
	_vector				m_vPatrolPoint = {};
	_float				m_fTargetDistance = 0.f;
	_bool				m_bAttack = false;
	_bool				m_bAttackCollision = false;
private:
	class CHpBack*			m_pHpBack = nullptr;
	class CMonsterHpBar*	m_pHpBar = nullptr;
	class CMonsterHpBar*	m_pShieldBar = nullptr;
private:
	HRESULT SetUp_Components();
private:
	virtual HRESULT	State_Manage(_double TimeDelta);

	void			Stand_Combat(_double TimeDelta);
	void			Patrol(_double TimeDelta);
	void			Stand(_double TimeDelta);
	void			Hit_Head(_double TimeDelta);
	void			Run(_double TimeDelta);
	void			Jump_Up(_double TimeDelta);
	void			Jump_Down(_double TimeDelta);
	void			Hide_Right(_double TimeDelta);
	void			Hide_Left(_double TimeDelta);
	void			Run_Attack(_double TimeDelta);
	void			Stand_Attack(_double TimeDelta);
	void			Hit_Body(_double TimeDelta);
private:
	HRESULT			Collider_Update();
public:
	static CBear*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END