#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "HpNumber.h"

BEGIN(Client)

class CNumberUI final : public CGameObject
{
public:
	enum NUMBERTYPE { TYPE_SHIELD, TYPE_HP, TYPE_WHITEBULLET, TYPE_GREENBULLET, TYPE_BLUEBULLET, TYPE_YELLOWBULLET, TYPE_COIN, TYPE_SKILL, TYPE_COOLTIME, TYPE_END };

	typedef struct tagNumberDesc
	{
		NUMBERTYPE	eNumberType = TYPE_END;
		_float		fX = 640.f;
		_float		fY = 360.f;
		_int*		pCurrent = nullptr;
		_int*		pMax = nullptr;
		_bool		bStopUI = false;
	}NUMBERDESC;
private:
	explicit CNumberUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNumberUI(const CNumberUI& rhs);
	virtual ~CNumberUI() = default;
public:
	void			Set_BulletNumberDesc(NUMBERDESC tNumberDesc);
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	NUMBERDESC			m_tNumberDesc;
	vector<CHpNumber*>	m_Numbers;

	_int				m_iOldCur = 0;
	_int				m_iOldMax = 0;
private:
	HRESULT			Push_Number(_float fX, _float fY, _float fSizeX, _float fSizeY, CHpNumber::NUMCOLOR eNumColor, _tchar* pLayerTag);
	HRESULT			Make_Number();

	void			Number_Shield();
	void			Number_Hp();
	void			Number_WhiteBullet();
	void			Number_GreenBullet();
	void			Number_BlueBullet();
	void			Number_YellowBullet();
	void			Number_Coin();
	void			Number_Skill();
	void			Number_CoolTime();
public:
	static	CNumberUI*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END