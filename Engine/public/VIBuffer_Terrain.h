#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	explicit CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;
public:
	HRESULT	NativeConstruct_Prototype(_uint iNumX, _uint iNumZ);
	HRESULT			NativeConstruct_Prototype(const _tchar* pHeightMap);
	virtual HRESULT NativeConstruct(void* pArg) override;
public:
	HRESULT			MakeTerrainDefault(_float4 vBrushPos, _float fBrushRadius, _int iSharpness, _float fDivisibleValue, _float fMaxY, _float fMinY);
	HRESULT			MakeTerrainSlope(_float4 vBrushPos, _float fBrushRadius, _float fSlope);
	HRESULT			MakeTerrainAngular(_float4 vBrushPos, _float fBrushRadius);
	HRESULT			MakeTerrainTwoPointSlope(_float4 vFirstPoint, _float4 vSecondPoint, _float fBrushRadius);
	HRESULT			MakeLight();
	HRESULT			Change_Terrain(_uint iNumVerticesX, _uint iNumVerticesZ, _float fTileNumX, _float fTileNumZ);
	_float			Compute_Y(const _float3& vPoint);
private:
	_float			m_fTileNumX = 30.f;
	_float			m_fTileNumZ = 30.f;
public:
	HRESULT			Save_Y(const char* szTerrainSaveFileName);
public:
	static CVIBuffer_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumX = 129, _uint iNumZ = 129);
	static CVIBuffer_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pHeightMap);
	virtual CComponent*			Clone(void* pArg);
	virtual void				Free() override;
};

END