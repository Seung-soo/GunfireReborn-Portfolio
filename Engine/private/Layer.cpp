#include "..\public\Layer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Component.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, _uint iIndex)
{
	if (iIndex >= m_Objects.size())
	{
		MSGBOX("iIndex >= m_Objects.size in CLayer::Get_Component");
		return nullptr;
	}

	auto iter = m_Objects.begin();

	for (_uint i = 0; i < iIndex; i++)
		++iter;
	
	return (*iter)->Get_Component(pComponentTag);
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	m_Objects.push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::ThrowToLayer()
{
	if (false == m_ObjectPool.empty())
	{
		m_Objects.push_back(m_ObjectPool.front());
		m_ObjectPool.pop_front();
		// static_cast<CTransform*>(m_Objects.back()->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(t), 0.5f, _float(rand() % 10), 1.f));
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CLayer::Add_ObjectPool(CGameObject * pGameObject)
{
	m_ObjectPool.push_back(pGameObject);

	return S_OK;
}

void CLayer::FrontChange()
{
	CGameObject* pGameObject = m_Objects.front();
	m_Objects.pop_front();
	m_Objects.push_back(pGameObject);
}

_int CLayer::Tick(_double dTimeDelta)
{
	for (auto iter = m_Objects.begin(); iter != m_Objects.end();)
	{
		if (m_Objects.empty())
			break;

		_int iState = (*iter)->Tick(dTimeDelta);
		if (0 > iState)
		{
			Safe_Release(*iter);
			iter = m_Objects.erase(iter);
		}
		else if (RETURN_OBJECTPOOL == iState)
		{
			Add_ObjectPool(*iter);
			iter = m_Objects.erase(iter);
		}
		else
			iter++;
	}
	return 0;
}

_int CLayer::LateTick(_double dTimeDelta)
{
	for (auto iter = m_Objects.begin(); iter != m_Objects.end();)
	{
		if (m_Objects.empty())
			break;

		_int iState = (*iter)->LateTick(dTimeDelta);
		if (0 > iState)
		{
			Safe_Release(*iter);
			iter = m_Objects.erase(iter);
		}
		else
			iter++;
	}
	return 0;
}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_Objects)
		Safe_Release(pGameObject);

	for (auto& pGameObject : m_ObjectPool)
		Safe_Release(pGameObject);

	m_Objects.clear();
}
