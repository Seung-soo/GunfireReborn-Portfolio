#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Terrain;
class CNavigation;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_FILTER, TYPE_BRUSH, TYPE_END };
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;
public:
	void			Set_Dead(_bool bDead) { m_bDead = bDead; }
	void			Set_Radius(_float fRadius) { m_fRadius = fRadius; }
	void			Set_Info(_float3 vPosition, _int iTextureIndex, _int iPass);
	HRESULT			Set_Navigation(const _tchar* pPrototypeTag);
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
public:
	HRESULT			Terrain_UpDownDefault(_int iSharpness, _float fDivisibleValue, _float fMaxY, _float fMinY);
	HRESULT			Terrain_UpDownSlope(_float fSlope);
	HRESULT			Terrain_Angular();
	HRESULT			Terrain_TwoPointSlope();
	HRESULT			Light_Update();
	HRESULT			Terrain_Update();
	HRESULT			SaveY(const char* szTerrainSaveFileName) { return m_pVIBufferCom->Save_Y(szTerrainSaveFileName); }
public:
	_bool			Pick_Terrain(_float3* vMousePos);
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom[TYPE_END] = { nullptr };
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Terrain*	m_pVIBufferCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;
private:
	_int				m_iTexIndex = 5;
	INFO_TERRAIN*		m_pInfoTerrain = nullptr;
	_vector				m_vBrushPosition = XMVectorSet(-10.f, 0.f, 10.f, 1.f);
	_float				m_fRadius = 20.f;
	_bool				m_bDead = false;
	_bool				m_bFirstPoint = false;
	_bool				m_bSecondPoint = false;
	_float4				m_vFirstPos;
	_float4				m_vSecondPos;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
private:
	HRESULT Create_FilterTexture();

public:
	static CTerrain*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END