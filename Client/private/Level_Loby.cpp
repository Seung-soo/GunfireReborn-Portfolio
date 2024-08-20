#include "stdafx.h"
#include "..\Public\Level_Loby.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Terrain.h"
#include "Load_Manager.h"
#include "HpBar.h"
#include "Level_Loading.h"
#include "Player.h"
#include "FireLight.h"
#include "StageWall.h"

CLevel_Loby::CLevel_Loby(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Loby::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
	{
		MSGBOX("Ready_Light returned E_FAIL in CLevel_Loby::NativeConstruct");
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	{
		MSGBOX("Ready_Layer_Player returned E_FAIL in CLevel_Loby::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player_Hand(TEXT("Layer_Player_Hand"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Ocean(TEXT("Layer_Ocean"))))
	{
		MSGBOX("Ready_Layer_Ocean returned E_FAIL in CLevel_Loby::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Meshes(TEXT("Layer_Mesh"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
		return E_FAIL;

	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(43.8f, 3.4f, 20.9f, 1.f));
	static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Portal"), TEXT("Prototype_GameObject_StageWall"))))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CAbyssalSerpent::Die");
		return E_FAIL;
	}
	static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Portal")))->WallSetting(19, CStageWall::LINE_AB, true);

	g_pGameInstance->PlayBGM(TEXT("Stage_Loby.mp3"), 0.5f);
	return S_OK;
}
unsigned int APIENTRY ThreadFunctionObjectPool(void* pArg)
{
	CLevel_Loby*		pLoader = (CLevel_Loby*)pArg;


	EnterCriticalSection(&pLoader->Get_CS());
	srand(unsigned(time(NULL)));

	g_pGameInstance->Add_ObjectPool(LEVEL_LOBY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"));
	g_pGameInstance->ThrowToLayer(LEVEL_LOBY, TEXT("Layer_Monster"));

	LeaveCriticalSection(&pLoader->Get_CS());

	return 0;
}

_int CLevel_Loby::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	if (g_pGameInstance->Get_KeyUp(DIK_NUMPADENTER) || true == g_bGoNextLevel)
	{

		g_eCurrentLevel = LEVEL_LOADING;
		if (FAILED(g_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_STAGE3_1))))
		{
			MSGBOX("g_pGameInstance->OpenLevel returned E_FAIL in CLevel_Loby::Tick");
			return -1;
		}
		g_pGameInstance->StopAll();
		g_bGoNextLevel = false;
	}

	

	return 0;
}


_int CLevel_Loby::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	return 0;
}

HRESULT CLevel_Loby::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	//SetWindowText(g_hWnd, TEXT("로비임. "));

	return S_OK;
}

HRESULT CLevel_Loby::Ready_Light()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC		LightDesc;



	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_POINT;
	LightDesc.vDiffuse = _float4(1.f, 0.5f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPosition = _float4(38.9f, 6.6f, 38.3f, 1.f);
	LightDesc.fRange = 20.f;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBY, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Light");
		return E_FAIL;
	}
	static_cast<CFireLight*>(g_pGameInstance->Get_Back(LEVEL_LOBY, TEXT("Layer_FireLight")))->Set_RealTexIndex(1);

	LightDesc.vPosition = _float4(41.9f, 7.6f, 73.8f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBY, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Light");
		return E_FAIL;
	}
	static_cast<CFireLight*>(g_pGameInstance->Get_Back(LEVEL_LOBY, TEXT("Layer_FireLight")))->Set_RealTexIndex(1);

	LightDesc.vPosition = _float4(45.f, 6.6f, 65.4f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBY, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Light");
		return E_FAIL;
	}
	static_cast<CFireLight*>(g_pGameInstance->Get_Back(LEVEL_LOBY, TEXT("Layer_FireLight")))->Set_RealTexIndex(1);

	LightDesc.vPosition = _float4(48.8f, 7.2f, 50.f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBY, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Light");
		return E_FAIL;
	}
	static_cast<CFireLight*>(g_pGameInstance->Get_Back(LEVEL_LOBY, TEXT("Layer_FireLight")))->Set_RealTexIndex(1);

	LightDesc.vPosition = _float4(51.6f, 6.6f, 26.7f, 1.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBY, TEXT("Layer_FireLight"), TEXT("Prototype_GameObject_FireLight"), &LightDesc.vPosition)))
	{
		__debugbreak();
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Light");
		return E_FAIL;
	}
	static_cast<CFireLight*>(g_pGameInstance->Get_Back(LEVEL_LOBY, TEXT("Layer_FireLight")))->Set_RealTexIndex(1);


	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTTIONAL;
	LightDesc.vDiffuse = _float4(0.26f, 0.33f, 0.45f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(0.f, -1.f, -1.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
	{
		MSGBOX("pGameInstance->Add_Light returned E_FAIL in CLevel_Loby::Ready_Light");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Loby::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* 사본객체를 생성한다. */
	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CameraDesc));

	CameraDesc.vEye = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 1.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &CameraDesc)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_Camera");
		return E_FAIL;
	}
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Loby::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	
	wifstream		fin;

	fin.open(L"../Data/Terrain/Terrain_Loby.txt");

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

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOBY, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_Terrain(GameObject_Terrain)");
			return E_FAIL;
		}
		static_cast<CTerrain*>(g_pGameInstance->Get_Back(LEVEL_LOBY, TEXT("Layer_Terrain")))->Set_Info(vPosition, tInfoTerrain.iTextureIndex, tInfoTerrain.iPass);
		static_cast<CTerrain*>(g_pGameInstance->Get_Back(LEVEL_LOBY, TEXT("Layer_Terrain")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Loby"));
		fin.close();
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Loby::Ready_Layer_Ocean(const _tchar * pLayerTag)
{
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOBY, TEXT("Layer_Ocean"), TEXT("Prototype_GameObject_Ocean"), &XMVectorSet(-32.f, 1.f, -160.f, 1.f))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_Ocean(GameObject_Ocean)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Loby::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Player"))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_Player");
		return E_FAIL;
	}
	static_cast<CPlayer*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Navigation(TEXT("Prototype_Component_Navigation_Loby"));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Loby::Ready_Layer_Player_Hand(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Player_Hand"))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_Player_Hand");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Loby::Ready_Layer_Meshes(const _tchar * pLayerTag)
{
	if (FAILED(CLoad_Manager::GetInstance()->Make_GameObject(LEVEL_LOBY)))
	{
		MSGBOX("CLoad_Manager::GetInstance()->Make_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_Meshes");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Loby::Ready_Layer_UI(const _tchar * pLayerTag)
{

	// 체력바 배경
	INFO_UI tInfoUI_HpBackGround;
	ZeroMemory(&tInfoUI_HpBackGround, sizeof(INFO_UI));

	tInfoUI_HpBackGround.fScaleX = 416.f;
	tInfoUI_HpBackGround.fScaleY = 96.f;
	tInfoUI_HpBackGround.fPositionX = 240.f;
	tInfoUI_HpBackGround.fPositionY = 620.f;
	tInfoUI_HpBackGround.fDepth = 4.f;
	tInfoUI_HpBackGround.iTextureIndex = 16;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_HpBackGround)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(HpBackGround)");
		return E_FAIL;
	}

	// 프린스 사진
	INFO_UI tInfoUI_PrincePicture;
	ZeroMemory(&tInfoUI_PrincePicture, sizeof(INFO_UI));

	tInfoUI_PrincePicture.fScaleX = 256.f;
	tInfoUI_PrincePicture.fScaleY = 256.f;
	tInfoUI_PrincePicture.fPositionX = 100.f;
	tInfoUI_PrincePicture.fPositionY = 540.f;
	tInfoUI_PrincePicture.fDepth = 3.f;
	tInfoUI_PrincePicture.iTextureIndex = 30;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_PrincePicture)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_PrincePicture)");
		return E_FAIL;
	}

	// 탭 그림
	INFO_UI tInfoUI_TapPicture;
	ZeroMemory(&tInfoUI_TapPicture, sizeof(INFO_UI));

	tInfoUI_TapPicture.fScaleX = 48.f;
	tInfoUI_TapPicture.fScaleY = 48.f;
	tInfoUI_TapPicture.fPositionX = 140.f;
	tInfoUI_TapPicture.fPositionY = 650.f;
	tInfoUI_TapPicture.fDepth = 3.f;
	tInfoUI_TapPicture.iTextureIndex = 19;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_TapPicture)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_TapPicture)");
		return E_FAIL;
	}

	// 탭 글자
	INFO_UI tInfoUI_TapText;
	ZeroMemory(&tInfoUI_TapText, sizeof(INFO_UI));

	tInfoUI_TapText.fScaleX = 32.f;
	tInfoUI_TapText.fScaleY = 16.f;
	tInfoUI_TapText.fPositionX = 166.f;
	tInfoUI_TapText.fPositionY = 665.f;
	tInfoUI_TapText.fDepth = 2.9f;
	tInfoUI_TapText.iTextureIndex = 21;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_TapText)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_TapText)");
		return E_FAIL;
	}

	// 속박 스킬
	INFO_UI tInfoUI_BindSkill;
	ZeroMemory(&tInfoUI_BindSkill, sizeof(INFO_UI));

	tInfoUI_BindSkill.fScaleX = 64.f;
	tInfoUI_BindSkill.fScaleY = 64.f;
	tInfoUI_BindSkill.fPositionX = 640.f;
	tInfoUI_BindSkill.fPositionY = 600.f;
	tInfoUI_BindSkill.fDepth = 3.f;
	tInfoUI_BindSkill.iTextureIndex = 15;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_BindSkill)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_BindSkill)");
		return E_FAIL;
	}

	// 속박 키
	INFO_UI tInfoUI_BindKey;
	ZeroMemory(&tInfoUI_BindKey, sizeof(INFO_UI));

	tInfoUI_BindKey.fScaleX = 24.f;
	tInfoUI_BindKey.fScaleY = 24.f;
	tInfoUI_BindKey.fPositionX = 680.f;
	tInfoUI_BindKey.fPositionY = 630.f;
	tInfoUI_BindKey.fDepth = 3.f;
	tInfoUI_BindKey.iTextureIndex = 23;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_BindKey)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_BindKey)");
		return E_FAIL;
	}

	// 독연막 배경
	INFO_UI tInfoUI_PoisonSmokeBackGround;
	ZeroMemory(&tInfoUI_PoisonSmokeBackGround, sizeof(INFO_UI));

	tInfoUI_PoisonSmokeBackGround.fScaleX = 64.f;
	tInfoUI_PoisonSmokeBackGround.fScaleY = 64.f;
	tInfoUI_PoisonSmokeBackGround.fPositionX = 970.f;
	tInfoUI_PoisonSmokeBackGround.fPositionY = 620.f;
	tInfoUI_PoisonSmokeBackGround.fDepth = 4.f;
	tInfoUI_PoisonSmokeBackGround.iTextureIndex = 17;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_PoisonSmokeBackGround)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_PoisonSmokeBackGround)");
		return E_FAIL;
	}

	// 독연막 스킬
	INFO_UI tInfoUI_PoisonSmokeSkill;
	ZeroMemory(&tInfoUI_PoisonSmokeSkill, sizeof(INFO_UI));

	tInfoUI_PoisonSmokeSkill.fScaleX = 42.f;
	tInfoUI_PoisonSmokeSkill.fScaleY = 42.f;
	tInfoUI_PoisonSmokeSkill.fPositionX = 970.f;
	tInfoUI_PoisonSmokeSkill.fPositionY = 613.f;
	tInfoUI_PoisonSmokeSkill.fDepth = 3.f;
	tInfoUI_PoisonSmokeSkill.iTextureIndex = 27;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_PoisonSmokeSkill)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_PoisonSmokeSkill)");
		return E_FAIL;
	}
	// 독연막 키
	INFO_UI tInfoUI_PoisonSmokeKey;
	ZeroMemory(&tInfoUI_PoisonSmokeKey, sizeof(INFO_UI));

	tInfoUI_PoisonSmokeKey.fScaleX = 24.f;
	tInfoUI_PoisonSmokeKey.fScaleY = 24.f;
	tInfoUI_PoisonSmokeKey.fPositionX = 1000.f;
	tInfoUI_PoisonSmokeKey.fPositionY = 650.f;
	tInfoUI_PoisonSmokeKey.fDepth = 3.f;
	tInfoUI_PoisonSmokeKey.iTextureIndex = 32;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_PoisonSmokeKey)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_PoisonSmokeKey)");
		return E_FAIL;
	}

	// 대쉬 배경
	INFO_UI tInfoUI_DashBackGround;
	ZeroMemory(&tInfoUI_DashBackGround, sizeof(INFO_UI));

	tInfoUI_DashBackGround.fScaleX = 64.f;
	tInfoUI_DashBackGround.fScaleY = 64.f;
	tInfoUI_DashBackGround.fPositionX = 1040.f;
	tInfoUI_DashBackGround.fPositionY = 620.f;
	tInfoUI_DashBackGround.fDepth = 4.f;
	tInfoUI_DashBackGround.iTextureIndex = 17;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_DashBackGround)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_DashBackGround)");
		return E_FAIL;
	}

	// 대쉬 스킬
	INFO_UI tInfoUI_DashSkill;
	ZeroMemory(&tInfoUI_DashSkill, sizeof(INFO_UI));

	tInfoUI_DashSkill.fScaleX = 48.f;
	tInfoUI_DashSkill.fScaleY = 48.f;
	tInfoUI_DashSkill.fPositionX = 1040.f;
	tInfoUI_DashSkill.fPositionY = 618.f;
	tInfoUI_DashSkill.fDepth = 3.f;
	tInfoUI_DashSkill.iTextureIndex = 14;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_DashSkill)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_DashSkill)");
		return E_FAIL;
	}

	// 대쉬 키
	INFO_UI tInfoUI_DashKey;
	ZeroMemory(&tInfoUI_DashKey, sizeof(INFO_UI));

	tInfoUI_DashKey.fScaleX = 48.f;
	tInfoUI_DashKey.fScaleY = 24.f;
	tInfoUI_DashKey.fPositionX = 1080.f;
	tInfoUI_DashKey.fPositionY = 650.f;
	tInfoUI_DashKey.fDepth = 3.f;
	tInfoUI_DashKey.iTextureIndex = 20;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_DashKey)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_DashKey)");
		return E_FAIL;
	}

	// 무기 배경
	INFO_UI tInfoUI_WeaponBackGround;
	ZeroMemory(&tInfoUI_WeaponBackGround, sizeof(INFO_UI));

	tInfoUI_WeaponBackGround.fScaleX = 224.f;
	tInfoUI_WeaponBackGround.fScaleY = 64.f;
	tInfoUI_WeaponBackGround.fPositionX = 1160.f;
	tInfoUI_WeaponBackGround.fPositionY = 620.f;
	tInfoUI_WeaponBackGround.fDepth = 4.f;
	tInfoUI_WeaponBackGround.iTextureIndex = 22;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_WeaponBackGround)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_WeaponBackGround)");
		return E_FAIL;
	}

	// 코인
	INFO_UI tInfoUI_Coin;
	ZeroMemory(&tInfoUI_Coin, sizeof(INFO_UI));

	tInfoUI_Coin.fScaleX = 32.f;
	tInfoUI_Coin.fScaleY = 32.f;
	tInfoUI_Coin.fPositionX = 980.f;
	tInfoUI_Coin.fPositionY = 20.f;
	tInfoUI_Coin.fDepth = 3.f;
	tInfoUI_Coin.iTextureIndex = 51;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI"), &tInfoUI_Coin)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_Coin)");
		return E_FAIL;
	}

	// 에임
	INFO_UI tInfoUI_Aim;
	ZeroMemory(&tInfoUI_Aim, sizeof(INFO_UI));

	tInfoUI_Aim.fScaleX = 150.f;
	tInfoUI_Aim.fScaleY = 150.f;
	tInfoUI_Aim.fPositionX = 640.f;
	tInfoUI_Aim.fPositionY = 360.f;
	tInfoUI_Aim.fDepth = 2.f;
	tInfoUI_Aim.iTextureIndex = 51;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_UI_Aim"), &tInfoUI_Aim)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_Aim)");
		return E_FAIL;
	}

	// 미니맵
	INFO_UI tInfoUI_MiniMap;
	ZeroMemory(&tInfoUI_MiniMap, sizeof(INFO_UI));

	tInfoUI_MiniMap.fScaleX = 180.f;
	tInfoUI_MiniMap.fScaleY = 180.f;
	tInfoUI_MiniMap.fPositionX = 110.f;
	tInfoUI_MiniMap.fPositionY = 110.f;
	tInfoUI_MiniMap.fDepth = 4.f;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_MiniMap"), TEXT("Prototype_GameObject_MiniMap"), &tInfoUI_MiniMap)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_MiniMap)");
		return E_FAIL;
	}

	// 스테이지 이름
	INFO_UI tInfoUI_StageName = {};
	ZeroMemory(&tInfoUI_StageName, sizeof(INFO_UI));

	tInfoUI_StageName.iTextureIndex = 0;
	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_LOBY, TEXT("Layer_StageNameUI"), TEXT("Prototype_GameObject_StageNameUI"), &tInfoUI_StageName)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_UI(tInfoUI_StageName)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Loby::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Loby::Ready_Layer_Sky(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loby::Ready_Layer_Sky");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_Loby * CLevel_Loby::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Loby*	pInstance = new CLevel_Loby(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_Loby");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loby::Free()
{
	__super::Free();

}
