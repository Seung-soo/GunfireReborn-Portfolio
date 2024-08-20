#include "stdafx.h"
#include "..\Public\Level_Stage3_1.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Terrain.h"
#include "Load_Manager.h"
#include "Level_Loading.h"
#include "Player.h"
#include "Yacha.h"
#include "CatfishWarrior.h"
#include "Kappa.h"
#include "Monk.h"
#include "Lobstar.h"
#include "StageWall.h"
#include "MonsterWave.h"
#include "SkillBox.h"
#include "StageName.h"

CLevel_Stage3_1::CLevel_Stage3_1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Stage3_1::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
	{
		MSGBOX("Ready_Light returned E_FAIL in CLevel_Stage3_1::NativeConstruct");
		return E_FAIL;
	}


	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Meshes(TEXT("Layer_Mesh"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
	//	return E_FAIL;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_StageWall"))))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::NativeConstruct");
		return E_FAIL;
	}
	CStageWall* pStageWall = static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Wall")));
	pStageWall->WallSetting(50, CStageWall::LINE_CA, false);
	pStageWall->Push_Cell(51);
	pStageWall->Push_Cell(48);
	pStageWall->Push_Cell(49);
	pStageWall->Push_Cell(52);
	pStageWall->Push_Cell(53);

	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(27.3f, 2.2f, 32.4f, 1.f));
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));

	_matrix TransformMatrix = XMMatrixScaling(4.f, 4.f, 1.f) * XMMatrixTranslation(27.3f, 5.2f, 40.4f);

	CStageName::STAGENAMEDESC tStageNameDesc = {};
	XMStoreFloat4x4(&tStageNameDesc.WorldMatrix, TransformMatrix);
	tStageNameDesc.iTexIndex = 5;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_StageNameStage"), TEXT("Prototype_GameObject_StageNameStage"), &tStageNameDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::NativeConstruct(tInfoUI_StageNameStage)");
		return E_FAIL;
	}

	g_pGameInstance->PlayBGM(TEXT("Stage3_1.mp3"), 0.5f);


	return S_OK;
}

_int CLevel_Stage3_1::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	if (g_pGameInstance->Get_KeyUp(DIK_NUMPADENTER) || true == g_bGoNextLevel)
	{
		CMonsterWave::GetInstance()->Free();
		g_pGameInstance->Release_Light();
		g_eCurrentLevel = LEVEL_LOADING;
		if (FAILED(g_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_STAGE3_2))))
		{
			MSGBOX("g_pGameInstance->OpenLevel returned E_FAIL in CLevel_Stage3_1::Tick");
			return -1;
		}
		g_bGoNextLevel = false;
		g_pGameInstance->StopAll();
	}

	return 0;
}

_int CLevel_Stage3_1::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	return 0;
}

HRESULT CLevel_Stage3_1::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	//SetWindowText(g_hWnd, TEXT("로비임. "));

	return S_OK;
}

HRESULT CLevel_Stage3_1::Ready_Light()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC		LightDesc;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_POINT;
	LightDesc.vDiffuse = _float4(0.26f, 0.4f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPosition = _float4(34.f, 3.4f, 41.5f, 1.f);
	LightDesc.fRange = 20.f;


	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Com_Transform")))->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));


	LightDesc.vPosition = _float4(21.9f, 3.4f, 40.4f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Com_Transform"), 1))->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));


	LightDesc.vPosition = _float4(5.1f, 2.3f, 66.5f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}


	LightDesc.vPosition = _float4(44.5f, 2.f, 67.6f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(43.3f, 1.6f, 76.2f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(7.9f, 1.6f, 115.7f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(12.f, 2.6f, 120.7f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(28.7f, 2.6f, 118.7f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(43.4f, 1.6f, 93.9f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(60.2f, 2.6f, 126.6f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(76.1f, 2.1f, 122.3f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(53.7f, 2.5f, 97.2f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(51.f, 1.7f, 68.7f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(102.1f, 1.6f, 63.6f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(106.2f, 6.5f, 56.1f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(109.2f, 4.6f, 44.f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(115.3f, 3.6f, 32.1f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(114.f, 3.9f, 16.f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(58.4f, 4.5f, 13.9f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(56.7f, 2.8f, 34.2f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(52.4f, 1.6f, 44.f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(84.8f, 9.f, 18.4f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(69.f, 7.2f, 26.3f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(69.f, 7.5f, 56.8f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	LightDesc.vPosition = _float4(94.2f, 7.1f, 40.5f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTTIONAL;
	LightDesc.vDiffuse = _float4(0.26f, 0.33f, 0.45f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
	{
		MSGBOX("pGameInstance->Add_Light returned E_FAIL in CLevel_Stage3_1::Ready_Light");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage3_1::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	
	wifstream		fin;

	fin.open(L"../Data/Terrain/Terrain_Stage3_1.txt");

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

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Terrain(GameObject_Terrain)");
			return E_FAIL;
		}
		static_cast<CTerrain*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Terrain")))->Set_Info(vPosition, tInfoTerrain.iTextureIndex, tInfoTerrain.iPass);
		static_cast<CTerrain*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Terrain")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
		static_cast<CPlayer*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));

		fin.close();
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage3_1::Ready_Layer_Meshes(const _tchar * pLayerTag)
{
	if (FAILED(CLoad_Manager::GetInstance()->Make_GameObject(LEVEL_STAGE3_1)))
	{
		MSGBOX("CLoad_Manager::GetInstance()->Make_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Meshes");
		return E_FAIL;
	}
	_float4x4 WorldMatrixFloat;

	_matrix WorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(6.f, 0.f, 81.f);
	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_SkillBox"), TEXT("Prototype_GameObject_SkillBox"), &WorldMatrixFloat)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Meshes(Prototype_GameObject_SkillBox)");
		return E_FAIL;
	}

	WorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(90.f, 3.5f, 97.f);
	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_SkillBox"), TEXT("Prototype_GameObject_SkillBox"), &WorldMatrixFloat)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Meshes(Prototype_GameObject_SkillBox)");
		return E_FAIL;
	}

	WorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(57.5f, 0.f, 21.f);
	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_SkillBox"), TEXT("Prototype_GameObject_SkillBox"), &WorldMatrixFloat)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Meshes(Prototype_GameObject_SkillBox)");
		return E_FAIL;
	}

	WorldMatrix = XMMatrixTranslation(96.f, 0.3f, 11.5f);
	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_NPC_Shop"), TEXT("Prototype_GameObject_NPC_Shop"), &WorldMatrixFloat)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Meshes(Prototype_GameObject_NPC_Shop)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Stage3_1::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	for (_uint i = 0; i < 7; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE3_1, TEXT("Layer_Fish"), TEXT("Prototype_GameObject_CatfishWarrior"))))
		{
			MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Monster(CatfishWarrior)");
			return E_FAIL;
		}
		static_cast<CCatfishWarrior*>(g_pGameInstance->Get_ObjectPoolBack(LEVEL_STAGE3_1, TEXT("Layer_Fish")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
	}

	for (_uint i = 0; i < 4; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE3_1, TEXT("Layer_Kappa"), TEXT("Prototype_GameObject_Kappa"))))
		{
			MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Monster(Kappa)");
			return E_FAIL;
		}
		static_cast<CKappa*>(g_pGameInstance->Get_ObjectPoolBack(LEVEL_STAGE3_1, TEXT("Layer_Kappa")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
	}

	for (_uint i = 0; i < 4; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE3_1, TEXT("Layer_Yacha"), TEXT("Prototype_GameObject_Yacha"))))
		{
			MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Monster(Yacha)");
			return E_FAIL;
		}
		static_cast<CYacha*>(g_pGameInstance->Get_ObjectPoolBack(LEVEL_STAGE3_1, TEXT("Layer_Yacha")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
	}

	if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE3_1, TEXT("Layer_Lobstar"), TEXT("Prototype_GameObject_Lobstar"))))
	{
		MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Monster(Lobstar)");
		return E_FAIL;
	}
	static_cast<CLobstar*>(g_pGameInstance->Get_ObjectPoolBack(LEVEL_STAGE3_1, TEXT("Layer_Lobstar")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));

	for (_uint i = 0; i < 2; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE3_1, TEXT("Layer_Monk"), TEXT("Prototype_GameObject_Monk"))))
		{
			MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLevel_Stage3_1::Ready_Layer_Monster(Monk)");
			return E_FAIL;
		}
		static_cast<CMonk*>(g_pGameInstance->Get_ObjectPoolBack(LEVEL_STAGE3_1, TEXT("Layer_Monk")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Stage3_1"));
	}

	CMonsterWave::GetInstance()->WaveAppear(CMonsterWave::STAGE3_1_1);

	return S_OK;
}

HRESULT CLevel_Stage3_1::Ready_Layer_UI(const _tchar * pLayerTag)
{
	// 스테이지 이름
	INFO_UI tInfoUI_StageName = {};
	ZeroMemory(&tInfoUI_StageName, sizeof(INFO_UI));

	tInfoUI_StageName.iTextureIndex = 1;
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE3_1, TEXT("Layer_StageNameUI"), TEXT("Prototype_GameObject_StageNameUI"), &tInfoUI_StageName)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Stage3_1::Ready_Layer_UI(tInfoUI_StageName)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Stage3_1::Ready_Layer_Effect(const _tchar * pLayerTag)
{


	return S_OK;
}

CLevel_Stage3_1 * CLevel_Stage3_1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Stage3_1*	pInstance = new CLevel_Stage3_1(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_Stage3_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage3_1::Free()
{
	__super::Free();
}
