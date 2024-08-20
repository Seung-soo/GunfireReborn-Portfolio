#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CMonster;
class CMonsterWave final : public CBase
{
	MAKE_SINGLETON(CMonsterWave)
public:
	enum STAGE { STAGE3_1_1, STAGE3_1_2, STAGE3_1_3, STAGE4_1_1, STAGE4_1_2, STAGE4_1_3, STAGE_END };
private:
	CMonsterWave();
	virtual ~CMonsterWave() = default;
public:
	list<CMonster*>* Get_MonstersList();
	const size_t	 Get_MonsterSize() const { return m_Monsters.size(); }
public:
	void	Update();
	void	WaveAppear(STAGE eStage);
	void	ReleaseWall();
private:
	_bool	m_bStage3_1_1 = false;
	_bool	m_bStage3_1_2 = false;
	_bool	m_bStage3_1_3 = false;
	_bool	m_bStage4_1_1 = false;
	_bool	m_bStage4_1_2 = false;
	_bool	m_bStage4_1_3 = false;
private:
	list<CMonster*>	m_Monsters;
private:
	HRESULT	Add_Monster(LEVEL eLevel, _uint iCellIndex, const _tchar* pLayerTag);

	void	Wave_Stage3_1_1();
	void	Wave_Stage3_1_2();
	void	Wave_Stage3_1_3();
	void	Wave_Stage4_1_1();
	void	Wave_Stage4_1_2();
	void	Wave_Stage4_1_3();
public:
	virtual void Free() override;
};

END