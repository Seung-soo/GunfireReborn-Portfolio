#include "stdafx.h"
#include "..\public\Load_Manager.h"
#include "Mesh.h"
#include "Model.h"

IMPLEMENT_SINGLETON(CLoad_Manager)

CLoad_Manager::CLoad_Manager()
{

}

HRESULT CLoad_Manager::Load_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelIndex, const _tchar * pLoadFullPath)
{
	wifstream		fin;

	HANDLE		hFile = CreateFile(pLoadFullPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
	{
		MSGBOX("Can't Load in CLoad_Manager::Load_Mesh");
		return E_FAIL;
	}
	_ulong					dwByte = 0;

	while (true)
	{
		INFO_MESH*		pInfoMesh = new INFO_MESH;
		ReadFile(hFile, pInfoMesh, sizeof(INFO_MESH), &dwByte, nullptr);

		if (0 == dwByte)		// 파일을 불러올 수 없는 상태
		{
			Safe_Delete(pInfoMesh);
			break;
		}

		m_vInfo_Meshes[iLevelIndex].push_back(pInfoMesh);

		char szFilePath[MAX_PATH] = "../Bin/Texture/Mesh/";
		char szTextureFilePath[MAX_PATH] = "../Bin/Texture/Mesh/";
		char szFileName[MAX_PATH] = "";
		char szFileExt[MAX_PATH] = "";
		_splitpath_s(pInfoMesh->szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szFileExt, MAX_PATH);
		strcat_s(szFilePath, szFileName);
		strcat_s(szFilePath, "/");

		char szPrototype[MAX_PATH] = "Prototype_Component_Model_";
		char szTexPrototype[MAX_PATH] = "Prototype_Component_Texture_";

		strcat_s(szPrototype, szFileName);
		strcat_s(szTexPrototype, szFileName);
		strcat_s(szTextureFilePath, szFileName);
		strcat_s(szTextureFilePath, "/");
		strcat_s(szTextureFilePath, szFileName);
		strcat_s(szTextureFilePath, ".dds");

		strcat_s(szFileName, szFileExt);
		_tchar* wszPrototype = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, szPrototype, sizeof(char) * MAX_PATH, wszPrototype, sizeof(_tchar) * MAX_PATH);
		m_vPrototypeKey.push_back(wszPrototype);

		_matrix		TransformMatrix;

		TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationX(XMConvertToRadians(90.f));

		if (nullptr == g_pGameInstance->Find_Component(iLevelIndex, wszPrototype))
		{
			if (FAILED(g_pGameInstance->Add_Prototype(iLevelIndex, wszPrototype, CModel::Create(pDevice, pDeviceContext, CModel::TYPE_NONANIM, szFilePath, szFileName, TransformMatrix))))
			{
				MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh");
				return E_FAIL;
			}
		}

		_tchar* wszTexPrototype = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, szTexPrototype, sizeof(char) * MAX_PATH, wszTexPrototype, sizeof(_tchar) * MAX_PATH);
		m_vPrototypeKey.push_back(wszTexPrototype);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLoad_Manager::Make_GameObject(_uint iLevelIndex)
{
	for (size_t i = 0; i < m_vInfo_Meshes[iLevelIndex].size(); i++)
	{
		if (FAILED(g_pGameInstance->Add_GameObject(iLevelIndex, TEXT("Layer_Mesh"), TEXT("Prototype_GameObject_Mesh"), m_vInfo_Meshes[iLevelIndex][i])))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoad_Manager::Load_Mesh");
			return E_FAIL;
		}

		char szFileName[MAX_PATH] = "";
		_splitpath_s(m_vInfo_Meshes[iLevelIndex][i]->szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		char szPrototype[MAX_PATH] = "Prototype_Component_Model_";
		strcat_s(szPrototype, szFileName);

		_tchar* wszPrototype = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, szPrototype, sizeof(char) * MAX_PATH, wszPrototype, sizeof(_tchar) * MAX_PATH);
		m_vPrototypeKey.push_back(wszPrototype);

		CMesh* pMesh = static_cast<CMesh*>(g_pGameInstance->Get_Back(iLevelIndex, TEXT("Layer_Mesh")));

		pMesh->Set_Model(wszPrototype);

		string str(m_vInfo_Meshes[iLevelIndex][i]->szFileName);
		if (LEVEL_LOBY <= iLevelIndex && LEVEL_STAGE3_2 >= iLevelIndex)
		{
			if (string::npos != str.find("woodbox"))
				pMesh->Set_Tex(TEXT("Prototype_Component_Texture_WoodBox"));
			else if (string::npos != str.find("Bamboo") || string::npos != str.find("bamboo"))
			{
				if (string::npos != str.find("Tree"))
					pMesh->Set_Tex(TEXT("Prototype_Component_Texture_bambooTree"));
				else
					pMesh->Set_Tex(TEXT("Prototype_Component_Texture_bambooLeaf"));
			}
			else if (string::npos != str.find("Grass"))
			{
				pMesh->Set_Tex(TEXT("Prototype_Component_Texture_grass"));
			}
			else if (string::npos != str.find("windmilltop"))
			{
				pMesh->Set_Tex(TEXT("Prototype_Component_Texture_WindmillTop"));
			}
			else 
				pMesh->Set_Tex(TEXT("Prototype_Component_Texture_Stage3"));
		}
		else if (LEVEL_STAGE4_1 <= iLevelIndex && LEVEL_STAGE4_2 >= iLevelIndex)
		{
			if (string::npos != str.find("Boss_"))
				pMesh->Set_Tex(TEXT("Prototype_Component_Texture_BossSnow"));
			else if (string::npos != str.find("Boss"))
				pMesh->Set_Tex(TEXT("Prototype_Component_Texture_Boss"));
			else
				pMesh->Set_Tex(TEXT("Prototype_Component_Texture_Stage4"));
		}


		
	}


	return S_OK;
}

void CLoad_Manager::Free()
{
	for (auto& pPrototypeKey : m_vPrototypeKey)
		Safe_Delete_Array(pPrototypeKey);

	m_vPrototypeKey.clear();

	for (_uint i = 0; i < LEVEL_END; ++i)
	{
		for (auto& pInfoMesh : m_vInfo_Meshes[i])
			Safe_Delete(pInfoMesh);
		m_vInfo_Meshes[i].clear();
	}

}
