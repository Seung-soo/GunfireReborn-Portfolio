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

class CBackGround final : public CGameObject
{
private:
	explicit CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;
public:
	void			Set_TexIndex(_int iTexIndex) { m_iTexIndex = iTexIndex; }
	void			Set_Scale(_float fSizeX, _float fSizeY) { m_fSizeX = fSizeX; m_fSizeY = fSizeY; }
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
	_int				m_iTexIndex = 1;
	INFO_UI*			m_pInfoUI = nullptr;
private:
	HRESULT				SetUp_Components();
public:
	static CBackGround*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END