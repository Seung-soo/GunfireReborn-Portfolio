#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Stage3_1 final : public CLevel
{
public:
	explicit CLevel_Stage3_1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Stage3_1() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int Tick(_double dTimeDelta);
	virtual _int LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	HRESULT Ready_Light();
private:
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Meshes(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
public:
	static CLevel_Stage3_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END