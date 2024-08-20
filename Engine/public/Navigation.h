#pragma once

#include "Component.h"
#include "Cell.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNaviDesc
	{
		class CTransform* pParent = nullptr;
		_uint	iCurrentIndex;
	}NAVIDESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;
public:
	CCell::TYPE		Get_Type() { return m_Cells[m_NaviDesc.iCurrentIndex]->Get_Type(); }
	CCell*			Get_Cell(_uint iIndex) { return m_Cells[iIndex]; }
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pNaviDataFilePath);
	virtual HRESULT	NativeConstruct(void* pArg) override;
public:
	_bool			Move_OnNavigation(_fvector vPosition, _fvector vCurrentPosition, _float3** pSlidingVector = nullptr);
	_float			Compute_Y(_fvector vPoint);
	void			Find_Cell(_fvector vPosition);
#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	vector<class CCell*>			m_Cells;
	typedef vector<class CCell*>	CELLS;

#ifdef _DEBUG
private:
	class CVIBuffer_Triangle*		m_pVIBuffer = nullptr;
	class CShader*					m_pShader = nullptr;
#endif
private:
	// 네비메쉬를 사용하는 객체가 존재하고 있는 셀의 인덱스
	NAVIDESC		m_NaviDesc;
private:
	HRESULT			SetUp_Neighbor();
public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNaviDataFilePath);
	virtual CComponent*	Clone(void* pArg) override;
	virtual void		Free() override;
};

END