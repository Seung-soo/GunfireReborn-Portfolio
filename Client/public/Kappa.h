#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CKappa final : public CMonster
{
	enum STATE {		
		  FIRE_UP, FIRE_SHOT, FIRE_SHOT2,  FIRE_DOWN, HIT_BODY, HIT_HEAD, 
		  JUMP_UP, JUMP_DOWN, PATROL, RUN, STAND, STAND_COMBAT, 
		  WALK_BACK, WALK_FRONT, WALK_LEFT, WALK_RIGHT, STATE_END
	};
private:
	CKappa(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CKappa(const CKappa& rhs);
	virtual ~CKappa() = default;
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
	virtual HRESULT	StartSetting();
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

	_bool				m_bJump = false;
	_bool				m_bScare = false;
	vector<class CKappaFire*>	m_vKappaFire;
	_int				m_iFireIndex = 0;
	_int				m_iRunDirReset = 0;
private:
	class CHpBack*			m_pHpBack = nullptr;
	class CMonsterHpBar*	m_pHpBar = nullptr;
private:
	HRESULT SetUp_Components();
private:
	virtual HRESULT	State_Manage(_double TimeDelta);

	void			Stand_Combat(_double TimeDelta);
	void			Walk_Front(_double TimeDelta);
	void			Walk_Right(_double TimeDelta);
	void			Walk_Back(_double TimeDelta);
	void			Walk_Left(_double TimeDelta);
	void			Patrol(_double TimeDelta);
	void			Stand(_double TimeDelta);
	void			Hit_Head(_double TimeDelta);
	void			Run(_double TimeDelta);
	void			Jump_Up(_double TimeDelta);
	void			Jump_Down(_double TimeDelta);
	void			Fire_Up(_double TimeDelta);
	void			Fire_Shot(_double TimeDelta);
	void			Fire_Shot2(_double TimeDelta);
	void			Fire_Down(_double TimeDelta);
	void			Hit_Body(_double TimeDelta);
private:
	HRESULT			Collider_Update();
public:
	static CKappa*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END