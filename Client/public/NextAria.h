#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CNavigation;
class CCell;
END

BEGIN(Client)

class CNextAria final : public CGameObject
{
public:
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CNextAria(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNextAria(const CNextAria& rhs);
	virtual ~CNextAria() = default;

public:
	virtual HRESULT		NativeConstruct_Prototype();
	virtual HRESULT		NativeConstruct(void* pArg);
	virtual _int		Tick(_double TimeDelta);
	virtual _int		LateTick(_double TimeDelta);
	virtual HRESULT		Render();
public:
	HRESULT				WallSetting(_uint iCellIndex, LINE eLine, _uint iAriaIndex);
	HRESULT				Push_Cell(_uint iCellIndex);
	HRESULT				Restore();
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	CNavigation*		m_pPlayerNavigation = nullptr;
	_vector				m_vPoint[4] = {};
	CCell*				m_pCell = nullptr;
	vector<CCell*>		m_Cells;
	CTransform*			m_pPlayerTransform = nullptr;
	_vector				m_vPlayerPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector				m_vOldPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_uint				m_iAriaIndex = 0;

	_vector				m_vWorldPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_matrix				m_WorldMatrixInv;
	_vector				m_vViewPortPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_float				m_fSizeX = 32.f;
	_float				m_fSizeY = 32.f;
	_float4				m_vWorldPos = {};
	const _float		m_fHalfSize = 16.f;

	class CMiniMap_Icon* m_pMiniMap_Icon = nullptr;
private:
	_float4x4			m_ProjMatrix;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_OnTerrain();
	void	SetUp_ViewPortPos();
public:
	static CNextAria*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END