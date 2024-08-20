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

class CItem_Smoke final : public CGameObject
{
public:
	typedef struct tagItemSmokeDesc
	{
		_vector	vTargetPos;
		_vector vPosition;
	}ITEMSMOKEDESC;
private:
	CItem_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CItem_Smoke(const CItem_Smoke& rhs);
	virtual ~CItem_Smoke() = default;
public:
	HRESULT			Set_Navigation(const _tchar* pPrototypeTag);
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
	CTransform*			m_pPlayerTransform = nullptr;
	CCollider*			m_pPlayerCollider = nullptr;

	_bool				m_bJump = true;
	_float				m_fJumpTime = 1.f;
	ITEMSMOKEDESC		m_tItemSmokeDesc = {};
	_vector				m_vCenterPoint = {};
	_double				m_dLerpTime = 0.0;
	_float4				m_vPosition;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT	SetUp_OnTerrain();
	_float	TerrainY(_fvector vPos);
public:
	static	CItem_Smoke*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END