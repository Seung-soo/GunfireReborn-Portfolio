#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CCursor;
class CShopUI final : public CGameObject
{

private:
	explicit CShopUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CShopUI(const CShopUI& rhs);
	virtual ~CShopUI() = default;
public:
	virtual HRESULT		NativeConstruct_Prototype();
	virtual HRESULT		NativeConstruct(void* pArg);
	virtual _int		Tick(_double dTimeDelta);
	virtual _int		LateTick(_double dTimeDelta);
	virtual HRESULT		Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
private:
	_float4x4			m_ProjMatrix;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
private:
	_int*				m_pCoin = nullptr;
	CCursor*			m_pCursor = nullptr;
	_bool*				m_pBuy = nullptr;
	CGameObject*		m_pNumber = nullptr;
private:
	HRESULT				SetUp_Components();
public:
	static	CShopUI*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END