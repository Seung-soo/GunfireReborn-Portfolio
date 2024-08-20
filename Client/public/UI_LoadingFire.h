#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class CTransform;
END

BEGIN(Client)

class CUI_LoadingFire final : public CGameObject
{
private:
	explicit CUI_LoadingFire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_LoadingFire(const CUI_LoadingFire& rhs);
	virtual ~CUI_LoadingFire() = default;
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
	_float				m_fTexIndex = 0;
private:
	HRESULT				SetUp_Components();
public:
	static CUI_LoadingFire*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END