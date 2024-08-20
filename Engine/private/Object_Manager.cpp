#include "..\public\Object_Manager.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		MSGBOX("iLevelIndex >= m_iNumLevels in CObject_Manager::Get_Component");
		return nullptr;
	}

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
	{
		MSGBOX("pLayer is nullptr in CObject_Manager::Get_Component");
		return nullptr;
	}
	return pLayer->Get_Component(pComponentTag, iIndex);
}

CGameObject * CObject_Manager::Get_Front(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second->Get_Object()->front();
}

CGameObject * CObject_Manager::Get_Back(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second->Get_Object()->back();
}

CGameObject * CObject_Manager::Get_ObjectPoolBack(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second->Get_ObjectPool()->back();
}

list<CGameObject*>* CObject_Manager::Get_ObjectList(_uint iLevelIndex, const _tchar * pLayerTag)
{
	{
		CLayer* pList = Find_Layer(iLevelIndex, pLayerTag);
		if (nullptr == pList)
			return nullptr;
		else
			return pList->Get_Object();
	}
}

HRESULT CObject_Manager::Reserve_Container(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[m_iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == pPrototype)
	{
		MSGBOX("pPrototype is nullptr in CObject_Manager::Add_Prototype");
		return E_FAIL;
	}

	if (nullptr != Find_Prototype(pPrototypeTag))
	{
		MSGBOX("Find_Prototype is not nullptr in CObject_Manager::Add_Prototype");
		return E_FAIL;
	}

	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		MSGBOX("iLevelIndex >= m_iNumLevels in CObject_Manager::Add_GameObject");
		return E_FAIL;
	}

	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
	{
		MSGBOX("pPrototype is nullptr in CObject_Manager::Add_GameObject");
		return E_FAIL;
	}

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
	{
		MSGBOX("pGameObject is nullptr in CObject_Manager::Add_GameObject");
		return E_FAIL;
	}

	CLayer*			pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
		{
			MSGBOX("pLayer is nullptr in CObject_Manager::Add_GameObject");
			return E_FAIL;
		}

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CObject_Manager::ThrowToLayer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		MSGBOX("iLevelIndex >= m_iNumLevels in CObject_Manager::ThrowToLayer");
		return E_FAIL;
	}

	CLayer*			pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		MSGBOX("nullptr == pLayer in CObject_Manager::ThrowToLayer");
		return E_FAIL;
	}
	else
	{
		if (FAILED(pLayer->ThrowToLayer()))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CObject_Manager::Add_ObjectPool(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		MSGBOX("iLevelIndex >= m_iNumLevels in CObject_Manager::Add_ObjectPool");
		return E_FAIL;
	}

	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
	{
		MSGBOX("pPrototype is nullptr in CObject_Manager::Add_ObjectPool");
		return E_FAIL;
	}

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
	{
		MSGBOX("pGameObject is nullptr in CObject_Manager::Add_ObjectPool");
		return E_FAIL;
	}

	CLayer*			pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
		{
			MSGBOX("pLayer is nullptr in CObject_Manager::Add_ObjectPool");
			return E_FAIL;
		}

		if (FAILED(pLayer->Add_ObjectPool(pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		if (FAILED(pLayer->Add_ObjectPool(pGameObject)))
			return E_FAIL;
	}


	return S_OK;

}

HRESULT CObject_Manager::Clear_LevelObject(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		MSGBOX("iLevelIndex >= m_iNumLevels in CObject_Manager::Clear_LevelObject");
		return E_FAIL;
	}

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

HRESULT CObject_Manager::Release_LayerObject(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		MSGBOX("iLevelIndex >= m_iNumLevels in CObject_Manager::Release_LayerObject");
		return E_FAIL;
	}

	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return S_OK;

	CLayer* pLayer = iter->second;
	pLayer->Free();
	Safe_Release(pLayer);

	m_pLayers[iLevelIndex].erase(iter);

	return S_OK;
}

HRESULT CObject_Manager::FrontChange(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		MSGBOX("iLevelIndex >= m_iNumLevels in CObject_Manager::FrontChange");
		return E_FAIL;
	}
	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return E_FAIL;

	((*iter).second)->FrontChange();

	return S_OK;
}

_int CObject_Manager::Tick(_double dTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (0 > Pair.second->Tick(dTimeDelta))
			{
				MSGBOX("Pair.second->Tick returned nagative number in CObject_Manager::Tick");
				return -1;
			}
		}
	}
	return 0;
}

_int CObject_Manager::LateTick(_double dTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr == Pair.second)
				continue;
			if (0 > Pair.second->LateTick(dTimeDelta))
			{
				MSGBOX("Pair.second->LateTick returned nagative number in CObject_Manager::LateTick");
				return -1;
			}
		}
	}
	return 0;
}

CGameObject * CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTagFinder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}



void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}
