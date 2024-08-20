#include "stdafx.h"
#include "..\public\ImGui_Manager.h"
#include <fstream>
#include <string>
#include "GameInstance.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Mesh.h"
#include "ImGui_Animation.h"
#include "HierarchyNode.h"
#include "CellPoint.h"
#include "VIBuffer_Triangle.h"
#include "PipeLine.h"
#include "Shader.h"

#ifdef USE_IMGUI

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

void CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pDeviceContext);



#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pVIBuffer)
	{
		MSGBOX("nullptr == m_pVIBuffer in CImGui_Manager::Initialize");
		return;
	}

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pShader in CImGui_Manager::Initialize");
		return;
	}

	/* For.Prototype_Component_Shader_PointInstance*/
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_PointInstance"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointINstance.hlsl"), VTXPOINT_MATRIX_DECLARATION::Elements, VTXPOINT_MATRIX_DECLARATION::iNumElements))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CImGui_Manager::Initialize(Point_Instance_Shader)");
		return;
	}
#endif
}

void CImGui_Manager::Tick()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	if (show_tool)
	{
		if (ImGui::Begin("Tool Window", &show_tool, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Close"))
						show_tool = false;
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Terrain"))
				{
					static _int iTerrainIndex = -1;
					Terrain_Names(iTerrainIndex);
					ImGui::SameLine();
					iTerrainIndex = Terrain_List();

					Terrain_Texture_List();

					Terrain_Info(iTerrainIndex);

					_int iLoadIndex = Terrain_Load_List();
					if (ImGui::Button("Load", ImVec2(100.f, 40.f)))
						Load_Terrain(iLoadIndex);

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Mesh"))
				{
					static _int iMeshIndex = -1;
					Mesh_Names(iMeshIndex);
					ImGui::SameLine();
					iMeshIndex = Mesh_List();

					Mesh_Model_List();

					Mesh_Info(iMeshIndex);

					_int iLoadIndex = Mesh_Load_List();
					if (ImGui::Button("Load", ImVec2(100.f, 40.f)))
						Load_Mesh(iLoadIndex);

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Animation"))
				{
					if (nullptr == m_pAnimationModel)
						Collider_Create();
					else
					{
						Model_List();
						Collider_BoneList();
						Animation_List();
						AnimModelTransform();
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Camera"))
				{
					static int selected = 0;
					{
						ImGui::BeginChild("left pane", ImVec2(150, 0), true);
						for (int i = 0; i < 100; i++)
						{
							// FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
							char label[128];
							sprintf_s(label, "Camera %d", i);
							if (ImGui::Selectable(label, selected == i))
								selected = i;
						}
						ImGui::EndChild();
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Particle"))
				{
					Particle_Info();
					

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("UI"))		// UI탭
				{
					static _int iIndex = -1;
					UI_Names(iIndex);
					ImGui::SameLine();
					iIndex = UI_List();

					UI_Texture_List();

					UI_Info(iIndex);

					_int iLoadIndex = UI_Load_List();
					if (ImGui::Button("Load", ImVec2(100.f, 40.f)))
						Load_UI(iLoadIndex);
					
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("NaviMesh"))
				{
					NaviMesh_List();
					NaviMesh_Make();
					NaviMesh_Load_List();
					if (ImGui::Button("LoadNavi"))
					{
						Load_NaviMesh();
					}

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}
}

void CImGui_Manager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CNavigation::Render(World)");
		return;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &g_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CNavigation::Render(View)");
		return;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &g_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CNavigation::Render(Proj)");
		return;
	}

	if (true == m_bNaviMeshRender)
		for (auto& pCell : m_Cells)
		{
			pCell->Render(m_pVIBuffer, m_pShader, _float4(0.f, 1.f, 0.f, 1.f));
		}
}

void CImGui_Manager::Terrain_Names(_int iTerrainListIndex)
{
	ImGui::BeginChild("pane", ImVec2(300.f, 150.f), true);
	ImGui::InputText("TerrainFileName", m_szTerrainFileName, IM_ARRAYSIZE(m_szTerrainFileName));
	if (ImGui::Button("Create"))
	{
		GROUPTERRAIN tGroupTerrain;
		ZeroMemory(&tGroupTerrain, sizeof(GROUPTERRAIN));
		strcpy_s(tGroupTerrain.szTerrainFileName, m_szTerrainFileName);
		m_vGroupTerrain.push_back(tGroupTerrain);
	}
	ImGui::InputText("TerrainChildName", m_szTerrainChildName, IM_ARRAYSIZE(m_szTerrainChildName));
	if (ImGui::Button("CreateChild"))
	{
		if (0 < m_vGroupTerrain.size() && iTerrainListIndex >= 0)
		{
			INFO_TERRAIN* tInfoTerrain = new INFO_TERRAIN;
			strcpy_s(tInfoTerrain->szChildName, m_szTerrainChildName);
			m_vGroupTerrain[iTerrainListIndex].vInfo_Terrain.push_back(tInfoTerrain);
			m_pInfoTerrain = m_vGroupTerrain[iTerrainListIndex].vInfo_Terrain.back();

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_Terrain")
				, TEXT("Prototype_GameObject_Terrain"), m_vGroupTerrain[iTerrainListIndex].vInfo_Terrain.back())))
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Terrain_Names");
			RELEASE_INSTANCE(CGameInstance);

			char* szTerrainName = new char[MAX_PATH];
			strcpy_s(szTerrainName, MAX_PATH, m_vGroupTerrain[iTerrainListIndex].szTerrainFileName);
			strcat_s(szTerrainName, MAX_PATH, tInfoTerrain->szChildName);
			m_vFirst.push_back(szTerrainName);
			CTerrain* pTerrain = (CTerrain*)g_pGameInstance->Get_Back(LEVEL_IMGUI, TEXT("Layer_Terrain"));

			m_Terrains.emplace(szTerrainName, pTerrain);
			m_pSelectTerrain = pTerrain;
		}
	}
	ImGui::EndChild();
}

void CImGui_Manager::Terrain_Texture_List()
{
	if (nullptr == m_pInfoTerrain)
		return;

	ImGui::Text("Texture List");
	ImGui::BeginChild("left pane", ImVec2(150.f, 300.f), true);

	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Texture/Terrain/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	int iResult = 0;
	int i = 0;
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		if (!strcmp(szFilename, ".") || !strcmp(szFilename, ".."))
		{
			iResult = _tfindnext64(handle, &fd);
			continue;
		}

		sprintf_s(szFilename, szFilename);


		if (ImGui::Selectable(szFilename, m_pInfoTerrain->iTextureIndex == i))
		{
			m_pInfoTerrain->iTextureIndex = i;
		}
		i++;

		iResult = _tfindnext64(handle, &fd);
	}
	

	_findclose(handle);

	ImGui::EndChild();
}

// 터레인 리스트 다루는 함수
_int CImGui_Manager::Terrain_List()
{
	auto iter = m_vGroupTerrain.begin();
	
	// 그룹 인덱스
	static int Terrainselected = 0;
	static int TerrainChildSelected = 0;
	{
		ImGui::BeginChild("top pane", ImVec2(250.f, 150.f), true);
		ImGui::Text("Terrain List");
		ImGui::BeginChild("top in pane", ImVec2(235.f, 90.f), true);

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		for (size_t i = 0; i < m_vGroupTerrain.size(); i++)
		{
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = (Terrainselected == i);	// 현재 i와 내가 선택한 터레인 그룹이 같은지
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			// 트리가 열렸는지 여부
			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, m_vGroupTerrain[i].szTerrainFileName);
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				Terrainselected = (_int)i;

			if (node_open)
			{
				for (_int j = 0; j < m_vGroupTerrain[i].vInfo_Terrain.size(); j++)
				{
					// 선택한 터레인 정보 가져오기
					if (nullptr != m_pInfoTerrain)
						if (ImGui::Selectable(m_vGroupTerrain[i].vInfo_Terrain[j]->szChildName, !strcmp(m_pInfoTerrain->szChildName, m_vGroupTerrain[i].vInfo_Terrain[j]->szChildName)))
						{
							TerrainChildSelected = j;
							m_pInfoTerrain = m_vGroupTerrain[i].vInfo_Terrain[j];
							m_pSelectTerrain = Find_Terrain(m_vGroupTerrain[i].szTerrainFileName, m_pInfoTerrain->szChildName);
							break;
						}
				}
				ImGui::TreePop();
			}
		}
		ImGui::EndChild();

		if (ImGui::Button("Terrain_Delete") && nullptr != m_pSelectTerrain && nullptr != m_pInfoTerrain)
		{
			// 터레인 없앰
			m_pSelectTerrain->Set_Dead(true);

			// 맵컨테이너에서 터레인 찾기
			char* szTerrainName = new char[MAX_PATH];
			strcpy_s(szTerrainName, MAX_PATH, m_vGroupTerrain[Terrainselected].szTerrainFileName);
			strcat_s(szTerrainName, MAX_PATH, m_pInfoTerrain->szChildName);

			_int	i = 0;
			for (; i < m_vFirst.size(); ++i)
			{
				if (!strcmp(m_vFirst[i], szTerrainName))
					break;
				if (i == m_vFirst.size() - 1)
				{
					i = -1;
					break;
				}
			}
			
			// 맵컨테이너에서 터레인 소멸
			if (i != -1)
				m_Terrains.erase(m_vFirst[i]);
			Safe_Delete_Array(szTerrainName);
			m_pSelectTerrain = nullptr;

			// 터레인 정보도 반환
			Safe_Delete(m_pInfoTerrain);
			

			auto Info_Iter = m_vGroupTerrain[Terrainselected].vInfo_Terrain.begin();
			for (_int i = 0; i < TerrainChildSelected; i++)
				++Info_Iter;
			m_vGroupTerrain[Terrainselected].vInfo_Terrain.erase(Info_Iter);

			if (0 < m_vGroupTerrain[Terrainselected].vInfo_Terrain.size())
				m_pInfoTerrain = m_vGroupTerrain[Terrainselected].vInfo_Terrain.front();
			else
				m_pInfoTerrain = nullptr;
		}

		ImGui::SameLine();

		if (ImGui::Button("Group_Delete") && false == m_vGroupTerrain.empty() && 0 == m_vGroupTerrain[Terrainselected].vInfo_Terrain.size())
		{
			auto Group_Iter = m_vGroupTerrain.begin();
			for (_int i = 0; i < Terrainselected; i++)
				++Group_Iter;
			m_vGroupTerrain.erase(Group_Iter);
			Terrainselected = 0;
		}

		ImGui::EndChild();
	}


	return Terrainselected;
}

void CImGui_Manager::Terrain_Info(_int iIndex)
{
	if (nullptr == m_pInfoTerrain)
		return;
	ImGui::SameLine();
	ImGui::BeginChild("Right pane", ImVec2(300.f, 300.f));
	ImGui::Text(m_pInfoTerrain->szChildName);
	if (strcmp(m_pInfoTerrain->szChildName, ""))
	{
		ImGui::InputInt("VecticesX", &m_pInfoTerrain->iNumVerticesX, 1, 1);
		ImGui::InputInt("VecticesZ", &m_pInfoTerrain->iNumVerticesZ, 1, 1);
		if (2 > m_pInfoTerrain->iNumVerticesX)
			m_pInfoTerrain->iNumVerticesX = 2;
		if (2 > m_pInfoTerrain->iNumVerticesZ)
			m_pInfoTerrain->iNumVerticesZ = 2;

		ImGui::Text("");
		ImGui::InputFloat("TileNumX", &m_pInfoTerrain->fTileNumX, 0.01f, 1.0f, "%.3f");
		ImGui::InputFloat("TileNumZ", &m_pInfoTerrain->fTileNumZ, 0.01f, 1.0f, "%.3f");
		if (1 > m_pInfoTerrain->fTileNumX)
			m_pInfoTerrain->iNumVerticesX = 1;
		if (1 > m_pInfoTerrain->fTileNumZ)
			m_pInfoTerrain->iNumVerticesZ = 1;
		ImGui::Text("");
		ImGui::InputFloat("PositionX", &m_pInfoTerrain->vPosition.x, 0.01f, 1.0f, "%.3f");
		ImGui::InputFloat("PositionY", &m_pInfoTerrain->vPosition.y, 0.01f, 1.0f, "%.3f");
		ImGui::InputFloat("PositionZ", &m_pInfoTerrain->vPosition.z, 0.01f, 1.0f, "%.3f");
	}

	ImGui::Text("\n");
	ImGui::InputText("SaveFileName", m_szTerrainSaveFileName, IM_ARRAYSIZE(m_szTerrainSaveFileName));
	if (ImGui::Button("Save", ImVec2(100.f, 40.f)))
		Save_Terrain();

	ImGui::SameLine();
	bool bWireFrame = ImGui::Checkbox("WireFrame", &m_bWireFrame);
	ImGui::SameLine();
	bool bBlend = ImGui::Checkbox("Blending", &m_bBlend);
	if (false == m_bWireFrame && false == m_bBlend)
		m_pInfoTerrain->iPass = 1;
	else if (false == m_bWireFrame && true == m_bBlend)
		m_pInfoTerrain->iPass = 0;
	else if (true == m_bWireFrame)
		m_pInfoTerrain->iPass = 2;

	ImGui::Checkbox("BrushOn", &m_bOnBrush);
	if (m_bOnBrush)
		UpDown_Terrain();

	Update_Terrain();

	ImGui::EndChild();

}

_int CImGui_Manager::Terrain_Load_List()
{
	ImGui::Text("Load List");
	ImGui::BeginChild("LoadList", ImVec2(150.f, 300.f), true);

	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Data/Terrain/*.*", &fd);
	if (handle == -1 || handle == 0)
		return -1;

	static _int iSelect = 0;
	int iResult = 0;
	int i = 0;
	char szFilename[64];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		if (!strcmp(szFilename, ".") || !strcmp(szFilename, ".."))
		{
			iResult = _tfindnext64(handle, &fd);
			continue;
		}

		sprintf_s(szFilename, szFilename);


		if (ImGui::Selectable(szFilename, iSelect == i))
		{
			iSelect = i;
			strcpy_s(m_szTerrainLoadFileName, szFilename);
		}
		i++;

		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	ImGui::EndChild();
	return iSelect;
}

CTerrain * CImGui_Manager::Find_Terrain(const char* pGroupTag, const char* pInfoTag)
{
	char szTerrainName[MAX_PATH] = "";
	strcpy_s(szTerrainName, pGroupTag);
	strcat_s(szTerrainName, pInfoTag);

	auto iter = find_if(m_Terrains.begin(), m_Terrains.end(), CTagCharFinder(szTerrainName));

	if (m_Terrains.end() == iter)
		return nullptr;

	return iter->second;
}

void CImGui_Manager::UpDown_Terrain()
{
	m_pSelectTerrain->Set_Radius(m_fBrushRadius);
	ImGui::Checkbox("DefaultMode", &m_bDefaultMode);
	if (m_bDefaultMode)
	{
		m_bSlopeMode = false;
		m_bAngularMode = false;
		m_bTwoPointSlopeMode = false;
		ImGui::InputInt("Sharpness", &m_iShapeness);
		ImGui::InputFloat("DivisibleValue", &m_fDivisibleValue, 0.1f);
		ImGui::DragFloat("MinY", &m_fMinY, 0.1f, -10.f, m_fMaxY);
		ImGui::DragFloat("MaxY", &m_fMaxY, 0.1f, m_fMinY, 90.f);

		m_pSelectTerrain->Terrain_UpDownDefault(m_iShapeness, m_fDivisibleValue, m_fMaxY, m_fMinY);
	}

	ImGui::Text("");
	ImGui::Checkbox("SlopeMode", &m_bSlopeMode);
	if (m_bSlopeMode)
	{
		m_bDefaultMode = false;
		m_bAngularMode = false;
		m_bTwoPointSlopeMode = false;
		ImGui::InputFloat("SlopeStart", &m_fSlopeStart, 0.1f);
		ImGui::InputFloat("SlopeEnd", &m_fSlopeEnd, 0.1f);
		ImGui::InputFloat("SlopeWeight", &m_fSlopeWeight, 0.1f);
		
		if (0 <= m_fSlopeWeight && m_fSlopeStart > m_fSlopeEnd)
			m_fSlopeStart = m_fSlopeEnd;
		else if (0 > m_fSlopeWeight && m_fSlopeStart < m_fSlopeEnd)
			m_fSlopeStart = m_fSlopeEnd;

		if (E_FAIL == m_pSelectTerrain->Terrain_UpDownSlope(m_fSlopeStart))
			m_fSlopeStart += m_fSlopeWeight;
	}

	ImGui::Text("");
	ImGui::Checkbox("AngularMode", &m_bAngularMode);
	if (m_bAngularMode)
	{
		m_bDefaultMode = false;
		m_bSlopeMode = false;
		m_bTwoPointSlopeMode = false;
		m_pSelectTerrain->Terrain_Angular();


	}

	ImGui::Text("");
	ImGui::Checkbox("TwoPointSlopeMode", &m_bTwoPointSlopeMode);
	if (m_bTwoPointSlopeMode)
	{
		m_bDefaultMode = false;
		m_bSlopeMode = false;
		m_bAngularMode = false;
		m_pSelectTerrain->Terrain_TwoPointSlope();

	}
	ImGui::Text("");
	ImGui::InputFloat("Radius", &m_fBrushRadius, 0.1f);

}

void CImGui_Manager::Update_Terrain()
{
	m_pSelectTerrain->Light_Update();
	m_pSelectTerrain->Terrain_Update();
}

void CImGui_Manager::Save_Terrain()
{
	wofstream		fout;

	char szSaveFileName[64] = "../Data/Terrain/";
	strcat_s(szSaveFileName, m_szTerrainSaveFileName);
	strcat_s(szSaveFileName, ".txt");
	_tchar pStr[64] = L"";
	MultiByteToWideChar(CP_ACP, 0, szSaveFileName, IM_ARRAYSIZE(szSaveFileName), pStr, IM_ARRAYSIZE(pStr));

	fout.open(pStr);
	//fout.open(L"../Data/ImgPath.txt");

	// 파일 개방 성공
	if (!fout.fail())
	{
		for (size_t i = 0; i < m_vGroupTerrain.size(); i++)
		{
			wstring wstr1(m_vGroupTerrain[i].szTerrainFileName, &m_vGroupTerrain[i].szTerrainFileName[strlen(m_vGroupTerrain[i].szTerrainFileName)]);
			for (size_t j = 0; j < m_vGroupTerrain[i].vInfo_Terrain.size(); j++)
			{
				wstring wstr2(m_vGroupTerrain[i].vInfo_Terrain[j]->szChildName, &m_vGroupTerrain[i].vInfo_Terrain[j]->szChildName[strlen(m_vGroupTerrain[i].vInfo_Terrain[j]->szChildName)]);

				fout << wstr1 << L"|" << wstr2 << L"|"
					<< m_vGroupTerrain[i].vInfo_Terrain[j]->iNumVerticesX << L"|" << m_vGroupTerrain[i].vInfo_Terrain[j]->iNumVerticesZ << L"|"
					<< m_vGroupTerrain[i].vInfo_Terrain[j]->fTileNumX << L"|" << m_vGroupTerrain[i].vInfo_Terrain[j]->fTileNumZ << L"|"
					<< m_vGroupTerrain[i].vInfo_Terrain[j]->vPosition.x << L"|" << m_vGroupTerrain[i].vInfo_Terrain[j]->vPosition.y << L"|" << m_vGroupTerrain[i].vInfo_Terrain[j]->vPosition.z << L"|"
					<< m_vGroupTerrain[i].vInfo_Terrain[j]->iTextureIndex << L"|"
					<< m_vGroupTerrain[i].vInfo_Terrain[j]->iPass << endl;
			}
		}


		fout.close();		// close 함수는 생략 가능(객체 타입이어서 소멸 시점에 알아서 개방한 파일 또한 소멸 가능)
	}

	// 윈도우의 기본 프로그램을 실행시켜주는 함수
	char szSaveFileNameFinal[64] = "notepad.exe ";
	strcat_s(szSaveFileNameFinal, szSaveFileName);
	WinExec(szSaveFileNameFinal, SW_SHOW);
	//WinExec("notepad.exe ../Data/ImgPath.txt", SW_SHOW);
	m_pSelectTerrain->SaveY(m_szTerrainSaveFileName);
}

void CImGui_Manager::Load_Terrain(_int iLoadIndex)
{
	wifstream		fin;

	char szLoadFileName[64] = "../Data/Terrain/";
	strcat_s(szLoadFileName, m_szTerrainLoadFileName);
	_tchar pStr[64] = L"";
	MultiByteToWideChar(CP_ACP, 0, szLoadFileName, IM_ARRAYSIZE(szLoadFileName), pStr, IM_ARRAYSIZE(pStr));

	fin.open(pStr);

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

		GROUPTERRAIN tGroupTerrain;
		WideCharToMultiByte(CP_ACP, 0, szTerrainFileName, MAX_PATH, tGroupTerrain.szTerrainFileName, MAX_PATH, NULL, NULL);

		INFO_TERRAIN* pInfoTerrain = new INFO_TERRAIN;
		WideCharToMultiByte(CP_ACP, 0, szChildName, MAX_PATH, pInfoTerrain->szChildName, MAX_PATH, NULL, NULL);
		pInfoTerrain->iNumVerticesX = (_int)_tstoi(szVerticesX);
		pInfoTerrain->iNumVerticesZ = (_int)_tstoi(szVerticesZ);
		pInfoTerrain->fTileNumX = (_float)_tstof(szTileNumX);
		pInfoTerrain->fTileNumZ = (_float)_tstof(szTileNumZ);
		pInfoTerrain->vPosition.x = (_float)_tstof(szPosX);
		pInfoTerrain->vPosition.y = (_float)_tstof(szPosY);
		pInfoTerrain->vPosition.z = (_float)_tstof(szPosZ);
		pInfoTerrain->iTextureIndex = (_int)_tstoi(szTexIndex);
		pInfoTerrain->iPass = (_uint)_tstoi(szPass);
		pInfoTerrain->bDelete = true;

		tGroupTerrain.vInfo_Terrain.push_back(pInfoTerrain);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (true == m_vGroupTerrain.empty() || strcmp(m_vGroupTerrain.back().szTerrainFileName, tGroupTerrain.szTerrainFileName))
			m_vGroupTerrain.push_back(tGroupTerrain);
		else
			m_vGroupTerrain.back().vInfo_Terrain.push_back(pInfoTerrain);

		char szDataFileName[MAX_PATH] = "../Data/";
		char szDataName[MAX_PATH] = "";
		_splitpath_s(szLoadFileName, nullptr, 0, nullptr, 0, szDataName, MAX_PATH, nullptr, 0);
		strcat_s(szDataFileName, szDataName);
		strcat_s(szDataFileName, ".dat");
		_tchar wszTerrainSaveFileName[MAX_PATH] = L"";

		char szPrototypeName[MAX_PATH] = "Prototype_Component_VIBuffer_Terrain_";
		strcat_s(szPrototypeName, pInfoTerrain->szChildName);

		_tchar* wszPrototypeName = new _tchar[MAX_PATH];
		m_vLFirst.push_back(wszPrototypeName);

		MultiByteToWideChar(CP_ACP, 0, szDataFileName, MAX_PATH, wszTerrainSaveFileName, sizeof(_tchar) * MAX_PATH);

		MultiByteToWideChar(CP_ACP, 0, szPrototypeName, MAX_PATH, wszPrototypeName, sizeof(_tchar) * MAX_PATH);


		if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_IMGUI, wszPrototypeName, CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, wszTerrainSaveFileName))))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLevel_Loading::Load_Terrain");
			return;
		}

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_IMGUI
			, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), pInfoTerrain)))
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Load_Terrain");

		char* szTerrainName = new char[MAX_PATH];
		strcpy_s(szTerrainName, MAX_PATH, m_vGroupTerrain[m_vGroupTerrain.size() - 1].szTerrainFileName);
		strcat_s(szTerrainName, MAX_PATH, pInfoTerrain->szChildName);
		m_vFirst.push_back(szTerrainName);
		CTerrain* pTerrain = (CTerrain*)g_pGameInstance->Get_Back(LEVEL_IMGUI, TEXT("Layer_Terrain"));

		m_Terrains.emplace(szTerrainName, pTerrain);
		m_pSelectTerrain = pTerrain;
		m_pSelectTerrain->Set_Info(_float3(pInfoTerrain->vPosition.x, pInfoTerrain->vPosition.y, pInfoTerrain->vPosition.z), pInfoTerrain->iTextureIndex, pInfoTerrain->iPass);

		RELEASE_INSTANCE(CGameInstance);

		m_pInfoTerrain = m_vGroupTerrain[0].vInfo_Terrain[0];
		fin.close();
	}
	char szLoadFileNameFinal[64] = "notepad.exe ";
	strcat_s(szLoadFileNameFinal, szLoadFileName);
	WinExec(szLoadFileNameFinal, SW_SHOW);
}


// 매쉬
void CImGui_Manager::Mesh_Names(_int iMeshListIndex)
{
	ImGui::BeginChild("pane", ImVec2(300.f, 150.f), true);

	ImGui::InputText("MeshFileName", m_szMeshFileName, IM_ARRAYSIZE(m_szMeshFileName));
	if (ImGui::Button("Create"))
	{
		// 매쉬 정보 동적 할당해서 매쉬정보 컨테이너에 넣어줌
		INFO_MESH* pInfoMesh = new INFO_MESH;
		strcpy_s(pInfoMesh->szFileName, m_szMeshFileName);
		strcpy_s(pInfoMesh->szFullPath, "../Bin/Texture/Mesh/terrain_mountain_301/terrain_mountain_301.obj");
		pInfoMesh->vScale[0] = 1.f;
		pInfoMesh->vScale[1] = 1.f;
		pInfoMesh->vScale[2] = 1.f;
		XMStoreFloat4x4(&pInfoMesh->vWorldMatrix, XMMatrixIdentity());
		m_pMeshesInfo.push_back(pInfoMesh);
		m_pCurrentInfoMesh = m_pMeshesInfo.back();

		// Create 누르면 매쉬 생성

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_Mesh"), TEXT("Prototype_GameObject_Mesh"), m_pMeshesInfo.back())))
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Mesh_Names");

		// 키값 컨테이너에 추가
		m_vMeshesFirst.push_back(m_pCurrentInfoMesh->szFileName);
		CMesh* pMesh = (CMesh*)g_pGameInstance->Get_Back(LEVEL_IMGUI, TEXT("Layer_Mesh"));

		// 매쉬 컨테이너에 추가
		m_Meshs.emplace(m_pCurrentInfoMesh->szFileName, pMesh);

		// 현재 매쉬 갱신
		m_pSelectMesh = pMesh;

		// 모델 넘겨줌
		m_pSelectMesh->Set_Model(TEXT("Prototype_Component_Model_terrain_mountain_301"));
		m_pSelectMesh->Set_Tex(TEXT("Prototype_Component_Texture_terrain_mountain_301"));
	}
	ImGui::EndChild();
}

void CImGui_Manager::Mesh_Model_List()
{
	if (nullptr == m_pCurrentInfoMesh)
		return;

	ImGui::Text("Model List");
	ImGui::BeginChild("left pane", ImVec2(200.f, 300.f), true);

	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Texture/Mesh/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

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

		strcat_s(szFullPath, "/");
		strcat_s(szFullPath, szFilename);
		strcat_s(szFullPath, ".obj");

		// 선택한 모델을 픽함
		if (ImGui::Selectable(szFilename, m_pCurrentInfoMesh->iTextureIndex == i))
		{
			m_pCurrentInfoMesh->iTextureIndex = i;
			
			strcpy_s(m_pCurrentInfoMesh->szFullPath, szFullPath);

			// 모델, 텍스쳐 갱신
			char szNameBack[MAX_PATH] = "";
			_splitpath_s(szFilename, nullptr, 0, nullptr, 0, szNameBack, MAX_PATH, nullptr, 0);
			char szNameFront[MAX_PATH] = "Prototype_Component_Model_";
			char szTexNameFront[MAX_PATH] = "Prototype_Component_Texture_";

			strcat_s(szNameFront, szNameBack);
			strcat_s(szTexNameFront, szNameBack);

			_tchar wszPrototypeName[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szNameFront, IM_ARRAYSIZE(szNameFront), wszPrototypeName, IM_ARRAYSIZE(wszPrototypeName));

			_tchar wszTexturePrototypeName[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szTexNameFront, IM_ARRAYSIZE(szNameFront), wszTexturePrototypeName, IM_ARRAYSIZE(wszTexturePrototypeName));

			m_pSelectMesh->Set_Model(wszPrototypeName);
			m_pSelectMesh->Set_Tex(wszTexturePrototypeName);
		}
		i++;

		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	ImGui::EndChild();
}

_int CImGui_Manager::Mesh_List()
{

	// 매쉬 인덱스
	static int MeshSelected = 0;
	{
		ImGui::BeginChild("top pane", ImVec2(250.f, 150.f), true);
		ImGui::Text("Mesh List");
		ImGui::BeginChild("top in pane", ImVec2(235.f, 90.f), true);
		for (size_t i = 0; i < m_pMeshesInfo.size(); i++)
		{
			// 선택한 매쉬 정보 가져오기
			if (nullptr != m_pCurrentInfoMesh)
				if (ImGui::Selectable(m_pMeshesInfo[i]->szFileName, !strcmp(m_pCurrentInfoMesh->szFileName, m_pMeshesInfo[i]->szFileName)))
				{
					MeshSelected = (_int)i;
					m_pCurrentInfoMesh = m_pMeshesInfo[i];
					m_pSelectMesh = Find_Mesh(m_pCurrentInfoMesh->szFileName);
					break;
				}
		}
		ImGui::EndChild();

		if (ImGui::Button("Delete") && nullptr != m_pSelectMesh && nullptr != m_pCurrentInfoMesh)
		{
			// 매쉬 없앰
			m_pSelectMesh->Set_Dead(true);

			// 맵컨테이너에서 매쉬 찾기

			_int	i = 0;
			for (; i < m_vMeshesFirst.size(); ++i)
			{
				if (!strcmp(m_vMeshesFirst[i], m_pCurrentInfoMesh->szFileName))
					break;
				if (i == m_vMeshesFirst.size() - 1)
				{
					i = -1;
					break;
				}
			}

			// 맵컨테이너에서 매쉬 소멸
			if (i != -1)
				m_Meshs.erase(m_vMeshesFirst[i]);

			m_pSelectMesh = nullptr;

			// 터레인 정보도 반환
			Safe_Delete(m_pCurrentInfoMesh);


			auto Info_Iter = m_pMeshesInfo.begin();
			for (_int i = 0; i < MeshSelected; i++)
				++Info_Iter;
			Info_Iter = m_pMeshesInfo.erase(Info_Iter);

			if (0 < m_pMeshesInfo.size())
			{
				m_pCurrentInfoMesh = m_pMeshesInfo.front();
				m_pSelectMesh = Find_Mesh(m_pCurrentInfoMesh->szFileName);
			}
			else
				m_pCurrentInfoMesh = nullptr;
		}
		ImGui::EndChild();

	}


	return MeshSelected;
}

void CImGui_Manager::Mesh_Info(_int iIndex)
{
	if (nullptr == m_pCurrentInfoMesh)
		return;

	ImGui::SameLine();
	ImGui::BeginChild("Right pane", ImVec2(300.f, 300.f));
	ImGui::Text(m_pCurrentInfoMesh->szFileName);

	CTransform* pTransform = static_cast<CTransform*>(m_pSelectMesh->Get_Component(TEXT("Com_Transform")));

	if (strcmp(m_pCurrentInfoMesh->szFileName, ""))		// 현재 매쉬정보가 있을 때
	{ 
		ImGui::DragFloat3("Scale", m_pCurrentInfoMesh->vScale, 0.01f, 0.01f, 20.f);
		pTransform->Scaled(XMVectorSet(m_pCurrentInfoMesh->vScale[0], m_pCurrentInfoMesh->vScale[1], m_pCurrentInfoMesh->vScale[2], 0.f));

		ImGui::DragFloat3("Rotation", m_pCurrentInfoMesh->vRotation, 0.1f, -180.f, 180.f);
		pTransform->AllRotation(XMVectorSet(XMConvertToRadians(m_pCurrentInfoMesh->vRotation[0]), XMConvertToRadians(m_pCurrentInfoMesh->vRotation[1]), XMConvertToRadians(m_pCurrentInfoMesh->vRotation[2]), 0.f));

		ImGui::DragFloat3("Position", m_pCurrentInfoMesh->vPosition, 0.1f, -129.f, 257.f);
		pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_pCurrentInfoMesh->vPosition[0], m_pCurrentInfoMesh->vPosition[1], m_pCurrentInfoMesh->vPosition[2], 1.f));

		if (ImGui::Button("OnTerrain"))
			m_pSelectMesh->SetUp_OnTerrain();

		ImGui::DragFloat("Frustum_Radius", &m_pCurrentInfoMesh->fFrustumRadius, 0.1f, 0.f, 100.f);

		m_pCurrentInfoMesh->vWorldMatrix = pTransform->Get_WorldFloat4x4();
	}

	ImGui::Text("\n");
	ImGui::InputText("SaveFileName", m_szMeshSaveFileName, IM_ARRAYSIZE(m_szMeshSaveFileName));
	if (ImGui::Button("Save", ImVec2(100.f, 40.f)))
		Save_Mesh();

	ImGui::SameLine();
	bool bWireFrame = ImGui::Checkbox("WireFrame", &m_bMeshWireFrame);
	ImGui::SameLine();
	bool bBlend = ImGui::Checkbox("Blending", &m_bMeshBlend);
	if (false == m_bMeshWireFrame && false == m_bMeshBlend)
		m_pCurrentInfoMesh->iPass = 0;
	else if (false == m_bMeshWireFrame && true == m_bMeshBlend)
		m_pCurrentInfoMesh->iPass = 1;
	else if (true == m_bMeshWireFrame)
		m_pCurrentInfoMesh->iPass = 2;

	ImGui::EndChild();
}

_int CImGui_Manager::Mesh_Load_List()
{
	ImGui::Text("Load List");
	ImGui::BeginChild("LoadList", ImVec2(150.f, 300.f), true);

	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Data/Mesh/*.*", &fd);
	if (handle == -1 || handle == 0)
		return -1;

	static _int iSelect = 0;
	int iResult = 0;
	int i = 0;
	char szFilename[64];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		if (!strcmp(szFilename, ".") || !strcmp(szFilename, ".."))
		{
			iResult = _tfindnext64(handle, &fd);
			continue;
		}

		sprintf_s(szFilename, szFilename);


		if (ImGui::Selectable(szFilename, iSelect == i))
		{
			iSelect = i;
			strcpy_s(m_szMeshLoadFileName, szFilename);
		}
		i++;

		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	ImGui::EndChild();

	return iSelect;
}

CMesh * CImGui_Manager::Find_Mesh(const char* pInfoTag)
{
	auto iter = find_if(m_Meshs.begin(), m_Meshs.end(), CTagCharFinder(pInfoTag));

	if (m_Meshs.end() == iter)
		return nullptr;

	return iter->second;
}

void CImGui_Manager::Collider_Create()
{
	if (ImGui::Button("Create"))
	{
		// Create 누르면 매쉬 생성
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_AnimationModel"), TEXT("Prototype_GameObject_AnimationModel"))))
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Collider_Create");

		m_pAnimationModel = static_cast<CImGui_Animation*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_AnimationModel")));
		
		m_pAnimationModel->Set_BoneName(m_pAnimationModel->Get_Model()->Get_FrontHierarchyNodeName());
	}
}

void CImGui_Manager::Animation_List()
{
	ImGui::Text("Animation List");
	ImGui::BeginChild("AnimationList", ImVec2(500.f, 150.f), true);

	//char szFilename[64];

	const aiScene* pScene = m_pAnimationModel->Get_Model()->Get_Scene();

	for (_uint i = 0; i < pScene->mNumAnimations; i++)
	{

		if (ImGui::Selectable(pScene->mAnimations[i]->mName.data, !strcmp(m_szAnimationFileName, pScene->mAnimations[i]->mName.data)))
		{
			m_iAnimationSelect = i;
			strcpy_s(m_szAnimationFileName, pScene->mAnimations[i]->mName.data);

			m_pAnimationModel->Get_Model()->SetUp_AnimIndex(i);
		}

	}
	ImGui::EndChild();
}

void CImGui_Manager::Model_List()
{
	ImGui::Text("Model List");
	ImGui::BeginChild("ModelList", ImVec2(500.f, 150.f), true);

	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Texture/Animation/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

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

		strcat_s(szFullPath, "/");
		strcat_s(szFullPath, szFilename);
		strcat_s(szFullPath, ".fbx");

		// 선택한 모델을 픽함
		if (ImGui::Selectable(szFilename, !strcmp(m_szAnimationModelFileName, szFilename)))
		{
			strcpy_s(m_szAnimationModelFileName, szFilename);

			// 모델, 텍스쳐 갱신
			char szNameBack[MAX_PATH] = "";
			_splitpath_s(szFilename, nullptr, 0, nullptr, 0, szNameBack, MAX_PATH, nullptr, 0);
			char szNameFront[MAX_PATH] = "Prototype_Component_Model_";

			strcat_s(szNameFront, szNameBack);

			_tchar wszPrototypeName[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szNameFront, IM_ARRAYSIZE(szNameFront), wszPrototypeName, IM_ARRAYSIZE(wszPrototypeName));

			m_pAnimationModel->Set_Model(wszPrototypeName);		// 애니메이션 모델 갱신
		}
		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	ImGui::EndChild();
}

void CImGui_Manager::Collider_BoneList()
{
	ImGui::Text("Bone List");
	ImGui::BeginChild("BoneList", ImVec2(500.f, 150.f), true);


	const aiScene* pScene = m_pAnimationModel->Get_Model()->Get_Scene();

	vector<CHierarchyNode*>* pNodes = m_pAnimationModel->Get_Model()->Get_Nodes();

	for (size_t i = 0; i < pNodes->size(); i++)
	{

		if (ImGui::Selectable((*pNodes)[i]->Get_Name(), m_iBoneSelect == i))
		{
			m_iBoneSelect = (_uint)i;

			m_pAnimationModel->Set_BoneName(((*pNodes)[i]->Get_Name()));
		}

	}
	ImGui::EndChild();

	ImGui::DragFloat("Collider Radius", &m_fColliderRadius, 0.01f, 0.01f, 120.f);
	m_pAnimationModel->Set_ColliderRadius(m_fColliderRadius);
	ImGui::DragFloat3("Collider LocalPosition", m_fColliderLocalPos, 0.01f, -200.f, 200.f);
	m_pAnimationModel->Set_LocalPos(XMVectorSet(m_fColliderLocalPos[0], m_fColliderLocalPos[1], m_fColliderLocalPos[2], 1.f));
}

void CImGui_Manager::AnimModelTransform()
{
	CTransform* pTransform = static_cast<CTransform*>(m_pAnimationModel->Get_Component(TEXT("Com_Transform")));
	//m_fAnimModelScale[0] = XMVectorGetX(pTransform->Get_Scale(CTransform::STATE_RIGHT));
	//m_fAnimModelScale[1] = XMVectorGetX(pTransform->Get_Scale(CTransform::STATE_UP));
	//m_fAnimModelScale[2] = XMVectorGetX(pTransform->Get_Scale(CTransform::STATE_LOOK));

	//m_fAnimModelPosition[0] = XMVectorGetX(pTransform->Get_State(CTransform::STATE_POSITION));
	//m_fAnimModelPosition[1] = XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION));
	//m_fAnimModelPosition[2] = XMVectorGetZ(pTransform->Get_State(CTransform::STATE_POSITION));

	ImGui::DragFloat3("Scale", m_fAnimModelScale, 0.01f, 0.01f, 10.f);
	ImGui::DragFloat3("Rotation", m_fAnimModelRotation, 0.1f, -180.f, 180.f);
	ImGui::DragFloat3("Position", m_fAnimModelPosition, 0.01f, -200.f, 200.f);

	pTransform->Scaled(XMVectorSet(m_fAnimModelScale[0], m_fAnimModelScale[1], m_fAnimModelScale[2], 0.f));
	pTransform->AllRotation(XMVectorSet(XMConvertToRadians(m_fAnimModelRotation[0]), XMConvertToRadians(m_fAnimModelRotation[1]), XMConvertToRadians(m_fAnimModelRotation[2]), 0.f));
	pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fAnimModelPosition[0], m_fAnimModelPosition[1], m_fAnimModelPosition[2], 1.f));
}

void CImGui_Manager::Save_Mesh()
{
	wofstream		fout;

	char szSaveFileName[64] = "../Data/Mesh/";
	strcat_s(szSaveFileName, m_szMeshSaveFileName);
	strcat_s(szSaveFileName, ".dat");
	_tchar pStr[64] = L"";
	MultiByteToWideChar(CP_ACP, 0, szSaveFileName, IM_ARRAYSIZE(szSaveFileName)
		, pStr, IM_ARRAYSIZE(pStr));

	HANDLE		hFile = CreateFile(pStr,
		GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// 파일을 저장할 수 없으면
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSGBOX("Can't Save");
		CloseHandle(hFile);
		return;
	}
	DWORD		dwByte = 0;

	for (_uint i = 0; i < m_pMeshesInfo.size(); i++)
	{
		CMesh* pMesh = Find_Mesh(m_pMeshesInfo[i]->szFileName);
		m_pMeshesInfo[i]->vWorldMatrix = static_cast<CTransform*>(
			pMesh->Get_Component(TEXT("Com_Transform")))->Get_WorldFloat4x4();
		WriteFile(hFile, m_pMeshesInfo[i], sizeof(INFO_MESH), &dwByte, NULL);
	}
	
	if (0 == dwByte)
	{
		MSGBOX("0 == dwByte");
		CloseHandle(hFile);
		return;
	}
	MSGBOX("저장 성공!");
}

void CImGui_Manager::Load_Mesh(_int iLoadIndex)
{
	wifstream		fin;

	char szLoadFileName[64] = "../Data/Mesh/";
	strcat_s(szLoadFileName, m_szMeshLoadFileName);
	_tchar pStr[64] = L"";
	MultiByteToWideChar(CP_ACP, 0, szLoadFileName, IM_ARRAYSIZE(szLoadFileName), pStr, IM_ARRAYSIZE(pStr));


	HANDLE		hFile = CreateFile(pStr, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
	{
		MSGBOX("Can't Load in CImGui_Manager::Load_Mesh");
		return;
	}	
	_ulong					dwByte = 0;

	while (true)
	{
		INFO_MESH*		pInfoMesh = new INFO_MESH;
		ReadFile(hFile, pInfoMesh, sizeof(INFO_MESH), &dwByte, nullptr);

		if (0 == dwByte)		// 파일을 불러올 수 없는 상태
			break;

		_bool bContinue = false;
		for (auto& pMeshInfo : m_pMeshesInfo)
		{
			if (!strcmp(pInfoMesh->szFileName, pMeshInfo->szFileName))
			{
				bContinue = true;
				break;
			}
		}
		if (true == bContinue)
			continue;

		m_pMeshesInfo.push_back(pInfoMesh);
		m_pCurrentInfoMesh = pInfoMesh;
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_Mesh"), TEXT("Prototype_GameObject_Mesh"), pInfoMesh)))
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Load_Mesh");

		CMesh* pMesh = static_cast<CMesh*>(g_pGameInstance->Get_Back(LEVEL_IMGUI, TEXT("Layer_Mesh")));
		m_Meshs.emplace(pInfoMesh->szFileName, pMesh);
		m_pSelectMesh = pMesh;
		m_vMeshesFirst.push_back(pInfoMesh->szFileName);
		
		char szFileName[MAX_PATH] = "";

		_splitpath_s(pInfoMesh->szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		char szPrototype[MAX_PATH] = "Prototype_Component_Model_";
		char szTexPrototype[MAX_PATH] = "Prototype_Component_Texture_";
		strcat_s(szPrototype, szFileName);
		strcat_s(szTexPrototype, szFileName);

		_tchar wszPrototype[MAX_PATH] = L"";
		MultiByteToWideChar(CP_ACP, 0, szPrototype, sizeof(char) * MAX_PATH, wszPrototype, sizeof(_tchar) * MAX_PATH);

		_tchar wszTexPrototype[MAX_PATH] = L"";
		MultiByteToWideChar(CP_ACP, 0, szTexPrototype, sizeof(char) * MAX_PATH, wszTexPrototype, sizeof(_tchar) * MAX_PATH);

		pMesh->Set_Model(wszPrototype);
		pMesh->Set_Tex(wszTexPrototype);
	}

	CloseHandle(hFile);

}

void CImGui_Manager::UI_Names(_int iUIListIndex)
{
	ImGui::BeginChild("pane", ImVec2(300.f, 150.f), true);
	ImGui::InputText("UIFileName", m_szUIFileName, IM_ARRAYSIZE(m_szUIFileName));
	if (ImGui::Button("Create"))
	{
		GROUPUI tGroupUI;
		ZeroMemory(&tGroupUI, sizeof(GROUPUI));
		strcpy_s(tGroupUI.szUIFileName, m_szUIFileName);
		m_vGroupUI.push_back(tGroupUI);
	}
	ImGui::InputText("UIChildName", m_szUIChildName, IM_ARRAYSIZE(m_szUIChildName));
	if (ImGui::Button("CreateChild"))
	{
		if (0 < m_vGroupUI.size() && iUIListIndex >= 0)
		{
			INFO_UI* tInfoUI = new INFO_UI;

			tInfoUI->fScaleX = g_iWinCX;
			tInfoUI->fScaleY = g_iWinCY;
			tInfoUI->fPositionX = g_iWinCX * 0.5f;
			tInfoUI->fPositionY = g_iWinCY * 0.5f;
			tInfoUI->iTextureIndex = 0;
			strcpy_s(tInfoUI->szChildName, m_szUIChildName);
			m_vGroupUI[iUIListIndex].vInfo_UI.push_back(tInfoUI);
			m_pInfoUI = m_vGroupUI[iUIListIndex].vInfo_UI.back();

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), m_vGroupUI[iUIListIndex].vInfo_UI.back())))
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::UI_Names");
			RELEASE_INSTANCE(CGameInstance);
		}
	}
	ImGui::EndChild();
}

void CImGui_Manager::UI_Texture_List()
{
	if (nullptr == m_pInfoUI)
		return;

	ImGui::Text("Texture List");
	ImGui::BeginChild("left pane", ImVec2(150.f, 300.f), true);

	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Texture/UI/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;
	
	int iResult = 0;
	int i = 0;
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		if (!strcmp(szFilename, ".") || !strcmp(szFilename, ".."))
		{
			iResult = _tfindnext64(handle, &fd);
			continue;
		}

		sprintf_s(szFilename, szFilename);


		if (ImGui::Selectable(szFilename, m_pInfoUI->iTextureIndex == i))
		{
			m_pInfoUI->iTextureIndex = i;
		}
		i++;
		
		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	ImGui::EndChild();
}

_int CImGui_Manager::UI_List()
{
	auto iter = m_vGroupUI.begin();
	for (size_t i = 0; i < m_vGroupUI.size(); i++)
	{
		if (false == m_vGroupUI[i].vInfo_UI.empty() && m_vGroupUI[i].vInfo_UI.front()->bDelete == true)
		{
			for (auto& pInfoUI : (*iter).vInfo_UI)
				Safe_Delete(pInfoUI);
			(*iter).vInfo_UI.clear();
			iter = m_vGroupUI.erase(iter);
			if (m_vGroupUI.empty())
				m_pInfoUI = nullptr;
			else
				m_pInfoUI = m_vGroupUI.front().vInfo_UI.front();
		}
		else
			iter++;
	}

	static int UIselected = 0;
	{
		ImGui::BeginChild("top pane", ImVec2(250.f, 150.f), true);
		ImGui::Text("UI List");
		ImGui::BeginChild("top in pane", ImVec2(235.f, 90.f), true);

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		for (size_t i = 0; i < m_vGroupUI.size(); i++)
		{
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = (UIselected == i);
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, m_vGroupUI[i].szUIFileName);
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				UIselected = (_int)i;

			if (node_open)
			{
				for (size_t j = 0; j < m_vGroupUI[i].vInfo_UI.size(); j++)
				{
					if (nullptr != m_pInfoUI)
						if (ImGui::Selectable(m_vGroupUI[i].vInfo_UI[j]->szChildName, !strcmp(m_pInfoUI->szChildName, m_vGroupUI[i].vInfo_UI[j]->szChildName)))
							m_pInfoUI = m_vGroupUI[i].vInfo_UI[j];
				}
				ImGui::TreePop();
			}
		}
		ImGui::EndChild();

		if (ImGui::Button("Delete"))
		{
			for (size_t i = 0; i < m_vGroupUI[UIselected].vInfo_UI.size(); i++)
				m_vGroupUI[UIselected].vInfo_UI[i]->bDelete = true;
			
		}
		ImGui::EndChild();
	}


	return UIselected;
}

void CImGui_Manager::UI_Info(_int iIndex)
{
	if (nullptr == m_pInfoUI)
		return;
	ImGui::SameLine();
	ImGui::BeginChild("Right pane", ImVec2(300.f, 300.f));
	ImGui::Text(m_pInfoUI->szChildName);
	if (strcmp(m_pInfoUI->szChildName, ""))
	{
		ImGui::InputFloat("ScaleX", &m_pInfoUI->fScaleX, 0.01f, 1.0f, "%.3f");
		ImGui::InputFloat("ScaleY", &m_pInfoUI->fScaleY, 0.01f, 1.0f, "%.3f");
		ImGui::Text("");
		ImGui::InputFloat("PositionX", &m_pInfoUI->fPositionX, 0.01f, 1.0f, "%.3f");
		ImGui::InputFloat("PositionY", &m_pInfoUI->fPositionY, 0.01f, 1.0f, "%.3f");
		ImGui::Text("");
		ImGui::InputFloat("Depth", &m_pInfoUI->fDepth, 0.01f, 1.0f, "%.3f");
	}

	ImGui::Text("\n");
	ImGui::InputText("SaveFileName", m_szSaveFileName, IM_ARRAYSIZE(m_szSaveFileName));
	if (ImGui::Button("Save", ImVec2(100.f, 40.f)))
		Save_UI();

	ImGui::EndChild();

}

_int CImGui_Manager::UI_Load_List()
{
	ImGui::Text("Load List");
	ImGui::BeginChild("LoadList", ImVec2(150.f, 300.f), true);

	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Data/UI/*.*", &fd);
	if (handle == -1 || handle == 0)
		return -1;

	static _int iSelect = 0;
	int iResult = 0;
	int i = 0;
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		if (!strcmp(szFilename, ".") || !strcmp(szFilename, ".."))
		{
			iResult = _tfindnext64(handle, &fd);
			continue;
		}

		sprintf_s(szFilename, szFilename);


		if (ImGui::Selectable(szFilename, iSelect == i))
		{
			iSelect = i;
			strcpy_s(m_szLoadFileName, szFilename);
		}
		i++;

		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	ImGui::EndChild();
	return iSelect;
}

void CImGui_Manager::Save_UI()
{
	wofstream		fout;

	char szSaveFileName[64] = "../Data/UI/";
	strcat_s(szSaveFileName, m_szSaveFileName);
	strcat_s(szSaveFileName, ".txt");
	_tchar pStr[64] = L"";
	MultiByteToWideChar(CP_ACP, 0, szSaveFileName, IM_ARRAYSIZE(szSaveFileName), pStr, IM_ARRAYSIZE(pStr));

	fout.open(pStr);
	//fout.open(L"../Data/ImgPath.txt");

	// 파일 개방 성공
	if (!fout.fail())
	{
		for (size_t i = 0; i < m_vGroupUI.size(); i++)
		{
			wstring wstr1(m_vGroupUI[i].szUIFileName, &m_vGroupUI[i].szUIFileName[strlen(m_vGroupUI[i].szUIFileName)]);
			for (size_t j = 0; j < m_vGroupUI[i].vInfo_UI.size(); j++)
			{
				wstring wstr2(m_vGroupUI[i].vInfo_UI[j]->szChildName, &m_vGroupUI[i].vInfo_UI[j]->szChildName[strlen(m_vGroupUI[i].vInfo_UI[j]->szChildName)]);

				fout << wstr1 << L"|" << wstr2 << L"|"
					<< m_vGroupUI[i].vInfo_UI[j]->fScaleX << L"|" << m_vGroupUI[i].vInfo_UI[j]->fScaleY << L"|"
					<< m_vGroupUI[i].vInfo_UI[j]->fPositionX << L"|" << m_vGroupUI[i].vInfo_UI[j]->fPositionY << L"|"
					<< m_vGroupUI[i].vInfo_UI[j]->fDepth << L"|" << m_vGroupUI[i].vInfo_UI[j]->iTextureIndex << endl;
			}
		}
		

		fout.close();		// close 함수는 생략 가능(객체 타입이어서 소멸 시점에 알아서 개방한 파일 또한 소멸 가능)
	}

	// 윈도우의 기본 프로그램을 실행시켜주는 함수
	char szSaveFileNameFinal[64] = "notepad.exe ";
	strcat_s(szSaveFileNameFinal, szSaveFileName);
	WinExec(szSaveFileNameFinal, SW_SHOW);
	//WinExec("notepad.exe ../Data/ImgPath.txt", SW_SHOW);
}

void CImGui_Manager::Load_UI(_int iLoadIndex)
{
	wifstream		fin;

	char szLoadFileName[64] = "../Data/UI/";
	strcat_s(szLoadFileName, m_szLoadFileName);
	_tchar pStr[64] = L"";
	MultiByteToWideChar(CP_ACP, 0, szLoadFileName, IM_ARRAYSIZE(szLoadFileName), pStr, IM_ARRAYSIZE(pStr));

	fin.open(pStr);

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

			GROUPUI tGroupUI;
			WideCharToMultiByte(CP_ACP, 0, szUIFileName, MAX_PATH, tGroupUI.szUIFileName, MAX_PATH, NULL, NULL);

			INFO_UI* pInfoUI = new INFO_UI;
			WideCharToMultiByte(CP_ACP, 0, szChildName, MAX_PATH, pInfoUI->szChildName, MAX_PATH, NULL, NULL);
			pInfoUI->fScaleX = (_float)_tstof(szScaleX);
			pInfoUI->fScaleY = (_float)_tstof(szScaleY);
			pInfoUI->fPositionX = (_float)_tstof(szPosX);
			pInfoUI->fPositionY = (_float)_tstof(szPosY);
			pInfoUI->fDepth = (_float)_tstof(szDepth);
			pInfoUI->iTextureIndex = (_int)_tstoi(szTexIndex);
			pInfoUI->bDelete = false;

			tGroupUI.vInfo_UI.push_back(pInfoUI);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (true == m_vGroupUI.empty() || strcmp(m_vGroupUI.back().szUIFileName, tGroupUI.szUIFileName))
				m_vGroupUI.push_back(tGroupUI);
			else
				m_vGroupUI.back().vInfo_UI.push_back(pInfoUI);

			if (FAILED(pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), m_vGroupUI.back().vInfo_UI.back())))
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Load_UI");
			RELEASE_INSTANCE(CGameInstance);
		}
		m_pInfoUI = m_vGroupUI[0].vInfo_UI[0];
		fin.close();
	}
	char szLoadFileNameFinal[64] = "notepad.exe ";
	strcat_s(szLoadFileNameFinal, szLoadFileName);
	WinExec(szLoadFileNameFinal, SW_SHOW);

}

void CImGui_Manager::Particle_Info()
{
	ImGui::InputInt("TexIndex", &m_tParticleDesc.iTexIndex);
	if (MAX_PATICLETEXTURE - 1 < m_tParticleDesc.iTexIndex)
		m_tParticleDesc.iTexIndex = MAX_PATICLETEXTURE - 1;
	else if (0 > m_tParticleDesc.iTexIndex)
		m_tParticleDesc.iTexIndex = 0;

	ImGui::DragFloat("MinSpeed", &m_tParticleDesc.fMinSpeed, 0.01f, -500.f, 500.f);
	ImGui::DragFloat("MaxSpeed", &m_tParticleDesc.fMaxSpeed, 0.01f, -500.f, 500.f);
	ImGui::DragFloat("OriginScale", &m_tParticleDesc.fOriginScale, 0.01f, -50.f, 50.f);
	ImGui::DragFloat("ScaleSpeed", &m_tParticleDesc.fScaleSpeed, 0.01f, -500.f, 500.f);
	ImGui::DragFloat("GravityPower", &m_tParticleDesc.fGravityPower, 0.01f, -100.f, 100.f);
	ImGui::DragFloat("LiveTime", &m_tParticleDesc.fLiveTime, 0.01f, -3000.f, 3000.f);
	ImGui::DragFloat("MakeRange", &m_tParticleDesc.fMakeRange, 0.01f, -100.f, 100.f);
	ImGui::DragFloat("MaxDistance", &m_tParticleDesc.fMaxDistance, 0.01f, -100.f, 100.f);

	if (ImGui::TreeNode("DIR"))
	{
		static ImGuiComboFlags flags = 0;

		const char* items[] = { "DIR_ONE", "DIR_RANDOM" };
		static int item_current_idx = 0; 
		const char* combo_preview_value = items[item_current_idx];
		if (ImGui::BeginCombo("DIR", combo_preview_value, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current_idx = n;
					m_tParticleDesc.eDir = (CParticle_Effect::DIR)item_current_idx;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::TreePop();
	}

	if (CParticle_Effect::DIR_ONE == m_tParticleDesc.eDir)
	{
		static _float	fDir[3] = {};
		ImGui::DragFloat3("Dir", fDir, 0.01f, -500.f, 500.f);
		m_tParticleDesc.vDir = XMVector4Normalize(XMVectorSet(fDir[0], fDir[1], fDir[2], 0.f));
	}

	static _float	fPosition[3] = {};
	ImGui::DragFloat3("Position", fPosition, 0.01f, -500.f, 500.f);
	m_vParticlePosition = _float4(fPosition[0], fPosition[1], fPosition[2], 1.f);
	m_tParticleDesc.pParentPosition = &m_vParticlePosition;

	if (ImGui::TreeNode("ParticleNumber"))
	{
		static ImGuiComboFlags flags = 0;

		const char* items[] = { "10", "30", "50", "100" };
		static int item_current_idx = 0;
		const char* combo_preview_value = items[item_current_idx];
		if (ImGui::BeginCombo("ParticleNumber", combo_preview_value, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current_idx = n;
					switch (item_current_idx)
					{
					case 0:
						m_tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
						break;
					case 1:
						m_tParticleDesc.eParticleNumber = CParticle_Effect::NUM_30;
						break;
					case 2:
						m_tParticleDesc.eParticleNumber = CParticle_Effect::NUM_50;
						break;
					case 3:
						m_tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
						break;
					}
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::TreePop();
	}

	ImGui::Checkbox("UseEasing", &m_tParticleDesc.bUseEasing);
	if (true == m_tParticleDesc.bUseEasing)
	{
		if (ImGui::TreeNode("Easing"))
		{
			static ImGuiComboFlags Easingflags = 0;

			const char* Easingitems[] = {
					"INSINE",		"OUTSINE",		"INOUTSINE",	"INQUAD",	"OUTQUAD",	"INOUTQUAD",
					"INCUBIC",		"OUTCUBIC",		"INOUTCUBIC",	"INQUART",	"OUTQUART",	"INOUTQUART",
					"INQUINT",		"OUTQUINT",		"INOUTQUINT",	"INEXPO",	"OUTEXPO",	"INOUTEXPO",
					"INCIRC",		"OUTCIRC",		"INOUTCIRC",	"INBACK",	"OUTBACK",	"INOUTBACK",
					"INELASTIC",	"OUTELASTIC",	"INOUTELASTIC", "INBOUNCE", "OUTBOUNCE", "INOUTBOUNCE"
			};
			static int Easing_current_idx = 0;
			const char* Easing_combo_preview_value = Easingitems[Easing_current_idx];
			if (ImGui::BeginCombo("Easing", Easing_combo_preview_value, Easingflags))
			{
				for (int n = 0; n < IM_ARRAYSIZE(Easingitems); n++)
				{
					const bool is_selected = (Easing_current_idx == n);
					if (ImGui::Selectable(Easingitems[n], is_selected))
					{
						Easing_current_idx = n;
						m_tParticleDesc.eEasing = (CParticle_Effect::EASINGFUNCTION)Easing_current_idx;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::TreePop();
		}
	}
	ImGui::Checkbox("Chase", &m_tParticleDesc.bChase);
	ImGui::Checkbox("Roof", &m_tParticleDesc.bRoof);
	ImGui::Checkbox("Trail", &m_tParticleDesc.bTrail);
	if (true == m_tParticleDesc.bTrail)
		ImGui::DragFloat("Trail_Interval", &m_tParticleDesc.fTrailInterval, 0.01f, 0.f, 10.f);


	if (ImGui::Button("CreateParticle"))
	{
		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &m_tParticleDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Particle_Info");
			return;
		}
	}
}

void CImGui_Manager::NaviMesh_Make()
{
	ImGui::Checkbox("MakeCell_Mode", &m_bMakeCell_Mode);
	if (true == m_bMakeCell_Mode)
	{
		m_bTypeSetting_Mode = false;
		m_bTypeChange_Mode = false;
		// 테스트 포인트 생성
		if (false == m_bMakeTestPoint)
		{
			for (_uint i = 0; i < 3; i++)
			{
				if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_TestPoint"), TEXT("Prototype_GameObject_CellPoint"), &m_Points[i])))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::NaviMesh_Names");
					return;
				}
			}
			m_bMakeTestPoint = true;
			m_bFirstSetting = false;
		}


		// 우클릭으로 포인트 인덱스 전환가능
		if (true == g_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_RBUTTON))
		{
			m_iPointIndex++;
			if (2 < m_iPointIndex)
				m_iPointIndex = 0;
		}
		// 포인트 인덱스 보여줌
		ImGui::Text("PointIndex : %d", m_iPointIndex);
		
		// 스페이스 + 좌클릭으로 터레인이나 마우스 피킹으로 포인트의 위치 특정해줌
		if ((g_pGameInstance->Get_DIKeyState(DIK_R) & 0x80) && true == g_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
		{
			_float3 vMousePos = _float3(0.f, 0.f, 0.f);
			if (true == m_pSelectTerrain->Pick_Terrain(&vMousePos))
			{
				m_Points[m_iPointIndex][0] = vMousePos.x;
				m_Points[m_iPointIndex][1] = vMousePos.y;
				m_Points[m_iPointIndex][2] = vMousePos.z;
			}

			CPicking* pPicking = GET_INSTANCE(CPicking);

			_vector Dir = pPicking->Get_RayDir();
			_vector Pos = pPicking->Get_RayPos();

			list<CCollider*>* ColliderList = g_pGameInstance->Get_ColliderList(TEXT("CellPoint"), CCollision_Manager::TARGET_OBJECT);
			if (nullptr != ColliderList)
				for (auto& pCollider : *ColliderList)
				{
					_float Dist = 0.f;
					if (true == g_pGameInstance->Collision_RayToSphere(Pos, Dir, pCollider, &Dist))
					{
						m_Points[m_iPointIndex][0] = pCollider->Get_Sphere()->Center.x;
						m_Points[m_iPointIndex][1] = pCollider->Get_Sphere()->Center.y;
						m_Points[m_iPointIndex][2] = pCollider->Get_Sphere()->Center.z;
					}
				}

			RELEASE_INSTANCE(CPicking);

			m_iPointIndex++;
			if (2 < m_iPointIndex)
				m_iPointIndex = 0;
		}

		ImGui::DragFloat3("Points1", m_Points[0], 0.01f, 0.f, 200.f);
		ImGui::DragFloat3("Points2", m_Points[1], 0.01f, 0.f, 200.f);
		ImGui::DragFloat3("Points3", m_Points[2], 0.01f, 0.f, 200.f);

		if (ImGui::Button("CreateCell"))
		{
			_float3	Points[3] = {
				_float3(m_Points[0][0], m_Points[0][1], m_Points[0][2]),
				_float3(m_Points[1][0], m_Points[1][1], m_Points[1][2]),
				_float3(m_Points[2][0], m_Points[2][1], m_Points[2][2]),
			};
			if (Points[0].x > Points[1].x)
			{
				_float3 Temp = Points[0];
				Points[0] = Points[1];
				Points[1] = Temp;
			}
			if (Points[0].x > Points[2].x)
			{
				_float3 Temp = Points[0];
				Points[0] = Points[2];
				Points[2] = Temp;
			}

			_float B = -((Points[1].z - Points[0].z) / (Points[1].x - Points[0].x) * Points[0].x - Points[0].z);
			_float Z = (Points[1].z - Points[0].z) / (Points[1].x - Points[0].x) * Points[2].x + B;
			if (Z < Points[2].z)
			{
				_float3 Temp = Points[1];
				Points[1] = Points[2];
				Points[2] = Temp;
			}


			CCell*	pCell = CCell::Create(m_pDevice, m_pDeviceContext, Points, (_uint)m_Cells.size());
			m_Cells.push_back(pCell);

			if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_A))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::NaviMesh_Names");
				return;
			}
			pCell->Set_CellPoint(CCell::POINT_A, g_pGameInstance->Get_Back(LEVEL_IMGUI, TEXT("Layer_Point")));

			if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_B))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::NaviMesh_Names");
				return;
			}
			pCell->Set_CellPoint(CCell::POINT_B, g_pGameInstance->Get_Back(LEVEL_IMGUI, TEXT("Layer_Point")));

			if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_C))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::NaviMesh_Names");
				return;
			}
			pCell->Set_CellPoint(CCell::POINT_C, g_pGameInstance->Get_Back(LEVEL_IMGUI, TEXT("Layer_Point")));

			ZeroMemory(m_Points, sizeof(_float) * 9);

			m_iCurrentCellIndex = (_uint)m_Cells.size() - 1;
		}
	}

	ImGui::Checkbox("Fix_Mode", &m_bTypeSetting_Mode);
	if (true == m_bTypeSetting_Mode)
	{
		m_bTypeChange_Mode = false;
		if (true == m_bMakeCell_Mode)
		{
			if (FAILED(g_pGameInstance->Release_LayerObject(LEVEL_IMGUI, TEXT("Layer_TestPoint"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CImGui_Manager::NaviMesh_Names");
				return;
			}
			m_bFirstSetting = true;
			m_bMakeCell_Mode = false;
			m_bMakeTestPoint = false;
		}

		if (0 <= m_iCurrentCellIndex)
		{
			_float3* vPointA = m_Cells[m_iCurrentCellIndex]->Get_Points(CCell::POINT_A);
			_float3* vPointB = m_Cells[m_iCurrentCellIndex]->Get_Points(CCell::POINT_B);
			_float3* vPointC = m_Cells[m_iCurrentCellIndex]->Get_Points(CCell::POINT_C);
			_float	fPoint_A[3] = { vPointA->x, vPointA->y, vPointA->z };
			_float	fPoint_B[3] = { vPointB->x, vPointB->y, vPointB->z };
			_float	fPoint_C[3] = { vPointC->x, vPointC->y, vPointC->z };

			ImGui::DragFloat3("Point_A", fPoint_A, 0.01f, -50.f, 200.f);
			ImGui::SameLine();
			ImGui::Checkbox("A", &m_bPointA);
			if (true == m_bPointA)
			{
				m_bPointB = false;
				m_bPointC = false;
			}

			ImGui::DragFloat3("Point_B", fPoint_B, 0.01f, -50.f, 200.f);
			ImGui::SameLine();
			ImGui::Checkbox("B", &m_bPointB);
			if (true == m_bPointB)
			{
				m_bPointA = false;
				m_bPointC = false;
			}

			ImGui::DragFloat3("Point_C", fPoint_C, 0.01f, -50.f, 200.f);
			ImGui::SameLine();
			ImGui::Checkbox("C", &m_bPointC);
			if (true == m_bPointC)
			{
				m_bPointB = false;
				m_bPointA = false;
			}

			// 스페이스 + 좌클릭으로 터레인이나 마우스 피킹으로 포인트의 위치 특정해줌
			if ((g_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80) && true == g_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
			{
				_float3 vMousePos = _float3(0.f, 0.f, 0.f);

				CPicking* pPicking = GET_INSTANCE(CPicking);

				_vector Dir = pPicking->Get_RayDir();
				_vector Pos = pPicking->Get_RayPos();

				list<CCollider*>* ColliderList = g_pGameInstance->Get_ColliderList(TEXT("CellPoint"), CCollision_Manager::TARGET_OBJECT);
				if (nullptr != ColliderList)
					for (auto& pCollider : *ColliderList)
					{
						_float Dist = 0.f;
						if (true == g_pGameInstance->Collision_RayToSphere(Pos, Dir, pCollider, &Dist))
						{
							if (true == m_bPointA)
							{
								fPoint_A[0] = pCollider->Get_Sphere()->Center.x;
								fPoint_A[1] = pCollider->Get_Sphere()->Center.y;
								fPoint_A[2] = pCollider->Get_Sphere()->Center.z;
							}
							else if (true == m_bPointB)
							{
								fPoint_B[0] = pCollider->Get_Sphere()->Center.x;
								fPoint_B[1] = pCollider->Get_Sphere()->Center.y;
								fPoint_B[2] = pCollider->Get_Sphere()->Center.z;
							}
							else if (true == m_bPointC)
							{
								fPoint_C[0] = pCollider->Get_Sphere()->Center.x;
								fPoint_C[1] = pCollider->Get_Sphere()->Center.y;
								fPoint_C[2] = pCollider->Get_Sphere()->Center.z;
							}
						}
					}

				RELEASE_INSTANCE(CPicking);
			}

			vPointA->x = fPoint_A[0];
			vPointA->y = fPoint_A[1];
			vPointA->z = fPoint_A[2];
			vPointB->x = fPoint_B[0];
			vPointB->y = fPoint_B[1];
			vPointB->z = fPoint_B[2];
			vPointC->x = fPoint_C[0];
			vPointC->y = fPoint_C[1];
			vPointC->z = fPoint_C[2];
		}

	}

	ImGui::Checkbox("TypeChange_Mode", &m_bTypeChange_Mode);
	if (true == m_bTypeChange_Mode)
	{
		if (true == m_bMakeCell_Mode)
		{
			if (FAILED(g_pGameInstance->Release_LayerObject(LEVEL_IMGUI, TEXT("Layer_TestPoint"))))
			{
				MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CImGui_Manager::NaviMesh_Names");
				return;
			}
			m_bFirstSetting = true;
			m_bMakeCell_Mode = false;
			m_bMakeTestPoint = false;
		}
		m_bTypeSetting_Mode = false;

		CPicking* pPicking = GET_INSTANCE(CPicking);
		if (g_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80 && g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
		{
			for (auto& pCell : m_Cells)
			{
				_vector CellPoint[3] = { pCell->Get_Point(CCell::POINT_A), pCell->Get_Point(CCell::POINT_B), pCell->Get_Point(CCell::POINT_C) };

  				if (true == pPicking->IsNaviPick(CellPoint))
				{
  					pCell->Set_Type(CCell::TYPE_ROAD);
					break;
				}
			}
		}
		else if (g_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80 && g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON))
		{
			for (auto& pCell : m_Cells)
			{
				_vector CellPoint[3] = { pCell->Get_Point(CCell::POINT_A), pCell->Get_Point(CCell::POINT_B), pCell->Get_Point(CCell::POINT_C) };

				if (true == pPicking->IsNaviPick(CellPoint))
				{
					pCell->Set_Type(CCell::TYPE_NONE);
					break;
				}
			}
		}
		RELEASE_INSTANCE(CPicking);
	}

	ImGui::InputText("SaveFileName", m_szSaveNaviDataFileName, MAX_PATH);
	if (ImGui::Button("Save_Navi"))
	{
		Save_NaviMesh();
	}
}

_int CImGui_Manager::NaviMesh_List()
{
	if (0 > m_iCurrentCellIndex)
		return 0;

	ImGui::BeginChild("top pane", ImVec2(250.f, 150.f), true);
	ImGui::Text("Cell List");
	ImGui::BeginChild("top in pane", ImVec2(235.f, 90.f), true);

	for (_uint i = 0; i < m_Cells.size(); i++)
	{
		char label[128];
		sprintf_s(label, "Cell%d", i);

		if (ImGui::Selectable(label, i == m_iCurrentCellIndex))
		{
			for (_uint i = 0; i < 3; i++)
				static_cast<CCellPoint*>(m_Cells[m_iCurrentCellIndex]->Get_CellPoints()[i])->IsPickCell(false);

			m_iCurrentCellIndex = i;
			for (_uint i = 0; i < 3; i++)
				static_cast<CCellPoint*>(m_Cells[m_iCurrentCellIndex]->Get_CellPoints()[i])->IsPickCell(true);
			break;
		}


	}
	ImGui::EndChild();

	if (ImGui::Button("Cell_Delete") && 0 < m_Cells.size())
	{
		auto iter = m_Cells.begin();

		for (_int i = 0; i < m_iCurrentCellIndex; i++)
			iter++;

		m_Cells[m_iCurrentCellIndex]->CellPointsDead();
		Safe_Release(m_Cells[m_iCurrentCellIndex]);
		m_Cells.erase(iter);
		m_iCurrentCellIndex = (_uint)m_Cells.size() - 1;
	}


	ImGui::EndChild();

	if (ImGui::Button("NaviRender"))
	{
		m_bNaviMeshRender = !m_bNaviMeshRender;
		g_bRender = !g_bRender;
	}

	return _int();
}

_int CImGui_Manager::NaviMesh_Load_List()
{
	ImGui::Text("Load List");
	ImGui::BeginChild("LoadList", ImVec2(150.f, 300.f), true);

	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Data/NaviMesh/*.*", &fd);
	if (handle == -1 || handle == 0)
		return -1;

	static _int iSelect = 0;
	int iResult = 0;
	int i = 0;
	char szFilename[64];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		if (!strcmp(szFilename, ".") || !strcmp(szFilename, ".."))
		{
			iResult = _tfindnext64(handle, &fd);
			continue;
		}

		sprintf_s(szFilename, szFilename);


		if (ImGui::Selectable(szFilename, iSelect == i))
		{
			iSelect = i;
			strcpy_s(m_szNaviDataFilePath, "../Data/NaviMesh/");
			strcat_s(m_szNaviDataFilePath, szFilename);
		}
		i++;

		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	ImGui::EndChild();

	return iSelect;
}

void CImGui_Manager::Save_NaviMesh()
{
	char szPath[MAX_PATH] = "../Data/NaviMesh/";
	strcat_s(szPath, m_szSaveNaviDataFileName);
	strcat_s(szPath, ".dat");
	_tchar			wszStr[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, szPath, IM_ARRAYSIZE(szPath), wszStr, IM_ARRAYSIZE(wszStr));

	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(wszStr, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return;

	_float3			vPoints[3];
	_uint			iType = 0;
	for (auto& pCell : m_Cells)
	{
		memcpy(vPoints, pCell->Get_Points(CCell::POINT_A), sizeof(_float3) * 3);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		iType = pCell->Get_Type();
		WriteFile(hFile, &iType, sizeof(_uint), &dwByte, nullptr);
	}

	if (0 == dwByte)
	{
		MSGBOX("Failed Save");
		return;
	}

	MSGBOX("Success Save");

	CloseHandle(hFile);
}

void CImGui_Manager::Load_NaviMesh()
{
	_ulong			dwByte = 0;
	_tchar			wszStr[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, m_szNaviDataFilePath, IM_ARRAYSIZE(m_szNaviDataFilePath), wszStr, IM_ARRAYSIZE(wszStr));
	HANDLE			hFile = CreateFile(wszStr, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return;

	_float3			vPoints[3];
	_uint			iType = 0;
	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		ReadFile(hFile, &iType, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pDeviceContext, vPoints, (_uint)m_Cells.size());
		if (nullptr == pCell)
			return;
		pCell->Set_Type((CCell::TYPE)iType);

		m_Cells.push_back(pCell);

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_A))))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Load_NaviMesh");
			return;
		}
		pCell->Set_CellPoint(CCell::POINT_A, g_pGameInstance->Get_Back(LEVEL_IMGUI, TEXT("Layer_Point")));

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_B))))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Load_NaviMesh");
			return;
		}
		pCell->Set_CellPoint(CCell::POINT_B, g_pGameInstance->Get_Back(LEVEL_IMGUI, TEXT("Layer_Point")));

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_IMGUI, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_C))))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Load_NaviMesh");
			return;
		}
		pCell->Set_CellPoint(CCell::POINT_C, g_pGameInstance->Get_Back(LEVEL_IMGUI, TEXT("Layer_Point")));

		m_iCurrentCellIndex = (_uint)m_Cells.size() - 1;
	}

	CloseHandle(hFile);
}

void CImGui_Manager::Free()
{
	for (size_t i = 0; i < m_vGroupUI.size(); i++)
	{
		for (size_t j = 0; j < m_vGroupUI[i].vInfo_UI.size(); j++)
			Safe_Delete(m_vGroupUI[i].vInfo_UI[j]);
		m_vGroupUI[i].vInfo_UI.clear();
	}
	m_vGroupUI.clear();
	for (size_t i = 0; i < m_vGroupTerrain.size(); i++)
	{
		for (size_t j = 0; j < m_vGroupTerrain[i].vInfo_Terrain.size(); j++)
			Safe_Delete(m_vGroupTerrain[i].vInfo_Terrain[j]);
		m_vGroupTerrain[i].vInfo_Terrain.clear();
	}
	m_vGroupTerrain.clear();

	for (size_t i = 0; i < m_vFirst.size(); i++)
	{
		Safe_Delete_Array(m_vFirst[i]);
	}
	m_vFirst.clear();
	for (size_t i = 0; i < m_vLFirst.size(); i++)
	{
		Safe_Delete_Array(m_vLFirst[i]);
	}
	m_vLFirst.clear();
	for (size_t i = 0; i < m_vMeshesFirst.size(); i++)
	{
		Safe_Delete_Array(m_vMeshesFirst[i]);
	}
	m_vMeshesFirst.clear();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
#endif

