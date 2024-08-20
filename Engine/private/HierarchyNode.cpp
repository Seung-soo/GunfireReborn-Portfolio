#include "..\public\HierarchyNode.h"

CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::NativeConstruct(const char * pName
	, _float4x4 TransformationMatrix, _uint iDepth, CHierarchyNode* pParent)
{
	m_pParent = pParent;
	Safe_AddRef(m_pParent);

	strcpy_s(m_szName, pName);
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(
		XMLoadFloat4x4(&TransformationMatrix)));
	XMStoreFloat4x4(&m_CombineTransformationMatrix, XMMatrixIdentity());
	m_iDepth = iDepth;

	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix()
{
	_matrix	TransformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
	_matrix CombinedTransformationMatrix;

	if (nullptr == m_pParent)		// 최상위 노드일 때
		CombinedTransformationMatrix = TransformationMatrix;	// 내 트랜스폼이 곧 컴파인드 트랜스폼
	else	// 최상위 노드가 아닐 때
			// 부모의 컴바인 행렬과 곱해줌
		CombinedTransformationMatrix = TransformationMatrix * XMLoadFloat4x4(&m_pParent->m_CombineTransformationMatrix);

	XMStoreFloat4x4(&m_CombineTransformationMatrix, CombinedTransformationMatrix);		// 갱신된 컴바인 행렬을 집어넣어줌
}

CHierarchyNode * CHierarchyNode::Create(const char * pName, _float4x4 TransformationMatrix, _uint iDepth, CHierarchyNode* pParent)
{
	CHierarchyNode* pInstance = new CHierarchyNode;

	if (FAILED(pInstance->NativeConstruct(pName, TransformationMatrix, iDepth, pParent)))
	{
		MSGBOX("Failed to Created CHierarchyNode");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHierarchyNode::Free()
{
	Safe_Release(m_pParent);
}

