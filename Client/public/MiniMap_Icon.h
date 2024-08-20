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

class CMiniMap_Icon final : public CGameObject
{
public:
	enum TARGET { TARGET_ARIA, TARGET_MONSTER, TARGET_PORTAL, TARGET_ENHANCE, TARGET_SHOP, TARGET_BOSS1, TARGET_BOSS2, TARGET_END };
private:
	explicit CMiniMap_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMiniMap_Icon(const CMiniMap_Icon& rhs);
	virtual ~CMiniMap_Icon() = default;
public:
	void				Set_Target(CTransform* pTransform) { m_pTargetTransform = pTransform; }
	void				Set_AriaPos(_float4* pPos) { m_pAriaPos = pPos; }
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
	_int				m_iTexIndex = 0;
	INFO_UI*			m_pInfoUI = nullptr;
	CTransform*			m_pTargetTransform = nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	_float4*			m_pAriaPos = nullptr;
private:
	HRESULT				SetUp_Components();
public:
	static	CMiniMap_Icon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END