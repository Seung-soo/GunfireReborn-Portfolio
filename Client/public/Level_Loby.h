#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loby final : public CLevel
{
public:
	explicit CLevel_Loby(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Loby() = default;
public:
	CRITICAL_SECTION Get_CS() {
		return m_CS;
	}
public:
	virtual HRESULT NativeConstruct();
	virtual _int Tick(_double dTimeDelta);
	virtual _int LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	HRESULT Ready_Light();
private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_CS;
	_bool				m_isFinished = false;

private:
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Ocean(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player_Hand(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Meshes(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Sky(const _tchar* pLayerTag);

public:
	static CLevel_Loby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END