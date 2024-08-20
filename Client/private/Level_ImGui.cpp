#include "stdafx.h"
bool	g_bRender = true;
#include "..\public\Level_ImGui.h"
#ifdef USE_IMGUI
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Dynamic.h"
#include "Terrain.h"
#include "BackGround.h"
#include "UI_LoadingFire.h"
#include "Camera_Dynamic.h"
#include "Sky.h"
#include "Mesh.h"
#include "ImGui_Animation.h"
#include "Collider.h"
#include "CellPoint.h"
#include <fstream>
#ifdef USE_IMGUI

#include "ImGui_Manager.h"

#endif



CLevel_ImGui::CLevel_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
#ifdef USE_IMGUI
	, m_pImGui_Manager(CImGui_Manager::GetInstance())
#endif
{
#ifdef USE_IMGUI
	Safe_AddRef(m_pImGui_Manager);
#endif
}

HRESULT CLevel_ImGui::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;
#ifdef USE_IMGUI
	m_pImGui_Manager->Initialize(m_pDevice, m_pDeviceContext);
#endif

	if (FAILED(Ready_Light()))
	{
		MSGBOX("Ready_Light returned E_FAIL in CLevel_ImGui::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Component()))
	{
		MSGBOX("Ready_Component returned E_FAIL in CLevel_ImGui::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_GameObject()))
	{
		MSGBOX("Ready_Prototype_GameObject returned E_FAIL in CLevel_ImGui::NativeConstruct");
		return E_FAIL;
	}



	/* 현재 레벨이 생성될 때, 레벨에서 사용하고자하는 사본객체를 생성한다. */
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
	//	return E_FAIL;

	return S_OK;
}

_int CLevel_ImGui::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

#ifdef USE_IMGUI
	m_pImGui_Manager->Tick();
#endif

	return 0;
}

_int CLevel_ImGui::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	//if (GetKeyState(VK_RETURN) & 0x8000)
	//{
	//	CGameInstance*	pGameInstance = CGameInstance::GetInstance();
	//	Safe_AddRef(pGameInstance);

	//	if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_LOBY))))
	//		return E_FAIL;

	//	Safe_Release(pGameInstance);
	//}

	return 0;
}

HRESULT CLevel_ImGui::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef USE_IMGUI
	m_pImGui_Manager->Render();
#endif

	//SetWindowText(g_hWnd, TEXT("로고레벨임. "));

	return S_OK;
}

HRESULT CLevel_ImGui::Ready_Light()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC		LightDesc;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTTIONAL;
	LightDesc.vDiffuse = _float4(0.52f, 0.65f, 0.9f, 1.f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
	{
		MSGBOX("pGameInstance->Add_Light returned E_FAIL in CLevel_ImGui::Ready_Light");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ImGui::Ready_Component()
{
	// 브러쉬 텍스쳐 컴포넌트
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Brush"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Brush.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLevel_ImGui::Ready_Component");
		return E_FAIL;
	}

	if (FAILED(Load_Models()))
	{
		MSGBOX("Load_Models returned E_FAIL in CLevel_ImGui::Ready_Component");
		return E_FAIL;
	}

	if (FAILED(Load_AnimModels()))
	{
		MSGBOX("Load_AnimModels returned E_FAIL in CLevel_ImGui::Ready_Component");
		return E_FAIL;
	}

	//// Collider
	//if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
	//{
	//	MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLevel_ImGui::Ready_Component");
	//	return E_FAIL;
	//}

	// Model Shader
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/NonAnimModel.hlsl"), VTXNONANIMMODELTEX_DECLARATION::Elements, VTXNONANIMMODELTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	// Shader_AnimModel
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/AnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	// VIBuffer_Terrain
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_ImGui::Ready_Component");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_ImGui::Ready_Prototype_GameObject()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* 원본객체 생성 */
	// GameObject Terrain
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLevel_ImGui::Ready_Prototype_GameObject(GameObject_Terrain)");
		return E_FAIL;
	}
	// GameObject Camera
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"), CCamera_Dynamic::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLevel_ImGui::Ready_Prototype_GameObject(GameObject_Camera)");
		return E_FAIL;
	}
	// GameObject Mesh
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mesh"), CMesh::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLevel_ImGui::Ready_Prototype_GameObject(GameObject_Mesh)");
		return E_FAIL;
	}

	// GameObject AnimationModel
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimationModel"), CImGui_Animation::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLevel_ImGui::Ready_Prototype_GameObject(GameObject_AnimationModel)");
		return E_FAIL;
	}

	// GameObject CellPoint
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CellPoint"), CCellPoint::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLevel_ImGui::Ready_Prototype_GameObject(GameObject_CCellPoint)");
		return E_FAIL;
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ImGui::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* 사본객체를 생성ㅎ나다. */
	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CameraDesc));

	CameraDesc.vEye = _float3(0.f, 3.f, -3.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);


	// GameObject Camera
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Dynamic"), &CameraDesc)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_ImGui::Ready_Layer_Camera(GameObject_Camera)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ImGui::Ready_Layer_Sky(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// VIBuffer_Cube
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOBY, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_VIBuffer_Cube)");
		return E_FAIL;
	}

	// Texture_Sky
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOBY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/SkyBox/Burger%d.dds"), 4))))
		return E_FAIL;
	// Shader
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOBY, TEXT("Prototype_Component_Shader_Cube"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	// GameObject Sky
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLevel_ImGui::Loading_ForLoby(GameObject_Sky)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBY, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_ImGui::Ready_Layer_Sky");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_ImGui::Load_Models()
{
	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Texture/Mesh/*.*", &fd);
	if (handle == -1 || handle == 0)
		return E_FAIL;

	int iResult = 0;
	int i = 0;
	char szFilename[MAX_PATH];

	while (iResult != -1)
	{
		char szFullPath[MAX_PATH] = "../Bin/Texture/Mesh/";

		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		if (!strcmp(szFilename, ".") || !strcmp(szFilename, ".."))
		{
			iResult = _tfindnext64(handle, &fd);
			continue;
		}

		sprintf_s(szFilename, szFilename);



		// 불러온 모델들 프로토타입 선언

		char szPrototypeNameFront[MAX_PATH] = "Prototype_Component_Model_";
		strcat_s(szPrototypeNameFront, szFilename);

		char szRealFullPath[MAX_PATH] = "";
		char szRealFileName[MAX_PATH] = "";
		strcpy_s(szRealFullPath, "../Bin/Texture/Mesh/");
		strcat_s(szRealFullPath, szFilename);
		strcat_s(szRealFullPath, "/");

		strcpy_s(szRealFileName, szFilename);
		strcat_s(szRealFileName, ".obj");



		_tchar* wszPrototype = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, szPrototypeNameFront, sizeof(char) * MAX_PATH, wszPrototype, sizeof(char) * MAX_PATH);
		m_vPrototypeKey.push_back(wszPrototype);

		_matrix		TransformMatrix;

		TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationX(XMConvertToRadians(90.f));

		if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, wszPrototype, CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, szRealFullPath, szRealFileName, TransformMatrix))))
		{
			MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CImGui_Manager::Load_Models");
			_findclose(handle);
			return E_FAIL;
		}

		// 불러온 모델들 텍스쳐 프로토타입 선언
		char szTextureFileName[MAX_PATH] = "";
		_splitpath_s(szFilename, nullptr, 0, nullptr, 0, szTextureFileName, MAX_PATH, nullptr, 0);
		char szTexturePrototypeNameFront[MAX_PATH] = "Prototype_Component_Texture_";
		strcpy_s(szTextureFileName, "../Bin/Texture/Mesh/");
		strcat_s(szTextureFileName, szFilename);
		strcat_s(szTextureFileName, "/");
		strcat_s(szTextureFileName, szFilename);
		strcat_s(szTextureFileName, ".dds");

		strcat_s(szTexturePrototypeNameFront, szFilename);
		_tchar wszTextureFileName[MAX_PATH] = L"";
		MultiByteToWideChar(CP_ACP, 0, szTextureFileName, sizeof(char) * MAX_PATH, wszTextureFileName, sizeof(_tchar) * MAX_PATH);

		_tchar* wszTexturePrototype = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, szTexturePrototypeNameFront, sizeof(char) * MAX_PATH, wszTexturePrototype, sizeof(char) * MAX_PATH);
		m_vPrototypeKey.push_back(wszTexturePrototype);
		if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_IMGUI, wszTexturePrototype, CTexture::Create(m_pDevice, m_pDeviceContext, wszTextureFileName))))
		{
			MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CImGui_Manager::Load_Models(Texture)");
			_findclose(handle);
			return E_FAIL;
		}

		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	return S_OK;
}

HRESULT CLevel_ImGui::Load_AnimModels()
{
	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Texture/Animation/*.*", &fd);
	if (handle == -1 || handle == 0)
		return E_FAIL;

	int iResult = 0;
	int i = 0;
	char szFilename[MAX_PATH];

	while (iResult != -1)
	{
		char szFullPath[MAX_PATH] = "../Bin/Texture/Animation/";

		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		if (!strcmp(szFilename, ".") || !strcmp(szFilename, ".."))
		{
			iResult = _tfindnext64(handle, &fd);
			continue;
		}

		sprintf_s(szFilename, szFilename);


		// 불러온 모델들 프로토타입 선언

		char szPrototypeNameFront[MAX_PATH] = "Prototype_Component_Model_";
		strcat_s(szPrototypeNameFront, szFilename);

		char szRealFullPath[MAX_PATH] = "";
		char szRealFileName[MAX_PATH] = "";
		strcpy_s(szRealFullPath, "../Bin/Texture/Animation/");
		strcat_s(szRealFullPath, szFilename);
		strcat_s(szRealFullPath, "/");

		strcpy_s(szRealFileName, szFilename);
		strcat_s(szRealFileName, ".fbx");



		_tchar* wszPrototype = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, szPrototypeNameFront, sizeof(char) * MAX_PATH, wszPrototype, sizeof(char) * MAX_PATH);
		m_vPrototypeKey.push_back(wszPrototype);

		_matrix		TransformMatrix;

		if (!strcmp(szFilename, "3921"))
			TransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(14.5f)) * XMMatrixTranslation(16.6f, 0.f, 30.53f);
		else
			TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

		if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, wszPrototype, CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, szRealFullPath, szRealFileName, TransformMatrix))))
		{
			MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CImGui_Manager::Load_Models");
			_findclose(handle);
			return E_FAIL;
		}


		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	return S_OK;
}

CLevel_ImGui * CLevel_ImGui::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_ImGui*	pInstance = new CLevel_ImGui(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_ImGui");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_ImGui::Free()
{
	__super::Free();
#ifdef USE_IMGUI
	for (auto& pKey : m_vPrototypeKey)
		Safe_Delete_Array(pKey);
	Safe_Release(m_pImGui_Manager);
	if (0 != CImGui_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CImGui_Manager");
#endif
}
#endif