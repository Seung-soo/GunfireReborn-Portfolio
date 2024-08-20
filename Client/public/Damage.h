#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CDamageNumber;
class CDamage final : public CGameObject
{
public:
	enum NUMBERTYPE { TYPE_ORIGIN, TYPE_CRITICAL, TYPE_COMB, TYPE_MIAS, TYPE_END };

	typedef struct tagNumberDesc
	{
		_vector		vPosition;
		_int		iDamage;
		NUMBERTYPE	eType;
	}NUMBERDESC;
private:
	explicit CDamage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDamage(const CDamage& rhs);
	virtual ~CDamage() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	NUMBERDESC				m_tNumberDesc;
	vector<CDamageNumber*>	m_Numbers;
	_float3					m_vColor;
	_vector					m_vMyPosition;
	_vector					m_vMyRight;
	_vector					m_vViewPortPosition;
	_float					m_fZ = 0.f;
private:
	HRESULT			Make_Number();
	void			Damege_10000();
	void			Damege_1000();
	void			Damege_100();
	void			Damege_10();
	void			Damege_1();
public:
	static	CDamage*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END