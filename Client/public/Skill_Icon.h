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
class CPlayer_Hand;
class CSkill_Icon final : public CGameObject
{
private:
	explicit CSkill_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSkill_Icon(const CSkill_Icon& rhs);
	virtual ~CSkill_Icon() = default;
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
	_bool				m_bStart = true;
	INFO_UI				m_tInfoUI = {};
	CGameObject*		m_pGlow = nullptr;
	CPlayer_Hand*		m_pPlayer = nullptr;
private:
	HRESULT				SetUp_Components();
public:
	static CSkill_Icon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END