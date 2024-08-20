#pragma once

#include "Base.h"


BEGIN(Engine)
class CComponent;

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;
public:
	CComponent*					Get_Component(const _tchar* pComponentTag, _uint iIndex);
	list<class CGameObject*>*	Get_Object() { return &m_Objects; }		// ������Ʈ ����Ʈ ��ȯ
	list<class CGameObject*>*	Get_ObjectPool() { return &m_ObjectPool; }		// ������Ʈ ����Ʈ ��ȯ
public:
	HRESULT	Add_GameObject(class CGameObject* pGameObject);
	HRESULT ThrowToLayer();
	HRESULT Add_ObjectPool(class CGameObject* pGameObject);
	void	FrontChange();

	_int			Tick(_double dTimeDelta);
	_int			LateTick(_double dTimeDelta);
private:
	list<class CGameObject*>			m_Objects;
	typedef list<class CGameObject*>	OBJECTS;

	list<class CGameObject*>			m_ObjectPool;
	typedef list<class CGameObject*>	OBJECTPOOL;
public:
	static CLayer*	Create();
	virtual void	Free() override;
};

END