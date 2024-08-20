#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CMiniBullet final : public CGameObject
{
private:
	CMiniBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMiniBullet(const CMiniBullet& rhs);
	virtual ~CMiniBullet() = default;

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
	CCollider*			m_pColliderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
private:
	_int				m_iLiveTime = 180;
	_vector				m_vDir;
private:
	_float				m_fAddNoiseUV = 0.f;
	_float				m_fMoveUV = 0.f;
	_int				m_iTexIndex = 0;
	_float				m_fDistortionPower = 0.01f;
	_float				m_fWaterEffectTime = 0.02f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static	CMiniBullet*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END