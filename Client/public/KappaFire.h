#pragma once

#include "Client_Defines.h"
#include "ElementFire.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CKappaFire final : public CGameObject
{
private:
	CKappaFire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CKappaFire(const CKappaFire& rhs);
	virtual ~CKappaFire() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
public:
	void				Go_Fire(CElementFire::FIRETYPE eFireType);
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	_float				m_fSprite = 0.f;
	_int				m_iSpriteY = 0;
	_float				m_fAddNoiseUV = 0;
	_int				m_iTexIndex = 0;
	_float4				m_vPosition;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CKappaFire*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END