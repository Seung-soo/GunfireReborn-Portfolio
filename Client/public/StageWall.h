#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CNavigation;
class CCell;
END

BEGIN(Client)

class CStageWall final : public CGameObject
{
public:
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CStageWall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CStageWall(const CStageWall& rhs);
	virtual ~CStageWall() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
public:
	HRESULT			WallSetting(_uint iCellIndex, LINE eLine, _bool bPortal);
	HRESULT			Push_Cell(_uint iCellIndex);
	HRESULT			Restore();
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pSourTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	CNavigation*		m_pPlayerNavigation = nullptr;
	_vector				m_vPoint[4] = {};
	_bool				m_bPortal = false;
	_bool				m_bGoNextStage = false;
	CCell*				m_pCell = nullptr;
	vector<CCell*>		m_Cells;
	_float				m_fRot = 0.f;
	class CMiniMap_Icon* m_pMiniMap_Icon = nullptr;
	_bool				m_bMakeUI = false;
	CGameObject*		m_pUI = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_OnTerrain();
private:
	void	Portal();
public:
	static CStageWall*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END