#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Cursor.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CNavigation;
END

BEGIN(Client)
class CHpBar;
class CHpNumber;
class CPoisonSmoke;
class CPlayer final : public CGameObject
{
public:
	enum PLAYERSTATE { PLAYER_IDLE, PLAYER_WALK, PLAYER_DASH, PLAYER_END };
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
public:
	PLAYERSTATE*	Get_PlayerState() { return &m_ePlayerState; }
	_bool*			Get_Jump() { return &m_bJump; }
	_float*			Get_JumpTime() { return &m_fJumpTime; }

	HRESULT			Set_Navigation(const _tchar* pPrototypeTag);
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
private:
	PLAYERSTATE			m_ePlayerState = PLAYER_IDLE;
	_float				m_fGravity = 3.f;
	_bool				m_bJump = false;
	_float				m_fJumpTime = 0.f;
	CCursor*			m_pCursor = nullptr;

	_bool				m_bUseDash = true;
	_float				m_fDashCoolTime = 3.5f;
	_float				m_fDashMaxCoolTime = 3.5f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT	SetUp_OnTerrain();
private:
	HRESULT StateManage(_double dTimeDelta);
	HRESULT Move(_double dTimeDelta);
	HRESULT Dash(_double dTimeDelta);
	HRESULT Jump(_double dTimeDelta);
private:
	HRESULT Dash_Effect();
public:
	static CPlayer*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END