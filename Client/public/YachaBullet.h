#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CYachaBullet final : public CGameObject
{
private:
	CYachaBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CYachaBullet(const CYachaBullet& rhs);
	virtual ~CYachaBullet() = default;
public:
	void			Set_Manipulation(CGameObject* pTarget);
	void			Set_OutMesh() { m_iTexIndex = 3; m_iPass = 9; 	m_pTransformCom->Scaled(XMVectorSet(1.2f, 5.f, 5.f, 0.f)); }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
private:
	_int				m_iLiveTime = 180;
	_vector				m_vDir;
	_bool				m_bManipulation = false;
	CGameObject*		m_pTarget = nullptr;
	_float				m_fMoveUV = 0.f;
	_uint				m_iTexIndex = 50;
	_float4				m_vPosition;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CYachaBullet*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END