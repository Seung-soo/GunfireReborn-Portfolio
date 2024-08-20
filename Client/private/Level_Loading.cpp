#include "stdafx.h"
#include "..\Public\Level_Loading.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Level_Loby.h"
#include "UI_LoadingFire.h"
#include "Level_Stage3_1.h"
#include "Level_Stage3_2.h"
#include "Level_Stage4_1.h"
#include "Level_Stage4_2.h"
#include "Level_MainManu.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{

}

HRESULT CLevel_Loading::NativeConstruct(LEVEL eNextLevel)
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	m_eNextLevel = eNextLevel;
	INFO_UI tInfoUI;

	switch (m_eNextLevel)
	{
	case LEVEL_MAINMANU:
		g_eCurrentLevel = LEVEL_MAINMANU;
		Load_LoadingData();
		break;
	case LEVEL_LOBY:
		g_eCurrentLevel = LEVEL_LOBY;
		Load_LoadingData();
		Load_Terrain();
		break;
	case LEVEL_STAGE3_1:

		tInfoUI.fScaleX = g_iWinCX;
		tInfoUI.fScaleY = g_iWinCY;
		tInfoUI.fPositionX = g_iWinCX * 0.5f;
		tInfoUI.fPositionY = g_iWinCY * 0.5f;
		tInfoUI.fDepth = -9.f;
		tInfoUI.iTextureIndex = 95;
		tInfoUI.bDelete = false;
		g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI);


		// Texture UI
		if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_UI_LoadingFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/LoadingFire/img_logofire_%d.png"), 30))))
		{
			MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CCLevel_Loading::NativeConstruct(Texture_UI_LoadingFire)");
			return E_FAIL;
		}

		// GameObject UI_LoadingFire
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI_LoadingFire"), TEXT("Prototype_GameObject_UI_LoadingFire"))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loading::NativeConstruct(GameObject_UI_LoadingFire)");
			return E_FAIL;
		}

		tInfoUI.fScaleX = 200.f;
		tInfoUI.fScaleY = 100.f;
		tInfoUI.fPositionX = 1140.f;
		tInfoUI.fPositionY = 650.f;
		tInfoUI.fDepth = -15.f;
		tInfoUI.iTextureIndex = 2;
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
		{
			__debugbreak();
			return E_FAIL;
		}

		break;
	case LEVEL_STAGE3_2:

		tInfoUI.fScaleX = g_iWinCX;
		tInfoUI.fScaleY = g_iWinCY;
		tInfoUI.fPositionX = g_iWinCX * 0.5f;
		tInfoUI.fPositionY = g_iWinCY * 0.5f;
		tInfoUI.fDepth = -9.f;
		tInfoUI.iTextureIndex = 96;
		tInfoUI.bDelete = false;
		g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI);



		// Texture UI
		if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_UI_LoadingFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/LoadingFire/img_logofire_%d.png"), 30))))
		{
			MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CCLevel_Loading::NativeConstruct(Texture_UI_LoadingFire)");
			return E_FAIL;
		}

		// GameObject UI_LoadingFire
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI_LoadingFire"), TEXT("Prototype_GameObject_UI_LoadingFire"))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loading::NativeConstruct(GameObject_UI_LoadingFire)");
			return E_FAIL;
		}
		tInfoUI.fScaleX = 200.f;
		tInfoUI.fScaleY = 100.f;
		tInfoUI.fPositionX = 1140.f;
		tInfoUI.fPositionY = 650.f;
		tInfoUI.fDepth = -15.f;
		tInfoUI.iTextureIndex = 2;
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
		{
			__debugbreak();
			return E_FAIL;
		}
		break;
	case LEVEL_STAGE4_1:

		tInfoUI.fScaleX = g_iWinCX;
		tInfoUI.fScaleY = g_iWinCY;
		tInfoUI.fPositionX = g_iWinCX * 0.5f;
		tInfoUI.fPositionY = g_iWinCY * 0.5f;
		tInfoUI.fDepth = -9.f;
		tInfoUI.iTextureIndex = 97;
		tInfoUI.bDelete = false;
		g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI);


		// Texture UI
		if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_UI_LoadingFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/LoadingFire/img_logofire_%d.png"), 30))))
		{
			MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CCLevel_Loading::NativeConstruct(Texture_UI_LoadingFire)");
			return E_FAIL;
		}

		// GameObject UI_LoadingFire
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI_LoadingFire"), TEXT("Prototype_GameObject_UI_LoadingFire"))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loading::NativeConstruct(GameObject_UI_LoadingFire)");
			return E_FAIL;
		}

		tInfoUI.fScaleX = 200.f;
		tInfoUI.fScaleY = 100.f;
		tInfoUI.fPositionX = 1140.f;
		tInfoUI.fPositionY = 650.f;
		tInfoUI.fDepth = -15.f;
		tInfoUI.iTextureIndex = 2;
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
		{
			__debugbreak();
			return E_FAIL;
		}
		break;
	case LEVEL_STAGE4_2:

		tInfoUI.fScaleX = g_iWinCX;
		tInfoUI.fScaleY = g_iWinCY;
		tInfoUI.fPositionX = g_iWinCX * 0.5f;
		tInfoUI.fPositionY = g_iWinCY * 0.5f;
		tInfoUI.fDepth = -9.f;
		tInfoUI.iTextureIndex = 98;
		g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI);


		// Texture UI
		if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_UI_LoadingFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/LoadingFire/img_logofire_%d.png"), 30))))
		{
			MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CCLevel_Loading::NativeConstruct(Texture_UI_LoadingFire)");
			return E_FAIL;
		}

		// GameObject UI_LoadingFire
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI_LoadingFire"), TEXT("Prototype_GameObject_UI_LoadingFire"))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loading::NativeConstruct(GameObject_UI_LoadingFire)");
			return E_FAIL;
		}
		tInfoUI.fScaleX = 200.f;
		tInfoUI.fScaleY = 100.f;
		tInfoUI.fPositionX = 1140.f;
		tInfoUI.fPositionY = 650.f;
		tInfoUI.fDepth = -15.f;
		tInfoUI.iTextureIndex = 2;
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
		{
			__debugbreak();
			return E_FAIL;
		}
		break;
	}

	m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, m_eNextLevel);

	return S_OK;
}

_int CLevel_Loading::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;


	return 0;
}

_int CLevel_Loading::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	if (true == m_pLoader->isFinished())
	{
		CGameInstance*	pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CLevel*		pLevel = nullptr;

		pGameInstance->Release_Light();

		switch (m_eNextLevel)
		{
		case LEVEL_MAINMANU:
			g_eCurrentLevel = LEVEL_MAINMANU;
			pLevel = CLevel_MainManu::Create(m_pDevice, m_pDeviceContext);
			break;
		case LEVEL_LOBY:
			g_eCurrentLevel = LEVEL_LOBY;
			pLevel = CLevel_Loby::Create(m_pDevice, m_pDeviceContext);
			break;
		case LEVEL_STAGE3_1:
			g_eCurrentLevel = LEVEL_STAGE3_1;
			pLevel = CLevel_Stage3_1::Create(m_pDevice, m_pDeviceContext);
			break;
		case LEVEL_STAGE3_2:
			g_eCurrentLevel = LEVEL_STAGE3_2;
			pLevel = CLevel_Stage3_2::Create(m_pDevice, m_pDeviceContext);
			break;
		case LEVEL_STAGE4_1:
			g_eCurrentLevel = LEVEL_STAGE4_1;
			pLevel = CLevel_Stage4_1::Create(m_pDevice, m_pDeviceContext);
			break;
		case LEVEL_STAGE4_2:
			g_eCurrentLevel = LEVEL_STAGE4_2;
			pLevel = CLevel_Stage4_2::Create(m_pDevice, m_pDeviceContext);
			break;
		}

		if (FAILED(pGameInstance->OpenLevel(m_eNextLevel, pLevel)))
			return E_FAIL;

		Safe_Release(pGameInstance);
	}

	return 0;
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Load_LoadingData()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	wifstream		fin;

	fin.open(L"../Data/UI/Loading0.txt");

	if (!fin.fail())
	{
		TCHAR	szUIFileName[MAX_PATH] = L"";
		TCHAR	szChildName[MAX_PATH] = L"";
		TCHAR	szScaleX[MAX_PATH] = L"";
		TCHAR	szScaleY[MAX_PATH] = L"";
		TCHAR	szPosX[MAX_PATH] = L"";
		TCHAR	szPosY[MAX_PATH] = L"";
		TCHAR	szDepth[MAX_PATH] = L"";
		TCHAR	szTexIndex[MAX_PATH] = L"";

		while (true)
		{
			fin.getline(szUIFileName, MAX_PATH, '|');
			fin.getline(szChildName, MAX_PATH, '|');
			fin.getline(szScaleX, MAX_PATH, '|');
			fin.getline(szScaleY, MAX_PATH, '|');
			fin.getline(szPosX, MAX_PATH, '|');
			fin.getline(szPosY, MAX_PATH, '|');
			fin.getline(szDepth, MAX_PATH, '|');
			fin.getline(szTexIndex, MAX_PATH);

			if (fin.eof())
				break;

			INFO_UI tInfoUI;
			lstrcat(szUIFileName, L"_");
			lstrcat(szUIFileName, szChildName);
			tInfoUI.fScaleX = (_float)_tstof(szScaleX);
			tInfoUI.fScaleY = (_float)_tstof(szScaleY);
			tInfoUI.fPositionX = (_float)_tstof(szPosX);
			tInfoUI.fPositionY = (_float)_tstof(szPosY);
			tInfoUI.fDepth = (_float)_tstof(szDepth);
			tInfoUI.iTextureIndex = (_int)_tstoi(szTexIndex);
			tInfoUI.bDelete = false;

			if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOADING, szUIFileName, TEXT("Prototype_GameObject_UI"), &tInfoUI)))
			{
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loading::Load_LoadingData");
				return;
			}

		}
		fin.close();
	}
	// Texture UI
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_UI_LoadingFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/LoadingFire/img_logofire_%d.png"), 30))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CCLevel_Loading::Load_LoadingData(Texture_UI_LoadingFire)");
		return;
	}

	// GameObject UI_LoadingFire
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Layer_UI_LoadingFire"), TEXT("Prototype_GameObject_UI_LoadingFire"))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loading::Load_LoadingData(GameObject_UI_LoadingFire)");
		return;
	}
	RELEASE_INSTANCE(CGameInstance);

}

void CLevel_Loading::Load_Terrain()
{


	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_LOBY, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Data/Terrain_Loby.dat")))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loading::Load_Terrain");
		return;
	}

}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevel)))
	{
		MSGBOX("Failed to Creating CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
