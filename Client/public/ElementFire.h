#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CElementFire final : public CGameObject
{
public:
	enum FIRETYPE { TYPE_FIRE, TYPE_THUNDER, TYPE_POISON, TYPE_END };
private:
	CElementFire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CElementFire(const CElementFire& rhs);
	virtual ~CElementFire() = default;

public:
	void			Set_FireType(FIRETYPE eFireType) { m_eFireType = eFireType; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
private:
	_float				m_fSprite = 0.f;
	_int				m_iSpriteY = 0;
	_float				m_fAddNoiseUV = 0;
private:
	_int				m_iLiveTime = 180;
	_vector				m_vDir;
	FIRETYPE			m_eFireType = TYPE_END;
	_float4				m_vPosition;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CElementFire*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END