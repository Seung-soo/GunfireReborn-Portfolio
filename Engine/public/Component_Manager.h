#pragma once

#include "Base.h"
#include "Renderer.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_RectXZ.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Triangle.h"
#include "VIBuffer_Point_Instance.h"
#include "Transform.h"
#include "Model.h"
#include "Collider.h"
#include "Navigation.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	MAKE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;
public:
	HRESULT			Reserve_Container(_uint iNumLevels);
	HRESULT			Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent*		Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	HRESULT			Clear_LevelComponent(_uint iLevelIndex);
public:
	CComponent*		Find_Component(_uint iLevelIndex, const _tchar* pPrototypeTag);
private:
	map<const _tchar*, CComponent*>*		m_pPrototypes = nullptr;
	typedef map<const _tchar*, CComponent*>	PROTOTYPES;
private:
	_uint			m_iNumLevels = 0;
public:
	virtual void	Free() override;
};

END