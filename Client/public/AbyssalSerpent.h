#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CCollider;
class CHierarchyNode;
class CTexture;
END

BEGIN(Client)

class CAbyssalSerpent final : public CMonster
{
public:
	enum STATE {
		RUSH4, RUSH3, RUSH2, RUSH1, WATERBALL, 
		X0, X1, 
		STAND, DIE, 
		X3, WAITING, X4, X5, X6, X7, X8,
		UP, SWEEP3, SWEEP2, SWEEP1, 
		X9, X10, X11, STATE_END
	};
	enum TYPE { TYPE_REAL, TYPE_COPY, TYPE_ATTACK, TYPE_END };
private:
	CAbyssalSerpent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CAbyssalSerpent(const CAbyssalSerpent& rhs);
	virtual ~CAbyssalSerpent() = default;
public:
	void			Set_Type(TYPE eType, STATUS* pRealStatus = nullptr);
	STATE			Get_State() { return m_eState; }
	virtual void	Set_Dead(_bool bDead) { m_bDead = bDead; }
	virtual _bool	Get_Dead() { return m_bDead; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
public:
	void	Up(_double TimeDelta);
	void	Stand(_double TimeDelta);
	void	Waiting(_double TimeDelta);
	void	Rush1(_double TimeDelta);
	void	Rush2(_double TimeDelta);
	void	Rush3(_double TimeDelta);
	void	Rush4(_double TimeDelta);
	void	Sweep1(_double TimeDelta);
	void	Sweep2(_double TimeDelta);
	void	Sweep3(_double TimeDelta);
	void	WaterBall(_double TimeDelta);
	void	Die(_double TimeDelta);
public:
	virtual _int	Hit(_float fAttack, _float fCriticalValue, _bool bHeavy = false);
	virtual HRESULT StartSetting();
	virtual HRESULT Add_ElementIcon();
private:
	CModel*						m_pModelCom = nullptr;
	CTexture*					m_pMaskTextureCom = nullptr;
	vector<CCollider*>			m_BodyCollider;
	vector<CCollider*>			m_WeakCollider;
private:
	STATE				m_eState = UP;

	_bool				m_bCutScene = false;
	_bool				m_bStart = true;
	_double				m_dWaitTime = 0.0;
	_uint				m_iBossPhase = 1;

	_bool				m_bRender = true;
	_bool				m_bCollision = true;
	
	_vector				m_vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);

	_bool				m_bMakeUI = true;

	TYPE				m_eType = TYPE_END;
	STATUS*				m_pRealStatus = nullptr;

	_bool				m_bCopyCall = false;
	_bool				m_bFire = false;
	_bool				m_bGroupAttack = false;
	_double				m_dGroupAttackTime = 0.0;
	_bool				m_bGroup = false;


	_bool				m_bCameraScene = false;
	_double				m_dCameraSlow = 1.0;

	_bool				m_bWaterEffect = false;
	_bool				m_bWaterBallWarning = false;
	_bool				m_bReadyWaterBall = false;

	_float				m_fWaveTime = 0.f;
	_float				m_fWaveInterval = 0.5f;
	_float				m_fMoveWaveInterval = 0.f;
	_bool				m_bFarAttack = false;
	_bool				m_bMakeWave = true;
	_bool				m_bWaveSound = false;
	CAbyssalSerpent*	m_pReal = nullptr;
private:
	HRESULT SetUp_Components();
private:
	virtual HRESULT	State_Manage(_double TimeDelta);

private:
	HRESULT			Collider_Update();
	HRESULT			SelectCollider_Update(CHierarchyNode* pNode, vector<CCollider*> ColliderContainer, _uint iIndex);
public:
	static CAbyssalSerpent*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END