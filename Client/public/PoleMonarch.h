#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CCollider;
class CHierarchyNode;
class CNavigation;
class CTexture;
END

BEGIN(Client)

class CPoleMonarch final : public CMonster
{
public:
	enum STATE {
		STAND, WALK_F, RUN, WALK_B, WALK_L, WALK_R, DIE, ATTACK3_1, ATTACK1_1, TRIPLEATTACK_1, TRIPLEATTACK_3, ATTACK2_1, SHOT1, RUSH1, RUSH2,
		FIRE1, FIRE2, FIRE3, END1, X1, X2, END2, END3, BLINKATTACK1_2, ATTACK2_2, ATTACK3_2, TRIPLEATTACK_4, X4, X5, SHOT2, SHOT3, BLINK1, BLINK2, BLINKATTACK1_1,
		BLINKATTACK2_1, BLINKATTACK3_1, ATTACK1_2, BLINKATTACK2_2, X8, BLINKATTACK3_2, X9, TRIPLEATTACK_2, STATE_END
	};
private:
	CPoleMonarch(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPoleMonarch(const CPoleMonarch& rhs);
	virtual ~CPoleMonarch() = default;
public:
	HRESULT			Set_Navigation(const _tchar* pPrototypeTag);
	_bool*			Get_Phase4() { return &m_bPhase4; }
	_bool			Get_Strong() { return m_bStrong; }
	virtual void	Set_Dead(_bool bDead) { m_bDead = bDead; }
	virtual _bool	Get_Dead() { return m_bDead; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
public:
	void			Stand(_double TimeDelta);
	void			Walk_Front(_double TimeDelta);
	void			Walk_Back(_double TimeDelta);
	void			Walk_Left(_double TimeDelta);
	void			Walk_Right(_double TimeDelta);
	void			Run(_double TimeDelta);
	void			Die(_double TimeDelta);
	void			Attack1_1(_double TimeDelta);
	void			Attack1_2(_double TimeDelta);
	void			Attack2_1(_double TimeDelta);
	void			Attack2_2(_double TimeDelta);
	void			Attack3_1(_double TimeDelta);
	void			Attack3_2(_double TimeDelta);
	void			TripleAttack_1(_double TimeDelta);
	void			TripleAttack_2(_double TimeDelta);
	void			TripleAttack_3(_double TimeDelta);
	void			TripleAttack_4(_double TimeDelta);
	void			Shot1(_double TimeDelta);
	void			Shot2(_double TimeDelta);
	void			Shot3(_double TimeDelta);
	void			Rush1(_double TimeDelta);
	void			Rush2(_double TimeDelta);
	void			Fire1(_double TimeDelta);
	void			Fire2(_double TimeDelta);
	void			Fire3(_double TimeDelta);
	void			Blink1(_double TimeDelta);
	void			Blink2(_double TimeDelta);
	void			BlinkAttack1_1(_double TimeDelta);
	void			BlinkAttack1_2(_double TimeDelta);
	void			BlinkAttack2_1(_double TimeDelta);
	void			BlinkAttack2_2(_double TimeDelta);
	void			BlinkAttack3_1(_double TimeDelta);
	void			BlinkAttack3_2(_double TimeDelta);
	void			End1(_double TimeDelta);
	void			End2(_double TimeDelta);
	void			End3(_double TimeDelta);
public:
	virtual _int	Hit(_float fAttack, _float fCriticalValue, _bool bHeavy = false);
	virtual HRESULT StartSetting();
	virtual HRESULT Add_ElementIcon();
	void			IncreaseForce(_int iForce) { 
		m_iForce += iForce;
		if (80 < m_iForce)
			m_iForce = 80;
	}
private:
	CModel*				m_pModelCom = nullptr;
	vector<CCollider*>	m_BodyCollider;
	vector<CCollider*>	m_WeakCollider;
	vector<CCollider*>	m_AttackCollider;
	CNavigation*		m_pNavigationCom = nullptr;
	CTexture*			m_pTextureCom[4] = { nullptr };
private:
	STATE				m_eState = STAND;

	_bool				m_bCutScene = false;
	_uint				m_iBossPhase = 1;

	_bool				m_bCollision = true;

	_vector				m_pPlayerPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector				m_vDir = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_float				m_fTargetDistance = 0.f;
	_int				m_iShotPattern = 0;
private:
	_float				m_fRunTime = 0.f;
	_int				m_iForce = 0;

	_bool				m_bFire = false;
	_bool				m_bFirePattern = false;
	_float				m_fFireTime = 0.f;

	_bool				m_bEndPhase = false;

	_vector				m_vTripleAttackTargetPoint = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	CCollider*			m_pPlayerCollider = nullptr;

	_bool				m_bSwordPower = false;

	_int				m_iGunSize = 0;

	_bool				m_bIce = false;

	_bool				m_bStrong = false;

	_bool				m_bPhase2 = false;
	_bool				m_bPhase3 = false;

	_bool				m_bPhase4 = false;

	list<CGameObject*>	m_RockList;

	_bool				m_bSwordTrail = false;
	_float				m_fMoveUV = 0.f;
	_bool				m_bSwingSound = false;


	_float				m_fEndingTime = 0.f;
private:
	HRESULT SetUp_Components();
private:
	virtual HRESULT	State_Manage(_double TimeDelta);

private:
	HRESULT			Collider_Update();
	HRESULT			SelectCollider_Update(CHierarchyNode* pNode, vector<CCollider*> ColliderContainer, _uint iIndex);
public:
	static	CPoleMonarch*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END