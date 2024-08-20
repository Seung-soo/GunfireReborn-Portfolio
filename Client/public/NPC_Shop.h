#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CNPC_Shop final : public CGameObject
{
private:
	CNPC_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNPC_Shop(const CNPC_Shop& rhs);
	virtual ~CNPC_Shop() = default;
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
private:
	_bool				m_bCollision = false;
	_bool				m_bBuy[3] = { false, };
	CTransform*			m_pCameraTransform = nullptr;
	CGameObject*		m_pUI = nullptr;
	_bool				m_bMakeUI = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static	CNPC_Shop*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END