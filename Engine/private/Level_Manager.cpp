#include "..\public\Level_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::OpenLevel(_uint iNextLevelIndex, CLevel * pNextLevel)
{
	if (nullptr == pNextLevel)
	{
		MSGBOX("pNextLevel is nullptr in CLevel_Manager::OpenLevel");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pCurrentLevel)
	{
		if (FAILED(pGameInstance->Clear_LevelResource(m_iCurrentLevelIndex)))
			return E_FAIL;
	}

	Safe_Release(m_pCurrentLevel);

	m_iCurrentLevelIndex = iNextLevelIndex;
	m_pCurrentLevel = pNextLevel;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CLevel_Manager::Tick(_double dTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
	{
		MSGBOX("m_pCurrentLevel is nullptr in CLevel_Manager::Tick");
		return -1;
	}

	return m_pCurrentLevel->Tick(dTimeDelta);
}

_int CLevel_Manager::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
	{
		MSGBOX("m_pCurrentLevel is nullptr in CLevel_Manager::LateTick");
		return -1;
	}
	return m_pCurrentLevel->LateTick(dTimeDelta);

}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
	{
		MSGBOX("m_pCurrentLevel is nullptr in CLevel_Manager::Render");
		return E_FAIL;
	}

	return m_pCurrentLevel->Render();
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
