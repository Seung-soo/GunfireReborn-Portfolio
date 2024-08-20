#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "VIBuffer_Cube.h"
#include "Sky.h"
#include "Player.h"
#include "Terrain.h"
#include "Camera_Dynamic.h"
#include "Model.h"
#include "Monster.h"
#include "Load_Manager.h"
#include "Mesh.h"
#include "HpBar.h"
#include "HpNumber.h"
#include "PoisonSmoke.h"
#include "EnergyOrb.h"
#include "Aim.h"
#include "Player_Hand.h"
#include "Ocean.h"
#include "TabUI.h"
#include "CatfishWarrior.h"
#include "Yacha.h"
#include "Kappa.h"
#include "Lobstar.h"
#include "Monk.h"
#include "Monkey.h"
#include "Ghost.h"
#include "Bear.h"
#include "UFO.h"
#include "Fox.h"
#include "Bullet_Tesla.h"
#include "Bullet_TigerCannon.h"
#include "Bullet_ThunderClapGloves.h"
#include "TigerCannon_Explosion.h"
#include "HpBack.h"
#include "MonsterHpBar.h"
#include "YachaBullet.h"
#include "UFOBullet.h"
#include "KappaFire.h"
#include "ElementFire.h"
#include "PlayerHit.h"
#include "TakeAim.h"
#include "LobstarBullet.h"
#include "UFOBuff.h"
#include "Navigation.h"
#include "AbyssalSerpent.h"
#include "Wave.h"
#include "WaterBall.h"
#include "WaterBallBomb.h"
#include "BossHp.h"
#include "Mini.h"
#include "MiniBullet.h"
#include "PoleMonarch.h"
#include "SwordPower.h"
#include "BossCannon.h"
#include "BossCannonBullet.h"
#include "BossGun.h"
#include "BossGunBullet.h"
#include "RevolutionRock.h"
#include "Item_Weapon.h"
#include "Item_Bullet.h"
#include "Item_Coin.h"
#include "NumberUI.h"
#include "StageWall.h"
#include "NextAria.h"
#include "MiniMap.h"
#include "MiniMap_Icon.h"
#include "SkillBox.h"
#include "Goblet.h"
#include "Victory_UI.h"
#include "VictoryRing.h"
#include "VictoryRock.h"
#include "VictoryShine.h"
#include "Talent_Icon.h"
#include "Talent_Info.h"
#include "Skill_Icon.h"
#include "Skill_Info.h"
#include "TalentBack.h"
#include "Talent.h"
#include "Weapon_Info.h"
#include "StageName.h"
#include "Damage.h"
#include "DamageNumber.h"
#include "ElementIcon.h"
#include "ShopUI.h"
#include "Product.h"
#include "ProductInfo.h"
#include "NPC_Shop.h"
#include "CoolTime.h"
#include "VIBuffer_Point_Instance.h"
#include "Particle.h"
#include "Chain.h"
#include "Effect.h"
#include "DashEffect.h"
#include "NoiseRect.h"
#include "Bullet_FireDragon.h"
#include "SwordTrail.h"
#include "BulletGlow_Tesla.h"
#include "TeslaHit.h"
#include "MonsterSwordTrail.h"
#include "BearAttack.h"
#include "MonkAttack.h"
#include "GhostBullet.h"
#include "GhostBulletEffect.h"
#include "GhostBlinkEffect.h"
#include "FoxBullet.h"
#include "FoxBulletEffect.h"
#include "UFORay.h"
#include "LobstarSholder.h"
#include "MonkSmallEffect.h"
#include "BearAttackSmoke.h"
#include "HitWave.h"
#include "RushWind.h"
#include "WaterEffect.h"
#include "RushWarning.h"
#include "WaterBallWarning.h"
#include "ReadyWaterBall.h"
#include "BossWave.h"
#include "WaterMoveWave.h"
#include "SwordPowerTrail.h"
#include "BossBlinkSmoke.h"
#include "BossWind.h"
#include "RockWind.h"
#include "BreakRock.h"
#include "RockWarning.h"
#include "EndPhase.h"
#include "BossTornado.h"
#include "FireLight.h"
#include "Start.h"
#include "Exit.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

unsigned int APIENTRY ThreadFunction(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CS());

	switch (pLoader->Get_LevelID())
	{
	case LEVEL_MAINMANU:
		pLoader->Loading_ForMainManu();
		break;
	case LEVEL_LOBY:
		pLoader->Loading_ForLoby();
		break;
	case LEVEL_STAGE3_1:
		pLoader->Loading_ForStage3_1();
		break;
	case LEVEL_STAGE3_2:
		pLoader->Loading_ForStage3_2();
		break;
	case LEVEL_STAGE4_1:
		pLoader->Loading_ForStage4_1();
		break;
	case LEVEL_STAGE4_2:
		pLoader->Loading_ForStage4_2();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CS());

	return 0;
}



HRESULT CLoader::NativeConstruct(LEVEL eNextLevel)
{
	InitializeCriticalSection(&m_CS);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunction, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	m_eLevel = eNextLevel;

	return S_OK;
}

HRESULT CLoader::Loading_ForMainManu()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINMANU, TEXT("Prototype_Component_Texture_Start"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Start.png")))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINMANU, TEXT("Prototype_Component_Texture_Exit"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Exit.png")))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Start"), CStart::Create(m_pDevice, m_pDeviceContext))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Exit"), CExit::Create(m_pDevice, m_pDeviceContext))))
	{
		__debugbreak();
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForLoby()
{
	//for (size_t i = 0; i < 3999999999; i++)
	//{
	//	int a = 0;
	//}
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* 원형컴포넌트를 생성한다. */
#pragma region PROTOTYPE_COMPONENT
	// VIBuffer_Cube
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_VIBuffer_Cube)");
		return E_FAIL;
	}

	// VIBuffer_RectXZ
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectXZ"), CVIBuffer_RectXZ::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_VIBuffer_RectXZ)");
		return E_FAIL;
	}

	// VIBuffer_Terrain_Ocean
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_Ocean"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, 193, 193))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_VIBuffer_Cube)");
		return E_FAIL;
	}

	// VIBuffer_Point_Instance_SideDrop
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_SideDrop"), CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, 100))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_VIBuffer_Point_Instance_SideDrop)");
		return E_FAIL;
	}

	_matrix		TransformMatrix;

	/* 썬더 건틀릿*/
	TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f)) 
		* XMMatrixTranslation(0.1f, 0.f, -0.25f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ThunderClapGloves"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/1410_Right/"
			, "1410_Right.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_AngelicAura)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_ThunderClapGloves"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/101 #1625278/", "101 #1625278.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Player_ThunderClapGloves)");
		return E_FAIL;
	}
	/* 엔젤릭 */
	TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(-0.02f, 0.f, -0.35f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_AngelicAura"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/1102_Right/", "1102_Right.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_AngelicAura)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_AngelicAura"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/101 #1625278/", "101 #1625278.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Player_AngelicAura)");
		return E_FAIL;
	}

	/* 타이거캐논 */
	TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(186.f)) * XMMatrixTranslation(0.95f, -0.9f, 0.23f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TigerCannon"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/1401_Right/", "1401_Right.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_TigerCannon)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_TigerCannon"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/101 #1625278/", "101 #1625278.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Player_TigerCannon)");
		return E_FAIL;
	}

	/* 테슬라 */
	TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(185.f)) * XMMatrixTranslation(0.f, 0.f, 1.6f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tesla"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/1211_Right/", "1211_Right.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Tesla)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Tesla"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/101 #1625278/", "101 #1625278.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Player_Tesla)");
		return E_FAIL;
	}

	/* 솔라 피어서 */
	TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.4f, -0.53f, 0.13f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_PiercingFrame"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/1501_Right/", "1501_Right.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_PiercingFrame)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_PiercingFrame"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/101 #1625278/", "101 #1625278.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Player_PiercingFrame)");
		return E_FAIL;
	}

	/* 파이어 드래곤 */
	TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(187.f)) * XMMatrixTranslation(1.15f, -0.9f, 1.f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireDragon"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/1412_Right/", "1412_Right.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_FireDragon)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_FireDragon"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/101 #1625278/", "101 #1625278.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Player_FireDragon)");
		return E_FAIL;
	}

	/* 짐새의 망령 */
	TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(182.f)) * XMMatrixTranslation(1.1f, -0.9f, 1.08f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sword"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/1603_Right/", "1603_Right.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Sword)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Sword"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/101 #1625278/", "101 #1625278.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Player_Sword)");
		return E_FAIL;
	}

	/* 스킬 쓰는 팔 */
	TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.1f, 0.f, 0.f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Skill"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/101 #1625278/", "101 #1625278.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Player_Skill)");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	// Model_PoisonSmoke
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_PoisonSmoke"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Player/Dropltem_Bullet_Gren_205/", "Dropltem_Bullet_Gren_205.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_PoisonSmoke)");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixScaling(2.85f, 2.85f, 2.85f);

	// VIBuffer_Model_Sphere
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sphere"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Mesh/Sphere/", "Sphere.obj", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Sphere)");
		return E_FAIL;
	}

	// DropItem_Weapon
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_1211"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/1211/", "1211.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_1211)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_1401"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/1401/", "1401.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_1401)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_1410"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/1410/", "1410.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_1211)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_1501"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/1501/", "1501.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_1501)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_1603"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/1603/", "1603.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_1211)");
		return E_FAIL;
	}
	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 0.1f, -0.2f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_1412"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/NonAnimMesh/1412/", "1412.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_1412)");
		return E_FAIL;
	}

	// DropItem_Bullet
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_GreenBullet"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/NonAnimMesh/Dropltem_Bullet_Shrapnel/", "Dropltem_Bullet_Shrapnel.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_GreenBullet)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BlueBullet"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/NonAnimMesh/Dropltem_Bullet_Standard/", "Dropltem_Bullet_Standard.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_BlueBullet)");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_YellowBullet"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/NonAnimMesh/Dropltem_Bullet_Special/", "Dropltem_Bullet_Special.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_YellowBullet)");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	// DropItem_Coin
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Coin"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/Coin/", "Coin.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Coin)");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	// SkillBox
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SkillBox"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/Box/", "Box.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Box)");
		return E_FAIL;
	}

	// Goblet
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Goblet"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/Goblet/", "Goblet.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_Goblet)");
		return E_FAIL;
	}

	// NPC_Shop
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Shop"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/NPC_Shop_Skin #1721096/", "NPC_Shop_Skin #1721096.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_NPC_Shop)");
		return E_FAIL;
	}
	TransformMatrix = XMMatrixRotationX(XMConvertToRadians(90.f));
	// ChainCircle
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainCircle"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/ChainCircle/", "ChainCircle.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_ChainCircle)");
		return E_FAIL;
	}
	TransformMatrix = XMMatrixRotationZ(XMConvertToRadians(90.f));
	// ChainLine
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainLine"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/ChainCircle/", "Chain.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_ChainLine)");
		return E_FAIL;
	}
	TransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationZ(XMConvertToRadians(90.f));
	// ChainPiece0
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainPiece0"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/ChainCircle/", "Chain0.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_ChainPiece0)");
		return E_FAIL;
	}
	// ChainPiece1
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainPiece1"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/ChainCircle/", "Chain1.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_ChainPiece1)");
		return E_FAIL;
	}
	// ChainPiece2
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainPiece2"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/ChainCircle/", "Chain2.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_ChainPiece2)");
		return E_FAIL;
	}
	// ChainPiece3
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainPiece3"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/ChainCircle/", "Chain3.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_ChainPiece3)");
		return E_FAIL;
	}
	// ChainPiece4
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainPiece4"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/ChainCircle/", "Chain4.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_ChainPiece4)");
		return E_FAIL;
	}
	TransformMatrix = XMMatrixScaling(2.f, 2.f, 2.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	// EnergyOrb
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EnergyOrb"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/EnergyOrb/", "EnergyOrb.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_EnergyOrb)");
		return E_FAIL;
	}

	// OrbExplotion
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_OrbExplotion"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/ChainCircle/", "OrbExplotion.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_OrbExplotion)");
		return E_FAIL;
	}
	// SwordTrail
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SwordTrail"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Weapon/", "SwordTrail.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_SwordTrail)");
		return E_FAIL;
	}

	// TeslaHit
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TeslaHit"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Weapon/", "Thunder.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_TeslaHit)");
		return E_FAIL;
	}
	// YachaTrail
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_YachaTrail"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2126/", "YachaTrail.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_YachaTrail)");
		return E_FAIL;
	}
	// YachaTrail2
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_YachaTrail2"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2126/", "YachaTrail2.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_YachaTrail2)");
		return E_FAIL;
	}
	// YachaWaterSpark
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_YachaWaterSpark"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2126/", "WaterSpark.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_YachaWaterSpark)");
		return E_FAIL;
	}
	// YachaWaterAttack
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_YachaWaterAttack"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2126/", "WaterAttack1.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_YachaWaterAttack)");
		return E_FAIL;
	}
	// MonkeyTrail
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MonkeyTrail"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2089/", "MonkeyTrail.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_MonkeyTrail)");
		return E_FAIL;
	}
	// MonkAttack
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_1, TEXT("Prototype_Component_Model_MonkAttack"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2283/", "MonkAttack.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_MonkAttack)");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	// LobstarFireRing
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_1, TEXT("Prototype_Component_Model_LobstarFireRing"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2203/", "FireRing.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_LobstarFireRing)");
		return E_FAIL;
	}

	// LobstarFireBoom
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_1, TEXT("Prototype_Component_Model_LobstarFireBoom"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2203/", "FireBoom.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_LobstarFireBoom)");
		return E_FAIL;
	}

	// MonkSmallEffect
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_1, TEXT("Prototype_Component_Model_MonkSmallEffect"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2283/", "SmallEffect.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_Component_Model_MonkSmallEffect)");
		return E_FAIL;
	}
	// Prototype_Component_Model_RushWind
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_RushWind"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3921/", "RushWind.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_Component_Model_RushWind)");
		return E_FAIL;
	}
	// Prototype_Component_Model_BossWind
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BossWind"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3924/", "Wind.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_Component_Model_BossWind)");
		return E_FAIL;
	}

	// Navigation_Loby
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOBY, TEXT("Prototype_Component_Navigation_Loby"), CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Data/NaviMesh/Stage_Loby.dat")))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_NaviMesh)");
		return E_FAIL;
	}

	CLoad_Manager::GetInstance()->Load_Mesh(m_pDevice, m_pDeviceContext, LEVEL_LOBY, TEXT("../Data/Mesh/Mesh_Loby.dat"));

	// Texture_Sky
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/SkyBox/SkyBox%d.dds"), 3))))
		return E_FAIL;

	// Texture_UI_Number
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Number"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/%d.png"), 11))))
		return E_FAIL;

	// Texture_UI_Number_White
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Number_White"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Number/%d.png"), 12))))
		return E_FAIL;

	// Texture_UI_Number_Green
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Number_Green"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Number/Green%d.png"), 11))))
		return E_FAIL;

	// Texture_UI_Number_Blue
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Number_Blue"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Number/Blue%d.png"), 11))))
		return E_FAIL;

	// Texture_UI_Number_Yellow
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Number_Yellow"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Number/Yellow%d.png"), 11))))
		return E_FAIL;

	// Texture_EnergyOrb
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EnergyOrb"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/EnergyOrb.png")))))
		return E_FAIL;

	// Texture_UI_Aim
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Aim"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Aim.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stage3"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Map/Stage3.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WoodBox"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Map/woodbox.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_woodbox)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_bambooLeaf"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Map/bambooLeaf.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_bambooLeaf)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_bambooTree"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Map/bambooTree.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_bambooTree)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_grass"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Map/grass.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_grass)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WindmillBot"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Map/WindmillBot.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Prototype_Component_Texture_WindmillBot)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WindmillTop"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Map/WindmillTop.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Prototype_Component_Texture_WindmillTop)");
		return E_FAIL;
	}

	// Texture_Bullet_Tesla
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bullet_Tesla"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/Bullet_Tesla.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_Bullet_Tesla)");
		return E_FAIL;
	}

	// Texture_Bullet_TigerCannon
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bullet_TigerCannon"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/Bullet_TigerCannon.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_Bullet_TigerCannon)");
		return E_FAIL;
	}

	// Texture_Bullet_ThunderClapGloves
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bullet_ThunderClapGloves"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/laser.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_Bullet_ThunderClapGloves)");
		return E_FAIL;
	}

	// Texture_TigerCannon_Explosion
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TigerCannon_Explosion"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/Explosion.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_TigerCannon_Explosion)");
		return E_FAIL;
	}

	// Texture_MonsterHpBack
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBack"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/HpBack.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_HpBack)");
		return E_FAIL;
	}

	// Texture_MonsterHpBar
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterHpBar"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/HpBar.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_HpBar)");
		return E_FAIL;
	}

	// Texture_MonsterShieldBack
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterShieldBar"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/ShieldBar.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_ShieldBar)");
		return E_FAIL;
	}

	// Texture_MonsterGuardBar
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterGuardBar"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/GuardBar.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_GuardBar)");
		return E_FAIL;
	}

	// Texture_YachaBullet
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_YachaBullet"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/YachaBullet.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_YachaBullet)");
		return E_FAIL;
	}

	// Texture_FireBall
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_FireBall"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/FireBall.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_FireBall)");
		return E_FAIL;
	}

	//// Texture_PurpleFire
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_PurpleFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/PurpleFire.png")))))
	//{
	//	MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_PurpleFire)");
	//	return E_FAIL;
	//}
	// Texture_KappaFire
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_1, TEXT("Prototype_Component_Texture_KappaFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2281/KappaFire.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_KappaFire)");
		return E_FAIL;
	}

	// Texture_Hit
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PlayerHit"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/Hit.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_Hit)");
		return E_FAIL;
	}

	// Texture_Ray
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_Ray"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/Ray.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_Ray)");
		return E_FAIL;
	}

	// Texture_Wall_Stage3
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wall_Stage3"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/Stage3Wall.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Stage3Wall)");
		return E_FAIL;
	}

	// Texture_Wall_Portal
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Portal"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/Portal.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Portal)");
		return E_FAIL;
	}

	// Texture_NextAria
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NextAria"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/icon_target.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_NextAria)");
		return E_FAIL;
	}

	// Texture_MiniMap
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMap"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/minimap.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_MiniMap)");
		return E_FAIL;
	}

	// Texture_MiniMap_Icon
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMap_Icon"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/MiniMap_Icon%d.png"), 7))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_MiniMap_Icon)");
		return E_FAIL;
	}

	// Texture_Victory
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Victory"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/Victory.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Victory)");
		return E_FAIL;
	}

	// Texture_VictoryShine
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_VictoryShine"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/VictoryShine.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_VictoryShine)");
		return E_FAIL;
	}

	// Texture_VictoryRing
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_VictoryRing"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/VictoryRing.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_VictoryRing)");
		return E_FAIL;
	}

	// Texture_VictoryRock
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_VictoryRock"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/VictoryRock%d.png"), 4))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_VictoryRock)");
		return E_FAIL;
	}

	// Texture_Talent_Icon
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Talent_Icon"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Talent_Icon%d.png"), 6))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Talent_Icon)");
		return E_FAIL;
	}

	// Texture_Talent_Info
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Talent_Info"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/TalentInfo%d.png"), 6))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Talent_Info)");
		return E_FAIL;
	}

	// Texture_Talent
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Talent"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Talent%d.png"), 6))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Talent)");
		return E_FAIL;
	}

	// Texture_SkillInfo
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkillInfo"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/SkillInfo%d.png"), 4))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_SkillInfo)");
		return E_FAIL;
	}

	// Texture_TalentBack
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TalentBack"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Panel%d.png"), 5))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_TalentBack)");
		return E_FAIL;
	}

	// Texture_Weapon_Info
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Info"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Weapon%d.png"), 6))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Weapon_Info)");
		return E_FAIL;
	}

	// Texture_StageName
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StageName"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/StageName%d.png"), 9))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_StageName)");
		return E_FAIL;
	}

	// Texture_ElementIcon
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ElementIcon"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/ElementIcon%d.png"), 6))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_ElementIcon)");
		return E_FAIL;
	}

	// Texture_Shop
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Shop"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/ShopBack.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Shop)");
		return E_FAIL;
	}

	// Texture_Product
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Product"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/Product%d.png"), 6))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Product)");
		return E_FAIL;
	}

	// Texture_Product_Info
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Product_Info"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/ProductInfo%d.png"), 9))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Product_Info)");
		return E_FAIL;
	}

	// Texture_CoolTime
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CoolTime"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/MyUI/CoolTime%d.png"), 2))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_CoolTime)");
		return E_FAIL;
	}

	// Texture_LightCircle
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LightCircle"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/EnergyOrb/LightCircle.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_LightCircle)");
		return E_FAIL;
	}

	// Texture_EnergyOrb_Lightning
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EnergyOrb_Lightning"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/EnergyOrb/ligtning_10.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_LightCircle)");
		return E_FAIL;
	}

	// Texture_DashEffect
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DashEffect"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Dash/Dash%d.png"), 8))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_DashEffect)");
		return E_FAIL;
	}

	// Texture_SmokeGlow
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SmokeGlow"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Smoke/Glow%d.png"), 2))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_SmokeGlow)");
		return E_FAIL;
	}

	// Texture_PoisonSmoke
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PoisonSmoke"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Smoke/Smoke.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_PoisonSmoke)");
		return E_FAIL;
	}

	// Texture_AngleMuzzle
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AngelMuzzle"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/WeaponMuzzle/AngelMuzzle.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_AngleMuzzle)");
		return E_FAIL;
	}

	// Texture_WeaponSmoke
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponSmoke"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/Smoke2.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_WeaponSmoke)");
		return E_FAIL;
	}

	// Texture_WeaponExplotion
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponExplotion"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/explo0.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_WeaponExplotion)");
		return E_FAIL;
	}

	// Texture_BulletGlow
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BulletGlow"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/BulletGlow.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_BulletGlow)");
		return E_FAIL;
	}

	// Texture_BulletGlow_Tesla
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BulletGlow_Tesla"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/BulletGlow_Tesla.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_BulletGlow_Tesla)");
		return E_FAIL;
	}

	// Texture_CannonMuzzle
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CannonMuzzle"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/WeaponMuzzle/CannonMuzzle.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_CannonMuzzle)");
		return E_FAIL;
	}

	// Texture_GloveLogo
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GloveLogo"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/GloveLogo.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_GloveLogo)");
		return E_FAIL;
	}

	// Texture_Scope
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Scope"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/Snipe.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Scope)");
		return E_FAIL;
	}
	// Texture_SnipeDark
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SnipeDark"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/SnipeDark.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_SnipeDark)");
		return E_FAIL;
	}
	// Texture_SnipeSmoke
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SnipeSmoke"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/SnipeSmoke.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_SnipeSmoke)");
		return E_FAIL;
	}
	// Texture_Hit_Yellow
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Yellow"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/Hit_Yellow.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_Hit_Yellow)");
		return E_FAIL;
	}
	// Texture_NoiseFire
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/NoiseRect/noise (%d).png"), 99))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_NoiseFire)");
		return E_FAIL;
	}
	// Texture_DestFire
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DestFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/NoiseRect/FireDragonEffect.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_DestFire)");
		return E_FAIL;
	}
	// Texture_FireDragonEffect
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireDragonEffect"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/NoiseRect/FireDragonEffect.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_FireDragonEffect)");
		return E_FAIL;
	}
	// Texture_TeslaMuzzleFlash
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TeslaMuzzleFlash"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/TeslaMuzzleFlash.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_TeslaMuzzleFlash)");
		return E_FAIL;
	}
	// Texture_TeslaMuzzleLightning
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TeslaMuzzleLightning"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/TeslaMuzzleLightning.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_TeslaMuzzleLightning)");
		return E_FAIL;
	}
	// Texture_HitSword
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HitSword"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/SwordHit.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_HitSword)");
		return E_FAIL;
	}
	// Texture_FireDragonBomb
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireDragonBomb"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/FireDragonBomb.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_FireDragonBomb)");
		return E_FAIL;
	}
	// Texture_CatfishWarriorTrail
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CatfishWarriorTrail"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2087/CatfishWarriorTrail.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_CatfishWarriorTrail)");
		return E_FAIL;
	}
	// Texture_YachaTrail
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_YachaTrail"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2126/YachaTrail.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_YachaTrail)");
		return E_FAIL;
	}
	// Texture_YachaTrail2
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_YachaTrail2"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2126/YachaTrail2.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_YachaTrail2)");
		return E_FAIL;
	}
	// Texture_YachaWaterSpark
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_YachaWaterSpark"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2126/WaterSpark.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_YachaWaterSpark)");
		return E_FAIL;
	}
	// Texture_MonkeyTrail
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonkeyTrail"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2089/MonkeyTrail.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_MonkeyTrail)");
		return E_FAIL;
	}
	// Texture_BearTrail
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BearTrail"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2128/BearTrail.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_BearTrail)");
		return E_FAIL;
	}
	// Texture_MonkFlashLight
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonkFlashLight"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2283/MonkFlashLight.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_MonkFlashLight)");
		return E_FAIL;
	}
	// Texture_MonkMuzzle
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonkMuzzle"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2283/MonkMuzzle.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_MonkMuzzle)");
		return E_FAIL;
	}
	// Texture_LobstarBullet
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_1, TEXT("Prototype_Component_Texture_LobstarBullet"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2203/MixBulletFire.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_LobstarBullet)");
		
		return E_FAIL;
	}

	// Texture_LobstarBulletFire
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LobstarBulletFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2203/fire_23.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_LobstarBulletFire)");

		return E_FAIL;
	}
	// Texture_LobstarBoomGlow
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LobstarBoomGlow"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2203/BoomGlow.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_LobstarBoomGlow)");
		return E_FAIL;
	}
	// Texture_LobstarBoomLight
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LobstarBoomLight"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2203/BoomLight.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_LobstarBoomLight)");
		return E_FAIL;
	}
	// Texture_LobstarBoomRing
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LobstarBoomRing"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2203/BoomRing.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_LobstarBoomRing)");
		return E_FAIL;
	}
	// Texture_MuzzleBomb
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MuzzleBomb"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2203/MuzzleBomb.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_MuzzleBomb)");
		return E_FAIL;
	}
	// Texture_TeslaBoom
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TeslaBoom"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Weapon/TeslaBoom.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_TeslaBoom)");
		return E_FAIL;
	}
	// Texture_GhostBoom
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GhostBoom"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2164/GhostBoom.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_GhostBoom)");
		return E_FAIL;
	}
	// Texture_BlueLight
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_BlueLight"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/FireLight/BlueLight%d.png"), 2))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Texture_BlueLight)");
		return E_FAIL;
	}
	// Texture_BurningFire
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BurningFire"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Element/Fire%d.png"), 3))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_Component_Texture_BurningFire)");
		return E_FAIL;
	}
	// Texture_ShockLightning
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShockLightning"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Element/Lightning%d.png"), 4))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_Component_Texture_ShockLightning)");
		return E_FAIL;
	}
	// Prototype_Component_Texture_DecayEffect
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DecayEffect"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Element/Decay.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_Component_Texture_DecayEffect)");
		return E_FAIL;
	}
	// Prototype_Component_Texture_Mani
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mani"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Element/W_%d.png"), 6))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_Component_Texture_DecayEffect)");
		return E_FAIL;
	}
	// Prototype_Component_Texture_MiasmaPoison
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiasmaPoison"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Element/MiasmaPoison.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_Component_Texture_MiasmaPoison)");
		return E_FAIL;
	}

	// Shader_Cube
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	// Model Shader
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/NonAnimModel.hlsl"), VTXNONANIMMODELTEX_DECLARATION::Elements, VTXNONANIMMODELTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/AnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_PointInstance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_PointInstance"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointINstance.hlsl"), VTXPOINT_MATRIX_DECLARATION::Elements, VTXPOINT_MATRIX_DECLARATION::iNumElements))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Point_Instance_Shader)");
		return E_FAIL;
	}

#pragma endregion

	// 원형 객체
#pragma  region PROTOTYPE_GAMEOBJECT
	// GameObject Sky
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Sky)");
		return E_FAIL;
	}
	// GameObject Player
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Player)");
		return E_FAIL;
	}
	// GameObject Player
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Hand"), CPlayer_Hand::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Player_Hand)");
		return E_FAIL;
	}

	// GameObject Camera_Dynamic
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"), CCamera_Dynamic::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Camera)");
		return E_FAIL;
	}
	// GameObject Terrain
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Terrain)");
		return E_FAIL;
	}

	// GameObject Ocean
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ocean"), COcean::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Ocean)");
		return E_FAIL;
	}

	// GameObject Mesh
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mesh"), CMesh::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Mesh)");
		return E_FAIL;
	}

	//for (size_t i = 0; i < 10; i++)
	//{
	//	if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_LOBY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"))))
	//	{
	//		MSGBOX("pGameInstance->Add_ObjectPool returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Monster)");
	//		return E_FAIL;
	//	}
	//}

	// GameObject HpBar
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpBar"), CHpBar::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_CHpBar)");
		return E_FAIL;
	}

	// GameObject HpNumber
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HpNumber"), CHpNumber::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_CHpNumber)");
		return E_FAIL;
	}

	// GameObject PoisonSmoke
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PoisonSmoke"), CPoisonSmoke::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_PoisonSmoke)");
		return E_FAIL;
	}

	// GameObject EnergyOrb
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnergyOrb"), CEnergyOrb::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_EnergyOrb)");
		return E_FAIL;
	}

	// GameObject Aim
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Aim"), CAim::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Aim)");
		return E_FAIL;
	}

	// GameObject TabUI
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TabUI"), CTabUI::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_TabUI)");
		return E_FAIL;
	}

	// GameObject Bullet_Tesla
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_Tesla"), CBullet_Tesla::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Bullet_Tesla)");
		return E_FAIL;
	}

	// GameObject Bullet_TigerCannon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_TigerCannon"), CBullet_TigerCannon::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Bullet_TigerCannon)");
		return E_FAIL;
	}

	// GameObject Bullet_ThunderClapGloves
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_ThunderClapGloves"), CBullet_ThunderClapGloves::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Bullet_ThunderClapGloves)");
		return E_FAIL;
	}

	// GameObject TigerCannon_Explosion
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TigerCannon_Explosion"), CTigerCannon_Explosion::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_TigerCannon_Explosion)");
		return E_FAIL;
	}

	// GameObject HpBack
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpBack"), CHpBack::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_HpBack)");
		return E_FAIL;
	}

	// GameObject MonsterHpBar
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterHpBar"), CMonsterHpBar::Create(m_pDevice, m_pDeviceContext, CMonsterHpBar::HP))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_MonsterHpBar)");
		return E_FAIL;
	}
	// GameObject MonsterShieldBar
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterShieldBar"), CMonsterHpBar::Create(m_pDevice, m_pDeviceContext, CMonsterHpBar::SHIELD))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_MonsterShieldBar)");
		return E_FAIL;
	}
	// GameObject MonsterGuardBar
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterGuardBar"), CMonsterHpBar::Create(m_pDevice, m_pDeviceContext, CMonsterHpBar::GUARD))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_MonsterGuardBar)");
		return E_FAIL;
	}

	// GameObject YachaBullet
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_YachaBullet"), CYachaBullet::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_YachaBullet)");
		return E_FAIL;
	}

	// GameObject UFOBullet
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UFOBullet"), CUFOBullet::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_UFOBullet)");
		return E_FAIL;
	}

	// GameObject KappaFire
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KappaFire"), CKappaFire::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_KappaFire)");
		return E_FAIL;
	}

	// GameObject ElementFire
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ElementFire"), CElementFire::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_ElementFire)");
		return E_FAIL;
	}

	// GameObject PlayerHit
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerHit"), CPlayerHit::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_PlayerHit)");
		return E_FAIL;
	}

	// GameObject TakeAim
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TakeAim"), CTakeAim::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_SnipeRay)");
		return E_FAIL;
	}

	// GameObject LobstarBullet
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LobstarBullet"), CLobstarBullet::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_LobstarBullet)");
		return E_FAIL;
	}

	// GameObject Item_Weapon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Weapon"), CItem_Weapon::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_tem_Weapon)");
		return E_FAIL;
	}

	// GameObject Item_Bullet
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Bullet"), CItem_Bullet::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Item_Bullet)");
		return E_FAIL;
	}

	// GameObject Item_Coin
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Coin"), CItem_Coin::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Item_Coin)");
		return E_FAIL;
	}

	// GameObject Item_Smoke
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Smoke"), CItem_Smoke::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Item_Smoke)");
		return E_FAIL;
	}

	// GameObject NumberUI
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NumberUI"), CNumberUI::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_NumberUI)");
		return E_FAIL;
	}

	// GameObject StageWall
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StageWall"), CStageWall::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_StageWall)");
		return E_FAIL;
	}

	// GameObject NextAria
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NextAria"), CNextAria::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_NextAria)");
		return E_FAIL;
	}

	// GameObject MiniMap
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniMap"), CMiniMap::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_MiniMap)");
		return E_FAIL;
	}

	// GameObject MiniMap_Icon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniMap_Icon"), CMiniMap_Icon::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_MiniMap_Icon)");
		return E_FAIL;
	}

	// GameObject SkillBox
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkillBox"), CSkillBox::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_SkillBox)");
		return E_FAIL;
	}

	// GameObject Goblet
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goblet"), CGoblet::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Goblet)");
		return E_FAIL;
	}

	// GameObject Victory
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Victory"), CVictory_UI::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Victory)");
		return E_FAIL;
	}

	// GameObject VictoryRock
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VictoryRock"), CVictoryRock::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Rock)");
		return E_FAIL;
	}

	// GameObject VictoryShine
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VictoryShine"), CVictoryShine::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_VictoryShine)");
		return E_FAIL;
	}

	// GameObject VictoryRing
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VictoryRing"), CVictoryRing::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_VictoryRing)");
		return E_FAIL;
	}

	// GameObject Talent_Icon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Talent_Icon"), CTalent_Icon::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Talent_Icon)");
		return E_FAIL;
	}
	// GameObject Talent_Info
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Talent_Info"), CTalent_Info::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Talent_Info)");
		return E_FAIL;
	}
	// GameObject Skill_Icon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Icon"), CSkill_Icon::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Skill_Icon)");
		return E_FAIL;
	}
	// GameObject Skill_Info
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Info"), CSkill_Info::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Skill_Info)");
		return E_FAIL;
	}
	// GameObject TalentBack
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TalentBack"), CTalentBack::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_TalentBack)");
		return E_FAIL;
	}
	// GameObject Talent
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Talent"), CTalent::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Talent)");
		return E_FAIL;
	}
	// GameObject StageNameUI
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StageNameUI"), CStageName::Create(m_pDevice, m_pDeviceContext, false))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_StageNameUI)");
		return E_FAIL;
	}
	// GameObject StageNameStage
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StageNameStage"), CStageName::Create(m_pDevice, m_pDeviceContext, true))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_StageNameStage)");
		return E_FAIL;
	}
	// GameObject Damage
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Damage"), CDamage::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Damage)");
		return E_FAIL;
	}
	// GameObject DamageNumber
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DamageNumber"), CDamageNumber::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_DamageNumber)");
		return E_FAIL;
	}
	// GameObject ElementIcon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ElementIcon"), CElementIcon::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_ElementIcon)");
		return E_FAIL;
	}
	// GameObject Shop
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shop"), CShopUI::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Shop)");
		return E_FAIL;
	}
	// GameObject Product
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Product"), CProduct::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Product)");
		return E_FAIL;
	}
	// GameObject Product_Info
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Product_Info"), CProductInfo::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Product_Info)");
		return E_FAIL;
	}
	// GameObject NPC_Shop
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC_Shop"), CNPC_Shop::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_NPC_Shop)");
		return E_FAIL;
	}
	// GameObject CoolTime
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CoolTime"), CCoolTime::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_CoolTime)");
		return E_FAIL;
	}
	// GameObject Particle
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle"), CParticle::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Particle)");
		return E_FAIL;
	}
	// GameObject ChainCircle
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChainCircle"), CChain::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_ChainCircle)");
		return E_FAIL;
	}
	// GameObject Effect
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"), CEffect::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Effect)");
		return E_FAIL;
	}
	// GameObject DashEffect
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DashEffect"), CDashEffect::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_DashEffect)");
		return E_FAIL;
	}
	// GameObject NoiseRect
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NoiseRect"), CNoiseRect::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_NoiseRect)");
		return E_FAIL;
	}
	// GameObject BulletFireDragon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_FireDragon"), CBullet_FireDragon::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_Bullet_FireDragon)");
		return E_FAIL;
	}
	// GameObject SwordTrail
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwordTrail"), CSwordTrail::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_SwordTrail)");
		return E_FAIL;
	}
	// GameObject BulletGlow_Tesla
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BulletGlow_Tesla"), CBulletGlow_Tesla::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_BulletGlow_Tesla)");
		return E_FAIL;
	}
	// GameObject TeslaHit
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TeslaHit"), CTeslaHit::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_TeslaHit)");
		return E_FAIL;
	}
	// GameObject MonsterSwordTrail
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterSwordTrail"), CMonsterSwordTrail::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_MonsterSwordTrail)");
		return E_FAIL;
	}
	// GameObject MonkAttack
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonkAttack"), CMonkAttack::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_MonkAttack)");
		return E_FAIL;
	}
	// GameObject LobstarShoulder
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LobstarShoulder"), CLobstarSholder::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(GameObject_LobstarShoulder)");
		return E_FAIL;
	}
	// GameObject MonkSmallEffect
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonkSmallEffect"), CMonkSmallEffect::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_GameObject_MonkSmallEffect)");
		return E_FAIL;
	}
	// GameObject HitWave
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HitWave"), CHitWave::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_GameObject_HitWave)");
		return E_FAIL;
	}
	// GameObject RushWind
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RushWind"), CRushWind::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_GameObject_RushWind)");
		return E_FAIL;
	}
	// GameObject BossWind
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossWind"), CBossWind::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_GameObject_BossWind)");
		return E_FAIL;
	}
	// GameObject FireLight
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireLight"), CFireLight::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Prototype_GameObject_FireLight)");
		return E_FAIL;
	}


#pragma endregion

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForStage3_1()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CLoad_Manager::GetInstance()->Load_Mesh(m_pDevice, m_pDeviceContext, LEVEL_STAGE3_1, TEXT("../Data/Mesh/Mesh_Stage3_1.dat"));

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_1, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Data/Terrain_Stage3_1.dat")))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage3_1");
		return E_FAIL;
	}

	// Texture_KappaFireBomb
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_KappaFireBomb"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2281/KappaFireBomb.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(Texture_KappaFireBomb)");
		return E_FAIL;
	}

	// Prototype_Component_Texture_AppearMonster
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AppearMonster"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/AppearMonster.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(Prototype_Component_Texture_AppearMonster)");
		return E_FAIL;
	}

	// Navigation_Stage3_1
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_1, TEXT("Prototype_Component_Navigation_Stage3_1"), CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Data/NaviMesh/Stage3_1.dat")))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(Component_NaviMesh)");
		return E_FAIL;
	}

	_matrix		TransformMatrix;

	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	// Model_CatfishWarrior
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_CatfishWarrior"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/2087_model/", "2087_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(Component_Model_CatfishWarrior)");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	// Model_Yacha
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_Yacha"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/2126_model/", "2126_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(Component_Model_Yacha)");
		return E_FAIL;
	}

	// Model_Kappa
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_Kappa"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/2281_model/", "2281_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(Component_Model_Kappa)");
		return E_FAIL;
	}

	// Model_Lobstar
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_Lobstar"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/2203_model/", "2203_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(Component_Model_Lobstar)");
		return E_FAIL;
	}

	// Model_Monk
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_Monk"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/2283_model/", "2283_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(Component_Model_Monk)");
		return E_FAIL;
	}
	// Model_AppearMonster
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_AppearMonster"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/", "AppearMonster.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(Prototype_Component_Model_AppearMonster)");
		return E_FAIL;
	}


	// GameObject Lobstar
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobstar"), CLobstar::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(GameObject_Lobstar)");
		return E_FAIL;
	}

	// GameObject Monk
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monk"), CMonk::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(GameObject_Monk)");
		return E_FAIL;
	}


	// GameObject CatfishWarrior
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CatfishWarrior"), CCatfishWarrior::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(GameObject_CatfishWarrior)");
		return E_FAIL;
	}

	// GameObject Yacha
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yacha"), CYacha::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(GameObject_Yacha)");
		return E_FAIL;
	}

	// GameObject Kappa
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kappa"), CKappa::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(GameObject_Kappa)");
		return E_FAIL;
	}

	// GameObject Weapon_Info
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Info"), CWeapon_Info::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_1(GameObject_Weapon_Info)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_ForStage3_2()
{
	CLoad_Manager::GetInstance()->Load_Mesh(m_pDevice, m_pDeviceContext, LEVEL_STAGE3_2, TEXT("../Data/Mesh/Mesh_Stage3_2.dat"));

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Data/Terrain_Stage3_2.dat")))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2");
		return E_FAIL;
	}
	// Collider AABB
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Collider_AABB)");
		return E_FAIL;
	}

	// Collider OBB
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Collider_OBB)");
		return E_FAIL;
	}

	// Navigation_Stage3_2
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Navigation_Stage3_2"), CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Data/NaviMesh/Stage3_2.dat")))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Component_NaviMesh)");
		return E_FAIL;
	}
	_matrix		TransformMatrix;

	TransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(14.5f)) * XMMatrixTranslation(16.6f, 0.f, 30.53f);

	// Component_Model_AbyssalSerpent
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_AbyssalSerpent"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/3921/", "3921.fbx", TransformMatrix))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Component_Model_AbyssalSerpent)");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(5.13f, 0.f, -2.53f);
	// Component_Model_Mini
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_Mini"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/3921_mini/", "3921_mini.fbx", TransformMatrix))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Component_Model_Mini)");
		return E_FAIL;
	}

	_matrix MatTransformMatrix = XMMatrixScaling(3.f, 3.f, 8.f);
	// Model_WaveSphere
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_WaveSphere"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Mesh/Sphere/", "Wave.fbx", MatTransformMatrix))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Component_Model_WaveSphere)");
		return E_FAIL;
	}	// Prototype_Component_Model_WaterBall
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_WaterBall"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3921/", "WaterBall.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Model_WaterBall)");
		return E_FAIL;
	}
	// Prototype_Component_Model_WaterEffect
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_WaterEffect"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3921/", "WaterEffect.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Model_WaterEffect)");
		return E_FAIL;
	}
	// Prototype_Component_Model_ReadyWaterBall
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_ReadyWaterBall"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3921/", "ReadyWaterBall.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Model_ReadyWaterBall)");
		return E_FAIL;
	}
	// Prototype_Component_Model_BossWave
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_BossWave"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3921/", "BossWave.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Model_BossWave)");
		return E_FAIL;
	}
	// Prototype_Component_Model_WaterMoveWave
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_WaterMoveWave"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3921/", "WaterMoveWave.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Model_WaterMoveWave)");
		return E_FAIL;
	}
	// Prototype_Component_Model_MiniBullet
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_MiniBullet"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3921/", "MiniBullet.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Model_MiniBullet)");
		return E_FAIL;
	}

	// Texture_WaveSphere
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_WaveSphere"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Mesh/Sphere/Sphere.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Component_Texture_WaveSphere)");
		return E_FAIL;
	}
	// Texture_WaterBall
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_WaterBall"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3921/WaterBall.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Texture_WaterBall)");
		return E_FAIL;
	}
	// Texture_Water
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3921/Water%d.png"), 3))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Texture_Water)");
		return E_FAIL;
	}
	// Texture_MiniWater
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniWater"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3921/MiniWater%d.png"), 3))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Texture_MiniWater)");
		return E_FAIL;
	}
	// Texture_RushWarning
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_RushWarning"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3921/RushWarning.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Texture_RushWarning)");
		return E_FAIL;
	}
	// Texture_MaskFallWater
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_MaskFallWater"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3921/FallWater.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Texture_MaskFallWater)");
		return E_FAIL;
	}
	// Texture_WaterBallWarning
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_WaterBallWarning"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3921/WaterBallWarning.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Texture_WaterBallWarning)");
		return E_FAIL;
	}

	// Prototype_Component_Texture_Boss1Glow
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss1BoomGlow"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3921/BoomGlow.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Texture_Boss1Glow)");
		return E_FAIL;
	}
	// Prototype_Component_Texture_Boss1BoomRing
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss1BoomRing"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3921/BoomRing.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Texture_Boss1BoomRing)");
		return E_FAIL;
	}

	// Prototype_Component_Texture_AbyssalSerpent_Mask
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_AbyssalSerpent_Mask"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Animation/3921/3921_s.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_Component_Texture_AbyssalSerpent_Mask)");
		return E_FAIL;
	}

	// 게임 오브젝트 프로토타입
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AbyssalSerpent"), CAbyssalSerpent::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(GameObject_AbyssalSerpent)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mini"), CMini::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(GameObject_Mini)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniBullet"), CMiniBullet::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(GameObject_MiniBullet)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wave"), CWave::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(GameObject_Wave)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterBall"), CWaterBall::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(GameObject_WaterBall)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterBallBomb"), CWaterBallBomb::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(GameObject_WaterBallBomb)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossHp"), CBossHp::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(GameObject_BossHp)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterEffect"), CWaterEffect::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_GameObject_WaterEffect)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RushWarning"), CRushWarning::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_GameObject_RushWarning)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterBallWarning"), CWaterBallWarning::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_GameObject_WaterBallWarning)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ReadyWaterBall"), CReadyWaterBall::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_GameObject_ReadyWaterBall)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossWave"), CBossWave::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_GameObject_BossWave)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterMoveWave"), CWaterMoveWave::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage3_2(Prototype_GameObject_WaterMoveWave)");
		return E_FAIL;
	}
	for (_uint i = 0; i < 4; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE3_2, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_AbyssalSerpent"))))
		{
			MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLoader::Loading_ForStage3_2(GameObject_AbyssalSerpent)");
			return E_FAIL;
		}
	}

	for (_uint i = 0; i < 5; i++)
	{
		if (FAILED(g_pGameInstance->Add_ObjectPool(LEVEL_STAGE3_2, TEXT("Layer_Mini"), TEXT("Prototype_GameObject_Mini"), &_float4(_float(rand() % 20 + 17), 1.f, 102.f, 1.f))))
		{
			MSGBOX("g_pGameInstance->Add_ObjectPool returned E_FAIL in CLoader::Loading_ForStage3_2(GameObject_Mini)");
			return E_FAIL;
		}
	}


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForStage4_1()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stage4"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Map/Stage4.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_Loby(Texture)");
		return E_FAIL;
	}

	CLoad_Manager::GetInstance()->Load_Mesh(m_pDevice, m_pDeviceContext, LEVEL_STAGE4_1, TEXT("../Data/Mesh/Mesh_Stage4_1.dat"));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_1, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Data/Terrain_Stage4_1.dat")))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_1");
		return E_FAIL;
	}

	// Navigation_Stage4_1
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_1, TEXT("Prototype_Component_Navigation_Stage4_1"), CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Data/NaviMesh/Stage4_1.dat")))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Component_NaviMesh)");
		return E_FAIL;
	}


	// Texture_Wall_Stage4
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wall_Stage4"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/Stage4Wall.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Texture_Stage4Wall)");
		return E_FAIL;
	}

	// Texture_snow
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_Snow"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/snow.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Texture_Snow)");
		return E_FAIL;
	}
	// Texture_FoxMuzzle
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FoxMuzzle"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2110/FoxMuzzle.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Texture_FoxMuzzle)");
		return E_FAIL;
	}

	// Texture_BearAttackSmoke
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_BearAttackSmoke"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2128/Smoke.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Texture_BearAttackSmoke)");
		return E_FAIL;
	}

	_matrix		TransformMatrix;

	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	// Model_Monkey
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Monkey"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/2089_model/", "2089_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Component_Model_Monkey)");
		return E_FAIL;
	}

	// Model_Bear
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Bear"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/2128_model/", "2128_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Component_Model_Bear)");
		return E_FAIL;
	}

	// Model_Ghost
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Ghost"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/2164_model/", "2164_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Component_Model_Ghost)");
		return E_FAIL;
	}

	// Model_UFO
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_UFO"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/2401_model/", "2401_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Component_Model_UFO)");
		return E_FAIL;
	}

	// Model_Aura
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Aura"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/aura_straight/", "aura_straight.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Component_Model_aura_straight)");
		return E_FAIL;
	}

	// Model_Fox
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Fox"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/2110_model/", "2110_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(Component_Model_Fox)");
		return E_FAIL;
	}
	TransformMatrix = XMMatrixScaling(5.f, 8.f, 5.f);
	// BearAttack
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_BearAttack"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2128/", "BearAttack.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_BearAttack)");
		return E_FAIL;
	}
	// GhostBullet
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_GhostBullet"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2164/", "Bullet.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_GhostBullet)");
		return E_FAIL;
	}
	// GhostBulletEffect
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_GhostBulletEffect"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2164/", "BulletEffect.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_GhostBulletEffect)");
		return E_FAIL;
	}

	// GhostBlinkEffect
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_GhostBlinkEffect"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2164/", "BlinkEffect.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_GhostBlinkEffect)");
		return E_FAIL;
	}
	// FoxBulletEffect
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_FoxBulletEffect"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2110/", "BulletEffect.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_FoxBulletEffect)");
		return E_FAIL;
	}
	// UFORay
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_UFORay"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2401/", "UFOBuffEffect.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_UFORay)");
		return E_FAIL;
	}
	// GhostTakeAim
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_TakeAim"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/2164/", "TakeAim.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForLoby(Component_Model_TakeAim)");
		return E_FAIL;
	}



	// GameObject UFO
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UFO"), CUFO::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_UFO)");
		return E_FAIL;
	}

	// GameObject Fox
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fox"), CFox::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_Fox)");
		return E_FAIL;
	}
	// GameObject Aura
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aura"), CUFOBuff::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_Aura)");
		return E_FAIL;
	}


	// GameObject Monkey
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monkey"), CMonkey::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_Monkey)");
		return E_FAIL;
	}

	// GameObject Bear
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bear"), CBear::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_Bear)");
		return E_FAIL;
	}

	// GameObject Ghost
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ghost"), CGhost::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_Ghost)");
		return E_FAIL;
	}
	// GameObject BearAttack
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BearAttack"), CBearAttack::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_BearAttack)");
		return E_FAIL;
	}
	// GameObject GhostBullet
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GhostBullet"), CGhostBullet::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_GhostBullet)");
		return E_FAIL;
	}
	// GameObject GhostBulletEffect
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GhostBulletEffect"), CGhostBulletEffect::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_GhostBulletEffect)");
		return E_FAIL;
	}
	// GameObject GhostBlinkEffect
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GhostBlinkEffect"), CGhostBlinkEffect::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_GhostBlinkEffect)");
		return E_FAIL;
	}
	// GameObject FoxBullet
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FoxBullet"), CFoxBullet::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_FoxBullet)");
		return E_FAIL;
	}
	// GameObject FoxBulletEffect
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FoxBulletEffect"), CFoxBulletEffect::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_FoxBulletEffect)");
		return E_FAIL;
	}
	// GameObject UFORay
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UFORay"), CUFORay::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_UFORay)");
		return E_FAIL;
	}
	// GameObject BearAttackSmoke
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BearAttackSmoke"), CBearAttackSmoke::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_1(GameObject_BearAttackSmoke)");
		return E_FAIL;
	}

	m_isFinished = true;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForStage4_2()
{
	CLoad_Manager::GetInstance()->Load_Mesh(m_pDevice, m_pDeviceContext, LEVEL_STAGE4_2, TEXT("../Data/Mesh/Mesh_Stage4_Boss.dat"));

	// 텍스쳐
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Map/1040101_terrain.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_Boss)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossSnow"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Map/1040101_terrain_snow.dds")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_BossSnow)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_BossCannonBullet"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/39247_glow_02.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_BossCannonBullet)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_BossGunBullet"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Image/Bullet_Red.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_BossGunBullet)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_Boss_Ice1"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Animation/3924_model/3924shadow_1_c.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_Boss_Ice1)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_Boss_Ice2"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Animation/3924_model/3924shadow_2_c.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Texture_Boss_Ice2)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossSwordTrail"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3924/SwordTrail.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Prototype_Component_Texture_BossSwordTrail)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GunMuzzle"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3924/GunMuzzle.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Prototype_Component_Texture_GunMuzzle)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_BossBlinkSmoke"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/2128/Smoke.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Prototype_Component_Texture_BossBlinkSmoke)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossCannonBulletLightning"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3924/BossCannonBulletLightning%d.png"), 3))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Prototype_Component_Texture_BossCannonBulletLightning)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_BossBlade"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3924/boss_blade.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Prototype_Component_Texture_BossBlade)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_BossBlades"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/Monster/3924/BossBlades.png")))))
	{
		MSGBOX("g_pGameInstance->Add_Prototype returned E_FAIL in CLoad_Manager::Load_Mesh(Prototype_Component_Texture_BossBlades)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Data/Terrain_Stage4_Boss.dat")))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2");
		return E_FAIL;
	}

	// Navigation_Stage4_1
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Navigation_Stage4_2"), CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Data/NaviMesh/Stage4_Boss.dat")))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(Component_NaviMesh)");
		return E_FAIL;
	}

	// 모델
	_matrix		TransformMatrix;

	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(170.f));

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_PoleMonarch"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/3924_model/", "3924_model.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Model_PoleMonarch)");
		return E_FAIL;
	}

	//TransformMatrix = XMMatrixScaling(0.04f, 0.04f, 0.04f) * XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 0.f, -3.f);
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_SwordPower"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3924/", "SwordPower.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Model_SwordPower)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_SwordPowerTrail"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3924/", "SwordPowerTrail.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_Component_Model_SwordPowerTrail)");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_BossCannon"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/39247_Weapon/", "39247_Weapon.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Model_BossCannon)");
		return E_FAIL;
	}
	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_BossGun"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Texture/Animation/39244_Weapon2/", "39244_Weapon2.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Model_BossGun)");
		return E_FAIL;
	}

	TransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Crystal1"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/Crystal1/", "Crystal1.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Model_Crystal1)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Crystal2"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/Crystal2/", "Crystal2.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Model_Crystal2)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Crystal3"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/Crystal3/", "Crystal3.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Model_Crystal3)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Crystal4"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/Crystal4/", "Crystal4.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Model_Crystal4)");
		return E_FAIL;
	}
	TransformMatrix = XMMatrixScaling(0.001f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_Crystal5"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/NonAnimMesh/Crystal5/", "Crystal5.fbx", TransformMatrix))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Model_Crystal5)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_RockWind"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3924/", "RockWind.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_Component_Model_RockWind)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BossSwordTrail"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3924/", "SwordTrail.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_Component_Model_BossSwordTrail)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BossEnergySwordTrail"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3924/", "EnergySwordTrail.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_Component_Model_BossEnergySwordTrail)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_EndPhase"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3924/", "EndPhase.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_Component_Model_EndPhase)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_BossTornado"), CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Texture/Effect/Monster/3924/", "BossTornado.fbx", XMMatrixIdentity()))))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_Component_Model_BossTornado)");
		return E_FAIL;
	}


	// 게임오브젝트
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PoleMonarch"), CPoleMonarch::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(GameObject_PoleMonarch)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwordPower"), CSwordPower::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(GameObject_SwordPower)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwordPowerTrail"), CSwordPowerTrail::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_GameObject_SwordPowerTrail)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossCannon"), CBossCannon::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(GameObject_BossCannon)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossCannonBullet"), CBossCannonBullet::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(GameObject_BossCannonBullet)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossGun"), CBossGun::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(GameObject_BossGun)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossGunBullet"), CBossGunBullet::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(GameObject_BossGunBullet)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RevolutionRock"), CRevolutionRock::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(GameObject_RevolutionRock)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossBlinkSmoke"), CBossBlinkSmoke::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_GameObject_BossBlinkSmoke)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RockWind"), CRockWind::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_GameObject_RockWind)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BreakRock"), CBreakRock::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_GameObject_BreakRock)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RockWarning"), CRockWarning::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_GameObject_RockWarning)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndPhase"), CEndPhase::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_GameObject_EndPhase)");
		return E_FAIL;
	}
	if (FAILED(g_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossTornado"), CBossTornado::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("pGameInstance->Add_Prototype returned E_FAIL in CLoader::Loading_ForStage4_2(Prototype_GameObject_BossTornado)");
		return E_FAIL;
	}


	m_isFinished = true;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eLevel)))
	{
		MSGBOX("Failed to Creating CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteObject(m_hThread);

	DeleteCriticalSection(&m_CS);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);


}
