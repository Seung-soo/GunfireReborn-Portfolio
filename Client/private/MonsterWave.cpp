#include "stdafx.h"
#include "..\public\MonsterWave.h"
#include "StageWall.h"
#include "Monster.h"
#include "Cell.h"
#include "Kappa.h"
#include "CatfishWarrior.h"
#include "Yacha.h"
#include "Lobstar.h"
#include "Monk.h"
#include "UFO.h"
#include "Fox.h"
#include "Monkey.h"
#include "Bear.h"
#include "Ghost.h"
#include "NextAria.h"

IMPLEMENT_SINGLETON(CMonsterWave)

CMonsterWave::CMonsterWave()
{
}

list<CMonster*>* CMonsterWave::Get_MonstersList()
{
	return &m_Monsters;
}

void CMonsterWave::Update()
{
#ifdef _DEBUG
	if (g_pGameInstance->Get_KeyEnter(DIK_G))
	{
		for (auto& pMonster : m_Monsters)
			pMonster->Set_Dead(true);
	}
#endif

	for (auto iter = m_Monsters.begin(); iter != m_Monsters.end() ;)
	{
		if (true == (*iter)->Get_Dead())
			iter = m_Monsters.erase(iter);
		else
			iter++;
	}

	if (true == m_Monsters.empty() || 0 == m_Monsters.size())
	{
		if (true == m_bStage3_1_1)
		{
			ReleaseWall();
			m_bStage3_1_1 = false;

			// 입구 렉트 생성
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_NextAria"), TEXT("Prototype_GameObject_NextAria"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonsterWave::Update");
				return;
			}
			static_cast<CNextAria*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_NextAria")))->WallSetting(51, CNextAria::LINE_BC, STAGE3_1_2);
			// 렉트 안에서 충돌하면 스테이지 3_1_2 함수 실행하게 한다.
		}
		else if (true == m_bStage3_1_2)
		{
			ReleaseWall();
			m_bStage3_1_2 = false;

			// 입구 렉트 생성
			// 렉트 안에서 충돌하면 스테이지 3_1_2 함수 실행하게 한다.

			// 입구 렉트 생성
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_NextAria"), TEXT("Prototype_GameObject_NextAria"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonsterWave::Update");
				return;
			}
			static_cast<CNextAria*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_NextAria")))->WallSetting(92, CNextAria::LINE_AB, STAGE3_1_3);
		}
		else if (true == m_bStage3_1_3)
		{
			ReleaseWall();
			m_bStage3_1_3 = false;

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Portal"), TEXT("Prototype_GameObject_StageWall"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonsterWave::Update");
				return;
			}
			static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Portal")))->WallSetting(111, CStageWall::LINE_AB, true);
		}
		else if (true == m_bStage4_1_1)
		{
			ReleaseWall();
			m_bStage4_1_1 = false;

			// 입구 렉트 생성
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_NextAria"), TEXT("Prototype_GameObject_NextAria"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonsterWave::Update");
				return;
			}
			static_cast<CNextAria*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_NextAria")))->WallSetting(37, CNextAria::LINE_AB, STAGE4_1_2);
		}
		else if (true == m_bStage4_1_2)
		{
			ReleaseWall();
			m_bStage4_1_2 = false;

			// 입구 렉트 생성
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_NextAria"), TEXT("Prototype_GameObject_NextAria"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonsterWave::Update");
				return;
			}
			static_cast<CNextAria*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_NextAria")))->WallSetting(63, CNextAria::LINE_AB, STAGE4_1_3);
		}
		else if (true == m_bStage4_1_3)
		{
			ReleaseWall();
			m_bStage4_1_3 = false;

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Portal"), TEXT("Prototype_GameObject_StageWall"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMonsterWave::Update");
				return;
			}
			static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Portal")))->WallSetting(93, CStageWall::LINE_CA, true);
		}
	}	
}

void CMonsterWave::WaveAppear(STAGE eStage)
{
	g_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT);
	g_pGameInstance->LetsPlaySound(TEXT("AppearMonster.mp3"), CSoundMgr::SYSTEM_EFFECT, 0.7f);

	switch (eStage)
	{
	case STAGE3_1_1:	// 처음에 맵 들어갈 때 호출
		m_bStage3_1_1 = true;
		Wave_Stage3_1_1();
		break;
	case STAGE3_1_2:	// 입구를 좀 뒤에 생성, 입구쪽의 셀들에 들어가면 소환하게
		m_bStage3_1_2 = true;
		Wave_Stage3_1_2();
		break;
	case STAGE3_1_3:	// 입구쪽 셀에 들어가면 소환
		m_bStage3_1_3 = true;
		Wave_Stage3_1_3();
		break;
	case STAGE4_1_1:	// 다 마찬가지
		m_bStage4_1_1 = true;
		Wave_Stage4_1_1();
		break;
	case STAGE4_1_2:
		m_bStage4_1_2 = true;
		Wave_Stage4_1_2();
		break;
	case STAGE4_1_3:	
		m_bStage4_1_3 = true;
		Wave_Stage4_1_3();
		break;
	}
}

void CMonsterWave::ReleaseWall()
{
	auto WallList = g_pGameInstance->Get_ObjectList(g_eCurrentLevel, TEXT("Layer_Wall"));
	if (nullptr == WallList)
		return;

	for (auto pWall : *WallList)
		static_cast<CStageWall*>(pWall)->Restore();

	if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Wall"))))
	{
		MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CMonsterWave::Update");
		return;
	}
}

HRESULT CMonsterWave::Add_Monster(LEVEL eLevel, _uint iCellIndex, const _tchar * pLayerTag)
{
	CCell* pCell = static_cast<CNavigation*>(g_pGameInstance
		->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Navigation")))
		->Get_Cell(iCellIndex);

	_vector vPosition = XMVectorSetW((pCell->Get_Point(CCell::POINT_A) 
		+ pCell->Get_Point(CCell::POINT_B) + pCell->Get_Point(CCell::POINT_C)) * 0.333f, 1.f);

	if (FAILED(g_pGameInstance->ThrowToLayer(eLevel, pLayerTag)))
	{
		MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CMonsterWave::Add_Monster");
		return E_FAIL;
	}

	CMonster* pMonster = static_cast<CMonster*>(g_pGameInstance->Get_Back(eLevel, pLayerTag));
	m_Monsters.push_back(pMonster);

	if (!lstrcmp(pLayerTag, TEXT("Layer_UFO")))
		static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")))
		->Set_State(CTransform::STATE_POSITION, vPosition + XMVectorSet(0.f, 8.f, 0.f, 0.f));
	else
		static_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")))
		->Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

void CMonsterWave::Wave_Stage3_1_1()
{
	
	// 생성할 셀 중앙에서 태어나게 하고

	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 13, TEXT("Layer_Fish"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_1(Fish)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Fish")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_1(Fish)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 28, TEXT("Layer_Fish"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_1(Fish)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Fish")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_1(Fish)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 32, TEXT("Layer_Fish"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_1(Fish)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Fish")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_1(Fish)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 44, TEXT("Layer_Fish"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_1(Fish)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Fish")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_1(Fish)");
		return;
	}
	static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Fish")))->Set_DropWeapon(true, 5);

	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 38, TEXT("Layer_Fish"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_1(Layer_Fish)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Fish")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_1(Layer_Fish)");
		return;
	}
}

void CMonsterWave::Wave_Stage3_1_2()
{
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 70, TEXT("Layer_Fish"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_2(Fish)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Fish")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_2(Fish)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 78, TEXT("Layer_Fish"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_2(Fish)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Fish")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_2(Fish)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 81, TEXT("Layer_Yacha"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_2(Yacha)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Yacha")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_2(Layer_Yacha)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 90, TEXT("Layer_Yacha"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_2(Yacha)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Yacha")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_2(Layer_Yacha)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 124, TEXT("Layer_Yacha"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_2(Kappa)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Yacha")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_2(Kappa)");
		return;
	}
	static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Yacha")))->Set_DropWeapon(true,2);
}

void CMonsterWave::Wave_Stage3_1_3()
{
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 115, TEXT("Layer_Fish"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_3(Fish)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Fish")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_3(Fish)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 113, TEXT("Layer_Lobstar"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_3(Lobstar)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Lobstar")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_3(Lobstar)");
		return;
	}
	static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Lobstar")))->Set_DropWeapon(true, 4);
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 104, TEXT("Layer_Yacha"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_3(Yacha)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Yacha")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_3(Yacha)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 109, TEXT("Layer_Yacha"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_3(Yacha)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Yacha")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_3(Yacha)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 111, TEXT("Layer_Monk"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_3(Monk)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Monk")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_3(Monk)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE3_1, 110, TEXT("Layer_Monk"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage3_1_3(Monk)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_1, TEXT("Layer_Monk")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage3_1_3(Monk)");
		return;
	}
}

void CMonsterWave::Wave_Stage4_1_1()
{
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 6, TEXT("Layer_Monkey"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_1(Monkey)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Monkey")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_1(Monkey)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 24, TEXT("Layer_Monkey"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_1(Monkey)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Monkey")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_1(Monkey)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 26, TEXT("Layer_Monkey"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_1(Monkey)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Monkey")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_1(Monkey)");
		return;
	}

	static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Monkey")))->Set_DropWeapon(true, 3);
	//if (FAILED(Add_Monster(LEVEL_STAGE4_1, 27, TEXT("Layer_UFO"))))
	//{
	//	MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_1(UFO)");
	//	return;
	//}
	//if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_UFO")))->StartSetting()))
	//{
	//	MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_1(UFO)");
	//	return;
	//}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 30, TEXT("Layer_Ghost"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_1(Ghost)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Ghost")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_1(Ghost)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 33, TEXT("Layer_Ghost"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_1(Ghost)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Ghost")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_1(Ghost)");
		return;
	}
}

void CMonsterWave::Wave_Stage4_1_2()
{
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 41, TEXT("Layer_Fox"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_2(Fox)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Fox")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_2(Fox)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 48, TEXT("Layer_Monkey"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_2(Monkey)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Monkey")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_2(Monkey)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 51, TEXT("Layer_Monkey"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_2(Monkey)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Monkey")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_2(Monkey)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 61, TEXT("Layer_Fox"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_2(Fox)");
		return;
	}
	static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Fox")))->Set_DropWeapon(true, 1);

	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Fox")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_2(Fox)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 55, TEXT("Layer_Ghost"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_2(Ghost)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Ghost")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_2(Ghost)");
		return;
	}
}

void CMonsterWave::Wave_Stage4_1_3()
{
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 83, TEXT("Layer_Bear"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_3(Bear)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Bear")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_3(Bear)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 95, TEXT("Layer_Bear"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_3(Bear)");
		return;
	}
	static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Bear")))->Set_DropWeapon(true, 0);
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Bear")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_3(Bear)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 98, TEXT("Layer_Bear"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_3(Bear)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Bear")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_3(Bear)");
		return;
	}
	//if (FAILED(Add_Monster(LEVEL_STAGE4_1, 88, TEXT("Layer_UFO"))))
	//{
	//	MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_3(UFO)");
	//	return;
	//}
	//if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_UFO")))->StartSetting()))
	//{
	//	MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_3(UFO)");
	//	return;
	//}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 89, TEXT("Layer_Fox"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_3(Fox)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Fox")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_3(Fox)");
		return;
	}
	if (FAILED(Add_Monster(LEVEL_STAGE4_1, 97, TEXT("Layer_Ghost"))))
	{
		MSGBOX("Add_Monster returned E_FAIL in Wave_Stage4_1_3(Ghost)");
		return;
	}
	if (FAILED(static_cast<CMonster*>(g_pGameInstance->Get_Back(LEVEL_STAGE4_1, TEXT("Layer_Ghost")))->StartSetting()))
	{
		MSGBOX("g_pGameInstance->Get_Back returned E_FAIL in Wave_Stage4_1_3(Ghost)");
		return;
	}
}

void CMonsterWave::Free()
{
	m_Monsters.clear();
	m_bStage3_1_1 = false;
	m_bStage3_1_2 = false;
	m_bStage3_1_3 = false;
	m_bStage4_1_1 = false;
	m_bStage4_1_2 = false;
	m_bStage4_1_3 = false;
}
