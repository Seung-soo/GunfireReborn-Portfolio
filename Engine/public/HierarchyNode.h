#pragma once

#include "Base.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;
public:
	HRESULT NativeConstruct(const char * pName, _float4x4 TransformationMatrix, _uint iDepth, CHierarchyNode* pParent);
	void	Update_CombinedTransformationMatrix();		// 누적 트랜스폼 행렬 갱신 함수
public:
	_uint			Get_Depth() { return m_iDepth; }
	const char*		Get_Name() { return m_szName; }
	_fmatrix		Get_OffsetMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }	// 오프셋 매트릭스 반환
	_float4x4*		Get_OffsetFloat4x4() { return &m_OffsetMatrix; }	// 오프셋 매트릭스 반환
	_fmatrix		Get_CombinedTransformationMatrix() { return XMLoadFloat4x4(&m_CombineTransformationMatrix); }	// 컴바인 매트릭스 반환
	_float4x4*		Get_CombinedTransformationFloat4x4() { return &m_CombineTransformationMatrix; }	// 컴바인 매트릭스 반환
	_fmatrix		Get_TransformationMatrix() { return XMLoadFloat4x4(&m_TransformationMatrix); }	// 매트릭스 반환
public:
	void			Set_OffsetMatrix(_float4x4 OffsetMatrix) { XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix))); }
	void			Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}
	void			Set_CombineTransformationMatrix(_fmatrix CombineTransformationMatrix) {
		XMStoreFloat4x4(&m_CombineTransformationMatrix, CombineTransformationMatrix);
	}
private:
	CHierarchyNode*	m_pParent = nullptr;
	char			m_szName[MAX_PATH] = "";
	_float4x4		m_OffsetMatrix;					// 매쉬의 로컬과 뼈의 로컬이 다를 수 있다. 이걸 조정해주는 행렬.
	_float4x4		m_TransformationMatrix;			// 현재 뼈의 행렬
	_float4x4		m_CombineTransformationMatrix;	// 누적 행렬
	_uint			m_iDepth;
public:
	static CHierarchyNode* Create(const char* pName, _float4x4 TransformationMatrix, _uint iDepth, CHierarchyNode* pParent);
	virtual void Free() override;
};

END