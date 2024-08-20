#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CModel;
class CCollider;
END

BEGIN(Client)

class CWave final : public CGameObject
{
private:
	CWave(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWave(const CWave& rhs);
	virtual ~CWave() = default;
public:
	HRESULT			Set_Collider();
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
private:
	_float				m_fFrustumRadius = 0.f;
	_double				m_dLiveTime = 0.0;
	_bool				m_bCollision = false;
	_float4				m_vMyPosition;
	_float4				m_vMyPosition1;
	_float4				m_vMyPosition2;
	_float4				m_vMyPosition3;
	_float4				m_vMyPosition4;
private:
	_float				m_fAddNoiseUV = 0.f;
	_int				m_iTexIndex = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CWave*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END