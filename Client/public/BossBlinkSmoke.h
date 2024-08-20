#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBossBlinkSmoke final : public CGameObject
{
private:
	CBossBlinkSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBossBlinkSmoke(const CBossBlinkSmoke& rhs);
	virtual ~CBossBlinkSmoke() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	_float				m_fSprite = 0.f;
	_int				m_iSpriteY = 0;
	_int				m_iMaxSpriteX = 4;
	_int				m_iMaxSpriteY = 4;
	_float				m_fAlpha = 255.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CBossBlinkSmoke*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END