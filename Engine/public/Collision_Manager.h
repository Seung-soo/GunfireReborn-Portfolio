#pragma once

#include "Collider.h"

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
	MAKE_SINGLETON(CCollision_Manager)
public:
	enum TARGET { TARGET_PLAYER, TARGET_ENEMY, TARGET_OBJECT, TARGET_END };
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;
public:
	list<CCollider*>*	Get_ColliderList(const _tchar* pColliderTag, TARGET eTarget);
public:
	HRESULT				Add_Collider(const _tchar* pColliderTag, TARGET eTarget, CCollider* pCollider);
	void				Clear_Collider();
public:
	_bool				Collision_AABB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_OBB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_RayToSphere(_fvector vStartPosition, _fvector vDir, CCollider* pDestCollider, _float* pDist);
	_bool				Collision_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
public:
	HRESULT				TargetList_Sort();
private:
	typedef list<CCollider*>			COLLIDERLIST;
	map <const _tchar*, COLLIDERLIST>	m_TargetList[TARGET_END];
public:
	virtual void	Free() override;
};

END