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
class CHpNumber;

class CHpBar final : public CGameObject
{
public:
	enum HP_TYPE { TYPE_HP, TYPE_SHIELD, TYPE_GUARD, TYPE_MONSTER_HP, TYPE_END };
private:
	explicit CHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHpBar(const CHpBar& rhs);
	virtual ~CHpBar() = default;
public:
	void			Set_Type(HP_TYPE eHp_Type);
	void			Set_Hp(_uint iMax, _uint iCurrent);
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

	_uint				m_iMaxHp = 100;
	_uint				m_iCurrentHp = 100;
	HP_TYPE				m_eHp_Type = TYPE_END;
private:
	HRESULT				SetUp_Components();
	HRESULT				Ready_Layer_UI_HpNumber();
	void				SetUp_Number();
public:
	static CHpBar*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END