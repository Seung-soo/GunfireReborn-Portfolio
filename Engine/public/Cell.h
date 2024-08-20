#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINES { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum TYPE { TYPE_NONE, TYPE_ROAD, TYPE_NOTROAD, TYPE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCell() = default;
public:
	_uint	Get_Index() { return m_iIndex; }
	_vector	Get_Point(POINTS ePoint) { return XMLoadFloat3(&m_vPoints[ePoint]); }
	_float3*	Get_Points(POINTS ePoint) { return &m_vPoints[ePoint]; }
	TYPE	Get_Type() { return m_eType; }
	_float3	Get_Line(LINES eLine) { return m_vLineDir[eLine]; }
	class CGameObject** Get_CellPoints();

	void	Set_NeighborIndex(LINES eLine, _int iNeighborIndex) { m_iNeighbor[eLine] = iNeighborIndex; }
	void	Set_CellPoint(POINTS ePoint, class CGameObject* pGameObject) { m_pCellPoint[ePoint] = pGameObject; }
	void	Set_Type(TYPE eType) { m_eType = eType; }
public:
	HRESULT NativeConstruct(const _float3* pPoints, _uint iIndex);
	_bool	Compare_Points(_fvector vSourPoint, _fvector vDestPoint);	// 두 점이 같은지 판단 인접한 삼각형 찾는 과정
	_bool	isIn(_fvector vPosition, _int* pNeighborIndex, _float3** pOut);
#ifdef _DEBUG
public:
	void	CellPointsDead();
public:
	HRESULT	Render(class CVIBuffer_Triangle* pVIBuffer, class CShader* pShader, _float4 vColor);
#endif
private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
private:
	_int						m_iNeighbor[LINE_END] = { -1,-1,-1 };
	_float3						m_vPoints[POINT_END];
	_float3						m_vLineDir[LINE_END];
	_uint						m_iIndex = 0;
	class CGameObject*			m_pCellPoint[3];

	TYPE						m_eType = TYPE_NONE;
public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END