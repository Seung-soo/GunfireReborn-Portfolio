#include "stdafx.h"
#include "..\Public\Level_Stage4_1.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "Load_Manager.h"
#include "Level_Loading.h"
#include "Player.h"
#include "Monkey.h"
#include "Ghost.h"
#include "Bear.h"
#include "Fox.h"
#include "MonsterWave.h"
#include "StageWall.h"
#include "SkillBox.h"
#include "StageName.h"
#include "Sky.h"
#include "Player_Hand.h"

CLevel_Stage4_1::CLevel_Stage4_1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Stage4_1::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
	{
		MSGBOX("Ready_Light returned E_FAIL in CLevel_Stage4_1::NativeConstruct");
		return E_FAIL;
	}


	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Meshes(TEXT("Layer_Mesh"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
	//	return E_FAIL;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_StageWall"))))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_1::NativeConstruct");
		return E_FAIL;
	}
	CStageWall* pStageWall = static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Wall")));
	pStageWall->WallSetting(37, CStageWall::LINE_AB, false);


	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(42.6f, 0.8f, 29.3f, 1.f));
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));

	_matrix TransformMatrix = XMMatrixScaling(4.f, 4.f, 1.f) * XMMatrixTranslation(42.6f, 3.8f, 37.3f);

	CStageName::STAGENAMEDESC tStageNameDesc = {};
	XMStoreFloat4x4(&tStageNameDesc.WorldMatrix, TransformMatrix);
	tStageNameDesc.iTexIndex = 7;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_1, TEXT("Layer_StageNameStage"), TEXT("Prototype_GameObject_StageNameStage"), &tStageNameDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_1::NativeConstruct(tInfoUI_StageNameStage)");
		return E_FAIL;
	}

	static_cast<CSky*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Sky")))->NextSky();
	g_pGameInstance->PlayBGM(TEXT("Stage4_1.mp3"), 0.5f);

	g_pGameInstance->PlayRoofSound(TEXT("SnowMapWind.mp3"), CSoundMgr::SYSTEM_EFFECT3, 0.5f);

	static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")));

	return S_OK;
}

_int CLevel_Stage4_1::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	if (g_pGameInstance->Get_KeyUp(DIK_NUMPADENTER) || true == g_bGoNextLevel)
	{
		CMonsterWave::GetInstance()->Free();
		g_eCurrentLevel = LEVEL_LOADING;
		if (FAILED(g_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_STAGE4_2))))
		{
			MSGBOX("g_pGameInstance->OpenLevel returned E_FAIL in CLevel_Stage4_1::Tick");
			return -1;
		}
		g_bGoNextLevel = false;
		g_pGameInstance->StopAll();
	}

	return 0;
}

_int CLevel_Stage4_1::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	return 0;
}

HRESULT CLevel_Stage4_1::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	//SetWindowText(g_hWnd, TEXT("로비임. "));

	return S_OK;
}

HRESULT CLevel_Stage4_1::Ready_Light()
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
		MSGBOX("pGameInstance->Add_Light returned E_FAIL in CLevel_Stage4_1::Ready_Light");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage4_1::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	wifstream		fin;

	fin.open(L"../Data/Terrain/Terrain_Stage4_1.txt");

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

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_1, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_1::Ready_Layer_Terrain(GameObject_Terrain)");
			return E_FAIL;
		}
		static_cast<CTerrain*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Terrain")))->Set_Info(vPosition, tInfoTerrain.iTextureIndex, tInfoTerrain.iPass);
		static_cast<CTerrain*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Terrain")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));
		static_cast<CPlayer*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));
		//}
		fin.close();
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage4_1::Ready_Layer_Meshes(const _tchar * pLayerTag)
{
	if (FAILED(CLoad_Manager::GetInstance()->Make_GameObject(LEVEL_STAGE4_1)))
	{
		MSGBOX("CLoad_Manager::GetInstance()->Make_GameObject returned E_FAIL in CLevel_Stage4_1::Ready_Layer_Meshes");
		return E_FAIL;
	}

	_float4x4 WorldMatrixFloat;

	_matrix WorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(170.f)) * XMMatrixTranslation(18.f, 0.f, 121.5f);
	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_1, TEXT("Layer_SkillBox"), TEXT("Prototype_GameObject_SkillBox"), &WorldMatrixFloat)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_1::Ready_Layer_Meshes(Prototype_GameObject_SkillBox)");
		return E_FAIL;
	}

	WorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(40.f)) * XMMatrixTranslation(92.f, 0.f, 75.f);
	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_1, TEXT("Layer_SkillBox"), TEXT("Prototype_GameObject_SkillBox"), &WorldMatrixFloat)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_1::Ready_Layer_Meshes(Prototype_GameObject_SkillBox)");
		return E_FAIL;
	}

	WorldMatrix = XMMatrixTranslation(118.f, 0.f, 10.5f);
	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_1, TEXT("Layer_SkillBox"), TEXT("Prototype_GameObject_SkillBox"), &WorldMatrixFloat)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_1::Ready_Layer_Meshes(Prototype_GameObject_SkillBox)");
		return E_FAIL;
	}

	WorldMatrix = XMMatrixTranslation(121.f, 19.8f, 10.5f);
	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_1, TEXT("Layer_NPC_Shop"), TEXT("Prototype_GameObject_NPC_Shop"), &WorldMatrixFloat)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_1::Ready_Layer_Meshes(Prototype_GameObject_NPC_Shop)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Stage4_1::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	for (_uint i = 0; i < 5; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE4_1, TEXT("Layer_Monkey"), TEXT("Prototype_GameObject_Monkey"))))
		{
			MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLevel_Stage4_1::Ready_Layer_Monster");
			return E_FAIL;
		}
		static_cast<CMonkey*>(g_pGameInstance->Get_ObjectPoolBack(LEVEL_STAGE4_1, TEXT("Layer_Monkey")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));
	}


	//if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE4_1, TEXT("Layer_UFO"), TEXT("Prototype_GameObject_UFO"))))
	//{
	//	MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLevel_Stage4_2::Ready_Layer_Monster");
	//	return E_FAIL;
	//}

	for (_uint i = 0; i < 3; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE4_1, TEXT("Layer_Fox"), TEXT("Prototype_GameObject_Fox"))))
		{
			MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLevel_Stage4_2::Ready_Layer_Monster");
			return E_FAIL;
		}
		static_cast<CFox*>(g_pGameInstance->Get_ObjectPoolBack(LEVEL_STAGE4_1, TEXT("Layer_Fox")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));
	}

	for (_uint i = 0; i < 4; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE4_1, TEXT("Layer_Ghost"), TEXT("Prototype_GameObject_Ghost"))))
		{
			MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLevel_Stage4_1::Ready_Layer_Monster");
			return E_FAIL;
		}
		static_cast<CGhost*>(g_pGameInstance->Get_ObjectPoolBack(LEVEL_STAGE4_1, TEXT("Layer_Ghost")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));
	}

	for (_uint i = 0; i < 3; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE4_1, TEXT("Layer_Bear"), TEXT("Prototype_GameObject_Bear"))))
		{
			MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLevel_Stage4_1::Ready_Layer_Monster");
			return E_FAIL;
		}
		static_cast<CBear*>(g_pGameInstance->Get_ObjectPoolBack(LEVEL_STAGE4_1, TEXT("Layer_Bear")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage4_1"));
	}

	CMonsterWave::GetInstance()->WaveAppear(CMonsterWave::STAGE4_1_1);

	return S_OK;
}

HRESULT CLevel_Stage4_1::Ready_Layer_UI(const _tchar * pLayerTag)
{
	// 스테이지 이름
	INFO_UI tInfoUI_StageName = {};
	ZeroMemory(&tInfoUI_StageName, sizeof(INFO_UI));

	tInfoUI_StageName.iTextureIndex = 3;
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_1, TEXT("Layer_StageNameUI"), TEXT("Prototype_GameObject_StageNameUI"), &tInfoUI_StageName)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_1::Ready_Layer_UI(tInfoUI_StageName)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Stage4_1::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CTransform*	pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_2, TEXT("Layer_Snow"), TEXT("Prototype_GameObject_Particle"), pPlayerTransform)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage4_1::Ready_Layer_UI(Snow)");
		return E_FAIL;
	}
	return S_OK;
}

CLevel_Stage4_1 * CLevel_Stage4_1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Stage4_1*	pInstance = new CLevel_Stage4_1(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_Stage4_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage4_1::Free()
{
	__super::Free();
}
