#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_MainManu final : public CLevel
{
public:
	explicit CLevel_MainManu(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_MainManu() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
private:
	class CCursor*	m_pCursor = nullptr;
public:
	static CLevel_MainManu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END