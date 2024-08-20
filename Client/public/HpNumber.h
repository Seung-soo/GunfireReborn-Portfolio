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

class CHpNumber final : public CGameObject
{
public:
	enum NUMCOLOR { COLOR_WHITE, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_END };
	typedef struct tagNumInfo
	{
		_float		fX, fY;
		_float		fSizeX, fSizeY;
		NUMCOLOR	eNumColor;
		_float*		pParentX;
		_float*		pParentY;
		_bool		bStopUI = false;
	}NUMINFO;
private:
	explicit CHpNumber(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHpNumber(const CHpNumber& rhs);
	virtual ~CHpNumber() = default;
public:
	void			Set_TexIndex(_int iTexIndex) { m_iTexIndex = iTexIndex; }
	void			Set_Position(_float fX, _float fY) { m_tNumInfo.fX += fX; m_tNumInfo.fY += fY; }
	void			Set_Size(_float fSizeX, _float fSizeY) { m_tNumInfo.fSizeX += fSizeX; m_tNumInfo.fSizeY += fSizeY; }
	void			Set_Color(NUMCOLOR eNumColor) { m_tNumInfo.eNumColor = eNumColor; }
	void			Set_Render(_bool bRender) { m_bRender = bRender; }
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
	CTexture*			m_pTextureCom[COLOR_END] = { nullptr, };
private:
	_float4x4			m_ProjMatrix;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
private:
	_int				m_iTexIndex = 1;
	NUMINFO				m_tNumInfo = {};
	_bool				m_bRender = true;
private:
	HRESULT				SetUp_Components();
public:
	static	CHpNumber*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END