#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CCollider;
END

BEGIN(Client)

class CUFO final : public CMonster
{
	enum STATE {
		STAND_COMBAT, DIE, MOVE, PATROL, FIRE,  STAND, STATE_END
	};
private:
	CUFO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUFO(const CUFO& rhs);
	virtual ~CUFO() = default;

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
	CCollider*			m_pBuffCollider = nullptr;
private:
	STATE				m_eState = PATROL;
	_float				m_fPatrolRadius = 0.f;
	_vector				m_vPatrolPoint = {};
	_float				m_fTargetDistance = 0.f;
	_int				m_iBullet = 9;
	_vector				m_vTargetPos;

	_float				m_fDamageCount = 0.f;
	_float				m_fDamageInterval = 1.f;
	_bool				m_bDamage = false;
private:
	class CHpBack*			m_pHpBack = nullptr;
	class CMonsterHpBar*	m_pHpBar = nullptr;
private:
	HRESULT SetUp_Components();
private:
	virtual HRESULT	State_Manage(_double TimeDelta);

	void			Stand_Combat(_double TimeDelta);
	void			Patrol(_double TimeDelta);
	void			Fire(_double TimeDelta);
	void			Die(_double TimeDelta);
	void			Stand(_double TimeDelta);
	void			Move(_double TimeDelta);
private:
	HRESULT			Collider_Update();
public:
	static CUFO*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END