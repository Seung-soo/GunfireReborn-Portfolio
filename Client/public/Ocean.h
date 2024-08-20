#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class COcean final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_FILTER, TYPE_BRUSH, TYPE_END };
private:
	COcean(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	COcean(const COcean& rhs);
	virtual ~COcean() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom[TYPE_END] = { nullptr };
	CTexture*			m_pNoiseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Terrain*	m_pVIBufferCom = nullptr;
private:
	_int				m_iTexIndex = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static COcean*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END