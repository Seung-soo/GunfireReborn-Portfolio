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
class CProductInfo;
class CProduct final : public CGameObject
{
public:
	typedef struct tagProductDesc
	{
		_float		fPositionX;
		_float		fPositionY;
		_int*		pCoin;
		_int		iTexIndex;
		_bool*		pBuy;
	}PRODUCTDESC;
private:
	explicit CProduct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CProduct(const CProduct& rhs);
	virtual ~CProduct() = default;
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
	_int				m_iTexIndex = 0;
	_int				m_iPrice = 0;
	_bool				m_bBuy = false;
	_bool				m_bStart = true;
	CProductInfo*		m_pProductInfo = nullptr;
private:
	PRODUCTDESC			m_tProductDesc = {};
private:
	HRESULT				SetUp_Components();
public:
	static	CProduct*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END