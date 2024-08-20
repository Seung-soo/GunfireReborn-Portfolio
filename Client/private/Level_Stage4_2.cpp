#include "stdafx.h"
#include "..\Public\Level_Stage4_2.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "Load_Manager.h"
#include "Level_Loading.h"
#include "Player.h"
#include "PoleMonarch.h"
#include "StageName.h"
#include "Sky.h"

CLevel_Stage4_2::CLevel_Stage4_2(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Stage4_2::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
	{
		MSGBOX("Ready_Light returned E_FAIL in CLevel_Stage4_2::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Boss(TEXT("Layer_Boss"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Meshes(TEXT("Layer_Mesh"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
	//	return E_FAIL;

	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(54.f, 41.3f, 29.f, 1.f));
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));

	_matrix TransformMatrix = XMMatrixScaling(4.f, 4.f, 1.f) * XMMatrixTranslation(54.f, 49.3f, 37.f);

	CStageName::STAGENAMEDESC tStageNameDesc = {};
	XMStoreFloat4x4(&tStageNameDesc.WorldMatrix, TransformMatrix);
	tStageNameDesc.iTexIndex = 8;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_2, TEXT("Layer_StageNameStage"), TEXT("Prototype_GameObject_StageNameStage"), &tStageNameDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_2::NativeConstruct(tInfoUI_StageNameStage)");
		return E_FAIL;
	}

	static_cast<CSky*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Sky")))->NextSky();
	g_pGameInstance->PlayBGM(TEXT("Stage4_2.mp3"), 0.7f);
	g_pGameInstance->PlayRoofSound(TEXT("SnowMapWind.mp3"), CSoundMgr::SYSTEM_EFFECT3, 0.5f);

	return S_OK;
}

_int CLevel_Stage4_2::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;



	return 0;
}

_int CLevel_Stage4_2::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	return 0;
}

HRESULT CLevel_Stage4_2::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	//SetWindowText(g_hWnd, TEXT("로비임. "));

	return S_OK;
}

HRESULT CLevel_Stage4_2::Ready_Light()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC		LightDesc;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTTIONAL;
	LightDesc.vDiffuse = _float4(0.52f, 0.65f, 0.9f, 1.f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(0.f, -1.f, -1.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
	{
		MSGBOX("pGameInstance->Add_Light returned E_FAIL in CLevel_Stage4_2::Ready_Light");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage4_2::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	wifstream		fin;

	fin.open(L"../Data/Terrain/Terrain_Stage4_Boss.txt");

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

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_2, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_2::Ready_Layer_Terrain(GameObject_Terrain)");
			return E_FAIL;
		}
		static_cast<CTerrain*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, TEXT("Layer_Terrain")))->Set_Info(vPosition, tInfoTerrain.iTextureIndex, tInfoTerrain.iPass);
		static_cast<CTerrain*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, TEXT("Layer_Terrain")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_2"));
		static_cast<CPlayer*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_2"));
		fin.close();
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage4_2::Ready_Layer_Meshes(const _tchar * pLayerTag)
{
	if (FAILED(CLoad_Manager::GetInstance()->Make_GameObject(LEVEL_STAGE4_2)))
	{
		MSGBOX("CLoad_Manager::GetInstance()->Make_GameObject returned E_FAIL in CLevel_Stage4_2::Ready_Layer_Meshes");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Stage4_2::Ready_Layer_Boss(const _tchar * pLayerTag)
{
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_2, pLayerTag, TEXT("Prototype_GameObject_PoleMonarch"), &XMVectorSet(54.f, 41.3f, 48.f, 1.f))))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_2::Ready_Layer_Boss");
		return E_FAIL;
	}
	static_cast<CPoleMonarch*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_2, pLayerTag))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_2"));

	for (_uint i = 0; i < 15; i++)
	{
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_2, TEXT("Layer_RevolutionRock"), TEXT("Prototype_GameObject_RevolutionRock"))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(GameObject_RevolutionRock)");
			return E_FAIL;
		}
	}

	for (_uint i = 0; i < 8; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE4_2, TEXT("Layer_BossCannon"), TEXT("Prototype_GameObject_BossCannon"))))
		{
			MSGBOX("pGameInstance->Add_ObjectPool returned E_FAIL in CLoader::Loading_ForStage4_2(GameObject_BossCannon)");
			return E_FAIL;
		}
	}

	for (_uint i = 0; i < 5; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE4_2, TEXT("Layer_BossGun"), TEXT("Prototype_GameObject_BossGun"))))
		{
			MSGBOX("pGameInstance->Add_ObjectPool returned E_FAIL in CLoader::Loading_ForStage4_2(GameObject_BossGun)");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_Stage4_2::Ready_Layer_UI(const _tchar * pLayerTag)
{
	// 스테이지 이름
	INFO_UI tInfoUI_StageName = {};
	ZeroMemory(&tInfoUI_StageName, sizeof(INFO_UI));

	tInfoUI_StageName.iTextureIndex = 4;
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_2, TEXT("Layer_StageNameUI"), TEXT("Prototype_GameObject_StageNameUI"), &tInfoUI_StageName)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_2::Ready_Layer_UI(tInfoUI_StageName)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Stage4_2::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	return S_OK;
}

CLevel_Stage4_2 * CLevel_Stage4_2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Stage4_2*	pInstance = new CLevel_Stage4_2(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_Stage4_2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage4_2::Free()
{
	__super::Free();
}
