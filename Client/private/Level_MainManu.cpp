#include "stdafx.h"
#include "..\public\Level_MainManu.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "Cursor.h"

CLevel_MainManu::CLevel_MainManu(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_MainManu::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
	{
		MSGBOX("Ready_Layer_UI returned E_FAIL in CLevel_MainManu::NativeConstruct");
		return E_FAIL;
	}
	g_pGameInstance->PlayBGM(TEXT("MainManuBGM.mp3"), 0.5f);

	m_pCursor = CCursor::Create(m_pDevice, m_pDeviceContext);

	return S_OK;
}

_int CLevel_MainManu::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	m_pCursor->Tick(dTimeDelta);

	if (g_pGameInstance->Get_KeyUp(DIK_NUMPADENTER) || true == g_bGoNextLevel)
	{

		g_eCurrentLevel = LEVEL_LOADING;
		if (FAILED(g_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_LOBY))))
		{
			MSGBOX("g_pGameInstance->OpenLevel returned E_FAIL in CLevel_Loby::Tick");
			return -1;
		}
		g_pGameInstance->StopAll();
		g_bGoNextLevel = false;
	}

	
	return 0;
}

_int CLevel_MainManu::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	return 0;
}

HRESULT CLevel_MainManu::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pCursor->Render();

	//SetWindowText(g_hWnd, TEXT("·ÎºñÀÓ. "));

	return S_OK;
}

HRESULT CLevel_MainManu::Ready_Layer_UI(const _tchar * pLayerTag)
{
	INFO_UI tInfoUI;
	tInfoUI.fPositionX = 640.f;
	tInfoUI.fPositionY = 360.f;
	tInfoUI.fScaleX = 1280.f;
	tInfoUI.fScaleY = 720.f;
	tInfoUI.iTextureIndex = 90;
	tInfoUI.fDepth = 0.f;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
	{
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, pLayerTag, TEXT("Prototype_GameObject_Start"))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, pLayerTag, TEXT("Prototype_GameObject_Exit"))))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

CLevel_MainManu * CLevel_MainManu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_MainManu*	pInstance = new CLevel_MainManu(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_MainManu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MainManu::Free()
{
	__super::Free();

	Safe_Release(m_pCursor);
}
