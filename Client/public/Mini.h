#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CCollider;
class CHierarchyNode;
END

BEGIN(Client)

class CMini final : public CMonster
{
	enum STATE {
		STAND, UP, X0, FIRE_UP, DIE, FIRE, X1, STATE_END
	};
private:
	CMini(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMini(const CMini& rhs);
	virtual ~CMini() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
public:
	void	Up(_double TimeDelta);
	void	Stand(_double TimeDelta);
	void	Fire(double TimeDelta);
	void	Fire_Up(_double TimeDelta);
	void	Die(_double TimeDelta);
public:
	virtual _int	Hit(_float fAttack, _float fCriticalValue, _bool bHeavy = false);
	virtual HRESULT StartSetting();
	virtual HRESULT Add_ElementIcon();
	virtual void	Set_Dead(_bool bDead) { m_bDead = bDead; }
	virtual _bool	Get_Dead() { return m_bDead; }
private:
	CModel*						m_pModelCom = nullptr;
	vector<CCollider*>			m_BodyCollider;
private:
	STATE				m_eState = UP;
	_double				m_dAttackInterver = 2.0;
	_vector				m_pPlayerPosition;
	_bool				m_bFire = false;
	_bool				m_bStart = true;
private:
	HRESULT SetUp_Components();
private:
	virtual HRESULT	State_Manage(_double TimeDelta);

private:
	HRESULT			Collider_Update();
	HRESULT			SelectCollider_Update(CHierarchyNode* pNode, vector<CCollider*> ColliderContainer, _uint iIndex);
public:
	static	CMini*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END