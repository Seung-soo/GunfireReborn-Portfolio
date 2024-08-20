#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CItem_Weapon final : public CGameObject
{
public:
	enum TYPE { TYPE_1211, TYPE_1401, TYPE_1410, TYPE_1412, TYPE_1501, TYPE_1603, TYPE_END };
	typedef struct tagItemWeaponDesc
	{
		TYPE	eType;
		_vector	vTargetPos;
		_vector vPosition;
	}ITEMWEAPONDESC;
private:
	CItem_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CItem_Weapon(const CItem_Weapon& rhs);
	virtual ~CItem_Weapon() = default;
public:
	HRESULT			Set_Navigation(const _tchar* pPrototypeTag);
	void			Set_Bullet(_int iBullet) { m_iBullet = iBullet; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
private:
	CTransform*			m_pCameraTransform = nullptr;
	_bool				m_bJump = false;
	_float				m_fJumpTime = 0.f;

	ITEMWEAPONDESC		m_tItemWeaponDesc = {};
	_vector				m_vCenterPoint = {};
	_double				m_dLerpTime = 0.0;

	_int				m_iBullet = 0;
	_bool				m_bStart = true;
	_float4				m_vPosition;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT	SetUp_OnTerrain();
	_float	TerrainY(_fvector vPos);
public:
	static	CItem_Weapon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END