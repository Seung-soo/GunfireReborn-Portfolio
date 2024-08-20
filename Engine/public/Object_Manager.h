#pragma once

#include "Base.h"
#include "GameObject.h"
#include "Layer.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	MAKE_SINGLETON(CObject_Manager)
public:
	CObject_Manager();
	virtual ~CObject_Manager() = default;
public:
	CComponent*	Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);
	size_t		Get_Size(_uint iLevelIndex, const _tchar* pLayerTag) { return Find_Layer(iLevelIndex, pLayerTag)->Get_Object()->size(); }
	CGameObject* Get_Front(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject* Get_Back(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject* Get_ObjectPoolBack(_uint iLevelIndex, const _tchar* pLayerTag);
	list<CGameObject*>*	Get_ObjectList(_uint iLevelIndex, const _tchar* pLayerTag);
public:
	HRESULT Reserve_Container(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg);
	HRESULT ThrowToLayer(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT Add_ObjectPool(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg);
	HRESULT	Clear_LevelObject(_uint iLevelIndex);
	HRESULT	Release_LayerObject(_uint iLevelIndex, const _tchar* pLayerTag);

	HRESULT FrontChange(_uint iLevelIndex, const _tchar* pLayerTag);
public:
	_int	Tick(_double dTimeDelta);
	_int	LateTick(_double dTimeDelta);
private:
	map<const _tchar*, CGameObject*>			m_Prototypes;
	typedef map<const _tchar*, CGameObject*>	PROTOTYPES;
private:
	map<const _tchar*, CLayer*>*				m_pLayers;
	typedef map<const _tchar*, CLayer*>			LAYERS;
private:
	_uint			m_iNumLevels = 0;
public:
	CGameObject*			Find_Prototype(const _tchar* pPrototypeTag);
	CLayer*					Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
public:
	virtual void	Free() override;
};

END