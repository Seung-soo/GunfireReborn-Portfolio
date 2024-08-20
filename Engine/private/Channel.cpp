#include "..\public\Channel.h"
#include "HierarchyNode.h"

CChannel::CChannel()
{
}

void CChannel::Set_TransformationMatrix(_fmatrix TransformationMatrix)
{
	if (nullptr == m_pNode)
		return;

	m_pNode->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Set_HierarchyNodePtr(CHierarchyNode * pNode)
{
	m_pNode = pNode;

	Safe_AddRef(m_pNode);
}

HRESULT CChannel::NativeConstruct(const char * pName)
{
	strcpy_s(m_szName, pName);

	return S_OK;
}

CChannel * CChannel::Create(const char * pName)
{
	CChannel*	pInstance = new CChannel();

	if (FAILED(pInstance->NativeConstruct(pName)))
	{
		MSGBOX("Don't Create CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	for (auto& pKeyFrame : m_KeyFrames)
		Safe_Delete(pKeyFrame);
	m_KeyFrames.clear();

	Safe_Release(m_pNode);
}