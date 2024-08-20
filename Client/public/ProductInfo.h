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

class CProductInfo final : public CGameObject
{
public:
	typedef struct tagProductInfoDesc
	{
		_int*	pCoin = nullptr;
		_bool*	pBuy = nullptr;
		_int*	pTexIndex = nullptr;
		_int*	pPrice = nullptr;
	}PRODUCTINFODESC;
private:
	explicit CProductInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CProductInfo(const CProductInfo& rhs);
	virtual ~CProductInfo() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
private:
	_float4x4			m_ProjMatrix;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
private:
	_uint				m_iTexIndex = 0;
	PRODUCTINFODESC		m_tProductInfoDesc;
private:
	HRESULT				SetUp_Components();
public:
	static CProductInfo*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END