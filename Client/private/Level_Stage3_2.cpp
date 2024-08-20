#include "stdafx.h"
#include "..\Public\Level_Stage3_2.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "Load_Manager.h"
#include "Level_Loading.h"
#include "Player.h"
#include "AbyssalSerpent.h"
#include "StageName.h"
#include "NextAria.h"

CLevel_Stage3_2::CLevel_Stage3_2(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Stage3_2::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
	{
		MSGBOX("Ready_Light returned E_FAIL in CLevel_Stage3_2::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Ocean(TEXT("Layer_Ocean"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Meshes(TEXT("Layer_Mesh"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
	//	return E_FAIL;

	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(46.4f, 2.3f, 7.6f, 1.f));
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_State(CTransform::STATE_LOOK, XMVectorSet(1.f, 0.f, 0.f, 0.f));

	_matrix TransformMatrix = XMMatrixScaling(4.f, 4.f, 1.f) * XMMatrixTranslation(46.4f, 5.3f, 15.6f);

	CStageName::STAGENAMEDESC tStageNameDesc = {};
	XMStoreFloat4x4(&tStageNameDesc.WorldMatrix, TransformMatrix);
	tStageNameDesc.iTexIndex = 6;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_StageNameStage"), TEXT("Prototype_GameObject_StageNameStage"), &tStageNameDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::NativeConstruct(tInfoUI_StageNameStage)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_NextAria"), TEXT("Prototype_GameObject_NextAria"))))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonsterWave::Update");
		return E_FAIL;
	}
	static_cast<CNextAria*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_NextAria")))->WallSetting(18, CNextAria::LINE_AB, 10);

	g_pGameInstance->PlayBGM(TEXT("Stage3_2_1.mp3"), 0.5f);
	g_pGameInstance->PlayRoofSound(TEXT("Wind.mp3"), CSoundMgr::MONSTER5, 0.2f);

	return S_OK;
}

_int CLevel_Stage3_2::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	if (g_pGameInstance->Get_KeyUp(DIK_NUMPADENTER) || true == g_bGoNextLevel)
	{
		g_eCurrentLevel = LEVEL_LOADING;
		if (FAILED(g_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_STAGE4_1))))
		{
			MSGBOX("g_pGameInstance->OpenLevel returned E_FAIL in CLevel_Stage3_2::Tick");
			return -1;
		}
		g_bGoNextLevel = false;
		g_pGameInstance->StopAll();
	}

	return 0;
}

_int CLevel_Stage3_2::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	return 0;
}

HRESULT CLevel_Stage3_2::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	//SetWindowText(g_hWnd, TEXT("로비임. "));

	return S_OK;
}

HRESULT CLevel_Stage3_2::Ready_Light()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC		LightDesc;


	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_POINT;
	LightDesc.vDiffuse = _float4(0.26f, 0.4f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPosition = _float4(35.6f, 3.6f, 9.5f, 1.f);
	LightDesc.fRange = 20.f;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(42.9f, 3.8f, 31.6f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(34.1f, 4.1f, 34.f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(12.4f, 4.3f, 34.9f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(19.6f, 4.1f, 56.4f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(31.4f, 5.f, 57.8f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(19.4f, 3.8f, 65.9f, 1.f);
	LightDesc.fRange = 8.f;
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Com_Transform"), 0))->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));


	LightDesc.vPosition = _float4(19.4f, 3.8f, 74.6f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Com_Transform"), 1))->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));


	LightDesc.vPosition = _float4(19.4f, 3.8f, 82.8f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Com_Transform"), 2))->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));


	LightDesc.vPosition = _float4(31.3f, 3.8f, 65.9f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Com_Transform"), 3))->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));


	LightDesc.vPosition = _float4(31.3f, 3.8f, 74.6f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Com_Transform"), 4))->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));


	LightDesc.vPosition = _float4(31.3f, 3.8f, 82.8f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Light");
		return E_FAIL;
	}
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STAGE3_2, TEXT("Layer_FireLightSmall"), TEXT("Com_Transform"), 5))->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTTIONAL;
	LightDesc.vDiffuse = _float4(0.26f, 0.33f, 0.45f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(-1.f, -1.f, 0.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage3_2::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	wifstream		fin;

	fin.open(L"../Data/Terrain/Terrain_Stage3_2.txt");

	if (!fin.fail())
	{
		TCHAR	szTerrainFileName[MAX_PATH] = L"";
		TCHAR	szChildName[MAX_PATH] = L"";
		TCHAR	szVerticesX[MAX_PATH] = L"";
		TCHAR	szVerticesZ[MAX_PATH] = L"";
		TCHAR	szTileNumX[MAX_PATH] = L"";
		TCHAR	szTileNumZ[MAX_PATH] = L"";
		TCHAR	szPosX[MAX_PATH] = L"";
		TCHAR	szPosY[MAX_PATH] = L"";
		TCHAR	szPosZ[MAX_PATH] = L"";
		TCHAR	szTexIndex[MAX_PATH] = L"";
		TCHAR	szPass[MAX_PATH] = L"";

		//while (true)
		//{
		fin.getline(szTerrainFileName, MAX_PATH, '|');
		fin.getline(szChildName, MAX_PATH, '|');
		fin.getline(szVerticesX, MAX_PATH, '|');
		fin.getline(szVerticesZ, MAX_PATH, '|');
		fin.getline(szTileNumX, MAX_PATH, '|');
		fin.getline(szTileNumZ, MAX_PATH, '|');
		fin.getline(szPosX, MAX_PATH, '|');
		fin.getline(szPosY, MAX_PATH, '|');
		fin.getline(szPosZ, MAX_PATH, '|');
		fin.getline(szTexIndex, MAX_PATH, '|');
		fin.getline(szPass, MAX_PATH);

		//if (fin.eof())
		//	break;

		INFO_TERRAIN tInfoTerrain;

		tInfoTerrain.vPosition.x = (_float)_tstof(szPosX);
		tInfoTerrain.vPosition.y = (_float)_tstof(szPosY);
		tInfoTerrain.vPosition.z = (_float)_tstof(szPosZ);
		tInfoTerrain.iTextureIndex = (_int)_tstoi(szTexIndex);
		tInfoTerrain.iPass = (_uint)_tstoi(szPass);
		_float3 vPosition;
		vPosition.x = tInfoTerrain.vPosition.x;
		vPosition.y = tInfoTerrain.vPosition.y;
		vPosition.z = tInfoTerrain.vPosition.z;

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Layer_Terrain(GameObject_Terrain)");
			return E_FAIL;
		}
		static_cast<CTerrain*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_2, TEXT("Layer_Terrain")))->Set_Info(vPosition, tInfoTerrain.iTextureIndex, tInfoTerrain.iPass);
		static_cast<CTerrain*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_2, TEXT("Layer_Terrain")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_2"));
		static_cast<CPlayer*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_2"));
		fin.close();
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage3_2::Ready_Layer_Ocean(const _tchar * pLayerTag)
{
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_Ocean"), TEXT("Prototype_GameObject_Ocean"), &XMVectorSet(-64.f, 1.f, -32.f, 1.f))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Layer_Ocean(GameObject_Ocean)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Stage3_2::Ready_Layer_Meshes(const _tchar * pLayerTag)
{
	if (FAILED(CLoad_Manager::GetInstance()->Make_GameObject(LEVEL_STAGE3_2)))
	{
		MSGBOX("CLoad_Manager::GetInstance()->Make_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Layer_Meshes");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Stage3_2::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Stage3_2::Ready_Layer_UI(const _tchar * pLayerTag)
{
	// 스테이지 이름
	INFO_UI tInfoUI_StageName = {};
	ZeroMemory(&tInfoUI_StageName, sizeof(INFO_UI));

	tInfoUI_StageName.iTextureIndex = 2;
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_2, TEXT("Layer_StageNameUI"), TEXT("Prototype_GameObject_StageNameUI"), &tInfoUI_StageName)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_2::Ready_Layer_UI(tInfoUI_StageName)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Stage3_2::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	return S_OK;
}

CLevel_Stage3_2 * CLevel_Stage3_2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Stage3_2*	pInstance = new CLevel_Stage3_2(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_Stage3_2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage3_2::Free()
{
	__super::Free();
}
