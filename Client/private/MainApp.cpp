#include "stdafx.h"
#include "..\public\MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_ImGui.h"
#include "BackGround.h"
#include "Cursor.h"
#include "Load_Manager.h"
#include "MonsterWave.h"
#include "Particle_Effect.h"
#include "UI_LoadingFire.h"

LEVEL g_eCurrentLevel = LEVEL_END;
CGameInstance* g_pGameInstance = nullptr;
_bool g_bGoNextLevel = false;
_bool g_bCameraAction = false;

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::NativeConstruct()
{

	CGraphic_Device::GRAPHICDESC GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(CGraphic_Device::GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = CGraphic_Device::MODE_WIN;
	GraphicDesc.iWinCX = g_iWinCX;
	GraphicDesc.iWinCY = g_iWinCY;

	g_pGameInstance = m_pGameInstance;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pDeviceContext)))
	{
		MSGBOX("m_pGameInstance->Initialize_Engine is returned E_FAIL in CMainApp::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_Components()))
	{
		MSGBOX("Ready_Prototype_Components returned E_FAIL in CMainApp::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_GameObjects()))
	{
		MSGBOX("Ready_Prototype_GameObjects returned E_FAIL in CMainApp::NativeConstruct");
		return E_FAIL;
	}

#ifdef USE_IMGUI
	g_eCurrentLevel = LEVEL_IMGUI;
	if (FAILED(Open_Level(LEVEL_IMGUI)))
		return E_FAIL;
#else
	if (FAILED(Open_Level(LEVEL_MAINMANU)))
		return E_FAIL;
#endif


	return S_OK;
}

_int CMainApp::Tick(_double dTimeDelta)
{
#ifndef USE_IMGUI
	ShowCursor(false);
#endif
	if (nullptr == m_pGameInstance)
		return -1;


	m_dTimerAcc += dTimeDelta;


	if (0 > m_pGameInstance->Tick_Engine(dTimeDelta))
		return -1;

	CMonsterWave::GetInstance()->Update();

	return _int();
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
	{
		MSGBOX("m_pGameInstance is nullptr in CMainApp::Render");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(_float4(0.5f, 0.5f, 0.5f, 1.f))))
	{
		MSGBOX("m_pGameInstance->Clear_BackBuffer_View returned E_FAIL in CMainApp::Render");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
	{
		MSGBOX("m_pGameInstance->Clear_DepthStencil_View returned E_FAIL in CMainApp::Render");
		return E_FAIL;
	}


	if (FAILED(m_pRenderer->Render()))
	{
		MSGBOX("m_pRenderer->Render returned E_FAIL in CMainApp::Render");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Render_Level()))
	{
		MSGBOX("m_pGameInstance->Render_Level returned E_FAIL in CMainApp::Render");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Present()))
	{
		MSGBOX("m_pGameInstance->Present returned E_FAIL in CMainApp::Render");
		return E_FAIL;
	}



	++m_dwNumRender;

	if (m_dTimerAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_dwNumRender);
		SetWindowText(g_hWnd, m_szFPS);

		m_dwNumRender = 0;
		m_dTimerAcc = 0.f;
	}



	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	HRESULT hr = 0;

	switch (eLevelIndex)
	{
	case LEVEL_MAINMANU:
		hr = m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eLevelIndex));
		break;
#ifdef USE_IMGUI
	case LEVEL_IMGUI:
		hr = m_pGameInstance->OpenLevel(eLevelIndex, CLevel_ImGui::Create(m_pDevice, m_pDeviceContext));
		break;
#endif
	default:
		hr = m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eLevelIndex));
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Components()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// 트랜스폼 컴포넌트 원형 생성
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), CTransform::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Components(Transform)");
		return E_FAIL;
	}

	// 랜더러 컴포넌트 원형 생성
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Components(Renderer)");
		return E_FAIL;
	}

	// 셰이더 컴포넌트 원형 생성
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Components(Shader)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/VtxNorTex.hlsl"), VTXNORMALTEX_DECLARATION::Elements, VTXNORMALTEX_DECLARATION::iNumElements))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Components(Shader)");
		return E_FAIL;
	}
	// 버퍼 렉트 컴포넌트 원형 생성
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	// 버퍼 렉트 컴포넌트 원형 생성
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Components(VIBuffer_Terrain)");
		return E_FAIL;
	}
	// Collider
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Components(Collider)");
		return E_FAIL;
	}
	// 커서 텍스쳐 컴포넌트 원형 생성
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Cursor.png")))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Components(Texture_Cursor)");
		return E_FAIL;
	}

	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Texture/UI/*.*", &fd);
	if (handle == -1 || handle == 0)
		return E_FAIL;;

	int iResult = 0;
	int i = 0;
	while (iResult != -1)
	{
		i++;
		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	// 로딩 텍스쳐 컴포넌트 원형 생성
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/UI/TexUI%d.dds"), i - 2))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Components(Texture_UI)");
		return E_FAIL;
	}


	{
		_tfinddata64_t terrainfd;
		__int64 handle = _tfindfirst64(L"../Bin/Texture/Terrain/*.*", &terrainfd);
		if (handle == -1 || handle == 0)
			return E_FAIL;

		int iResult = 0;
		int i = 0;

		while (iResult != -1)
		{
			i++;

			iResult = _tfindnext64(handle, &terrainfd);
		}

		_findclose(handle);

		// Texture Terrain
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Terrain/terrain%d.dds"), i - 2))))
		{
			MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLevel_ImGui::Ready_Prototype_GameObject(Texture_Terrain)");
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	// VIBuffer_Point_Instance_10
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_10"), CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, 10))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Component(Component_VIBuffer_Point_Instance_10)");
		return E_FAIL;
	}
	// VIBuffer_Point_Instance_30
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_30"), CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, 30))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Component(Component_VIBuffer_Point_Instance_30)");
		return E_FAIL;
	}
	// VIBuffer_Point_Instance_50
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_50"), CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, 50))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Component(Component_VIBuffer_Point_Instance_50)");
		return E_FAIL;
	}
	// VIBuffer_Point_Instance_100
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_100"), CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, 100))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Component(Component_VIBuffer_Point_Instance_100)");
		return E_FAIL;
	}

	// 파티클이펙트 텍스쳐 컴포넌트 원형 생성
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Particle_Effect"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Particle/Particle%d.dds"), MAX_PATICLETEXTURE))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_Components(Texture_Particle_Effect)");
		return E_FAIL;
	}

	Safe_AddRef(m_pRenderer);
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObjects()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//* For.Prototype_GameObject_UI */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"), CBackGround::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_GameObjects(CBackGround)");
		return E_FAIL;
	}

	//* For.Prototype_GameObject_Particle_Effect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Effect"), CParticle_Effect::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CMainApp::Ready_Prototype_GameObjects(Particle_Effect)");
		return E_FAIL;
	}
	// GameObject UI
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LoadingFire"), CUI_LoadingFire::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLevel_Loading::Load_LoadingData(GameObject_UI_LoadingFire)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	CMonsterWave::GetInstance()->DestroyInstance();
	CLoad_Manager::GetInstance()->DestroyInstance();
	CGameInstance::Release_Engine();
}