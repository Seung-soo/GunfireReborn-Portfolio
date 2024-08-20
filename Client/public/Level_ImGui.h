#pragma once

#include "Client_Defines.h"
#include "Level.h"
#ifdef USE_IMGUI
BEGIN(Client)

#ifdef USE_IMGUI
class CImGui_Manager;
#endif

class CLevel_ImGui final : public CLevel
{
public:
	explicit CLevel_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_ImGui() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int Tick(_double dTimeDelta);
	virtual _int LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
#ifdef USE_IMGUI
	CImGui_Manager*			m_pImGui_Manager = nullptr;
#endif
	vector<_tchar*>			m_vPrototypeKey;
private:
	HRESULT Ready_Light();
private:
	HRESULT Ready_Component();
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Sky(const _tchar* pLayerTag);
	HRESULT Load_Models();
	HRESULT Load_AnimModels();
public:
	static CLevel_ImGui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void		 Free() override;
};

END
#endif