#pragma once

#include "Base.h"
#include "Client_Defines.h"
#ifdef USE_IMGUI

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Particle_Effect.h"

BEGIN(Engine)
class CCell;
END

BEGIN(Client)
class CTerrain;
class CMesh;
class CImGui_Manager final : public CBase
{
	MAKE_SINGLETON(CImGui_Manager)
public:
	enum TOOL { TOOL_TERRAIN, TOOL_UI, TOOL_END };
private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;
public:
	void	Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void	Tick();
	void	Render();
public:
	INFO_UI*		Get_InfoUI() { return m_pInfoUI; }
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

private:
	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	bool show_tool = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


private:	// Terrain
	vector<GROUPTERRAIN>		m_vGroupTerrain;
	char						m_szTerrainFileName[MAX_PATH] = "";
	char						m_szTerrainChildName[MAX_PATH] = "";
	char						m_szTerrainSaveFileName[64] = "";
	char						m_szTerrainLoadFileName[64] = "";

	INFO_TERRAIN*				m_pInfoTerrain = nullptr;
	_bool						m_bWireFrame = false;
	_bool						m_bBlend = false;

	map<const char*, CTerrain*>	m_Terrains;
	CTerrain*					m_pSelectTerrain = nullptr;
	vector<const char*>			m_vFirst;
	vector<const _tchar*>		m_vLFirst;

	_bool						m_bOnBrush = false;
	_float						m_fBrushRadius = 1.f;

	_bool						m_bDefaultMode = true;
	_int						m_iShapeness = 0;
	_float						m_fMaxY = 0.f;
	_float						m_fMinY = 0.f;

	_bool						m_bSlopeMode = false;
	_float						m_fSlopeStart = 0.f;
	_float						m_fSlopeEnd = 0.f;
	_float						m_fSlopeWeight = 0.f;
	_float						m_fDivisibleValue = 1.f;

	_bool						m_bAngularMode = false;

	_bool						m_bTwoPointSlopeMode = false;
private:
	void		Terrain_Names(_int iTerrainListIndex);
	void		Terrain_Texture_List();
	_int		Terrain_List();
	void		Terrain_Info(_int iIndex);
	_int		Terrain_Load_List();
	CTerrain*	Find_Terrain(const char* pGroupTag, const char* pInfoTag);
	void		UpDown_Terrain();
	void		Update_Terrain();
private:
	void Save_Terrain();
	void Load_Terrain(_int iLoadIndex);


private:	// Animation
	char						m_szAnimationFileName[MAX_PATH] = "";
	char						m_szAnimationModelFileName[MAX_PATH] = "";

	class CImGui_Animation*		m_pAnimationModel;
	INFO_COLLIDER				m_tColliderInfo = {};

	vector<_tchar*>				m_vModelName;
	vector<char*>				m_vAnimationName;
	vector<char*>				m_vBoneName;

	_float						m_fColliderRadius = 1.f;
	_bool						m_bAnimationWireFrame = false;
	_bool						m_bAnimationBlend = false;

	_uint						m_iAnimationSelect = 0;
	_uint						m_iBoneSelect = 0;
	_float						m_fColliderLocalPos[3] = {};

	_float						m_fAnimModelScale[3] = { 1.f, 1.f, 1.f };
	_float						m_fAnimModelRotation[3] = {};
	_float						m_fAnimModelPosition[3] = {};
private:
	void		Collider_Create();
	void		Model_List();
	void		Animation_List();
	void		Collider_BoneList();
	void		AnimModelTransform();



private:	// Mesh
	char						m_szMeshFileName[MAX_PATH] = "";
	char						m_szModelFileName[MAX_PATH] = "";
	char						m_szMeshSaveFileName[64] = "";
	char						m_szMeshLoadFileName[64] = "";
	INFO_MESH*					m_pCurrentInfoMesh = nullptr;

	vector<INFO_MESH*>			m_pMeshesInfo;
	map<const char*, CMesh*>	m_Meshs;
	CMesh*						m_pSelectMesh = nullptr;
	vector<const char*>			m_vMeshesFirst;

	_bool						m_bMeshWireFrame = false;
	_bool						m_bMeshBlend = false;
private:
	void		Mesh_Names(_int iMeshListIndex);
	void		Mesh_Model_List();
	_int		Mesh_List();
	void		Mesh_Info(_int iIndex);
	_int		Mesh_Load_List();
	CMesh*		Find_Mesh(const char* pInfoTag);

private:
	void Save_Mesh();
	void Load_Mesh(_int iLoadIndex);


private:	// UI
	vector<GROUPUI>		m_vGroupUI;
	char				m_szUIFileName[MAX_PATH] = "";
	char				m_szUIChildName[MAX_PATH] = "";
	char				m_szSaveFileName[64] = "";
	char				m_szLoadFileName[64] = "";
	INFO_UI*			m_pInfoUI = nullptr;
	_bool				m_bUIStart = true;
	_int				m_iUiTexNum = 0;
private:
	void UI_Names(_int iUIListIndex);
	void UI_Texture_List();
	_int UI_List();
	void UI_Info(_int iIndex);
	_int UI_Load_List();
private:
	void Save_UI();
	void Load_UI(_int iLoadIndex);

private:
	_bool					m_bMakeCell_Mode = false;
	_bool					m_bTypeSetting_Mode = false;
	_bool					m_bTypeChange_Mode = false;
	_float					m_Points[3][3] = {};
	_uint					m_iPointIndex = 0;
	class CCellPoint*		m_CellPoint[3];
	vector<CCell*>			m_Cells;
	_bool					m_bMakeTestPoint = false;
	_bool					m_bFirstSetting = false;
	_int					m_iCurrentCellIndex = -1;

	_bool					m_bPointA = false;
	_bool					m_bPointB = false;
	_bool					m_bPointC = false;
	char					m_szNaviDataFilePath[MAX_PATH] = "";

	char					m_szSaveNaviDataFileName[MAX_PATH] = "";

	_bool					m_bNaviMeshRender = true;
#ifdef _DEBUG
private:
	class CVIBuffer_Triangle*		m_pVIBuffer = nullptr;
	class CShader*					m_pShader = nullptr;
#endif
private:
	void		NaviMesh_Make();
	_int		NaviMesh_List();
	_int		NaviMesh_Load_List();

private:
	void Save_NaviMesh();
	void Load_NaviMesh();

	//Particle
private:
	void Particle_Info();
private:
	CParticle_Effect::PARTICLEDESC	m_tParticleDesc;
	_float4							m_vParticlePosition = _float4(0.f, 0.f, 0.f, 1.f);
public:
	virtual void Free() override;
};


END
#endif
