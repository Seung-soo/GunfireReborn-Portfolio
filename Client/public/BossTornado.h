#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CBossTornado final : public CGameObject
{
private:
	CBossTornado(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBossTornado(const CBossTornado& rhs);
	virtual ~CBossTornado() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
private:
	_float				m_fAddNoiseUV = 0.f;
	_float				m_fMoveUV = 0.f;
	_int				m_iTexIndex = 0;
	_int				m_iPass = 14;
	_bool				m_bSound = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CBossTornado*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END