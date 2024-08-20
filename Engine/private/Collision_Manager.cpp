#include "..\public\Collision_Manager.h"
#include "GameInstance.h"
IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

_bool CCollision_Manager::Collision_AABB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == pSrcCollider || nullptr == pDestCollider)
	{
		MSGBOX("nullptr == pSrcCollider || nullptr == pDestCollider in CCollision_Manager::Collision_AABB");
		return false;
	}

	if (true == pSrcCollider->Get_AABB()->Intersects(*pDestCollider->Get_AABB()))
		return true;

	return false;
}

_bool CCollision_Manager::Collision_OBB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == pSrcCollider || nullptr == pDestCollider)
	{
		MSGBOX("nullptr == pSrcCollider || nullptr == pDestCollider in CCollision_Manager::Collision_OBB");
		return false;
	}

	if (nullptr != pDestCollider->Get_AABB())
	{
		if (true == pSrcCollider->Get_OBB()->Intersects(*pDestCollider->Get_AABB()))
			return true;
	}
	else
	{
		if (true == pSrcCollider->Get_OBB()->Intersects(*pDestCollider->Get_OBB()))
			return true;
	}

	return false;
}

_bool CCollision_Manager::Collision_Sphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == pSrcCollider || nullptr == pDestCollider)
	{
		MSGBOX("nullptr == pSrcCollider || nullptr == pDestCollider in CCollision_Manager::Collision_Sphere");
		return false;
	}

	if (true == pSrcCollider->Get_Sphere()->Intersects(*pDestCollider->Get_Sphere()))
		return true;

	return false;
}

_bool CCollision_Manager::Collision_RayToSphere(_fvector vStartPosition, _fvector vDir, CCollider * pDestCollider, _float* pDist)
{
	if (nullptr == pDestCollider)
	{
		MSGBOX("nullptr == pDestCollider in CCollision_Manager::Collision_RayToSphere");
		return false;
	}

	_bool bCollision = pDestCollider->Get_Sphere()->Intersects(vStartPosition, vDir, *pDist);

	pDestCollider->Set_IsCollision(bCollision);

	return bCollision;
}

_bool CCollision_Manager::Collision_AABBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == pSrcCollider || nullptr == pDestCollider)
	{
		MSGBOX("nullptr == pSrcCollider || nullptr == pDestCollider in CCollision_Manager::Collision_AABBToSphere");
		return false;
	}

	if (true == pSrcCollider->Get_AABB()->Intersects(*pDestCollider->Get_Sphere()))
		return true;

	return false;
}

_bool CCollision_Manager::Collision_OBBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == pSrcCollider || nullptr == pDestCollider)
	{
		MSGBOX("nullptr == pSrcCollider || nullptr == pDestCollider in CCollision_Manager::Collision_OBBToSphere");
		return false;
	}

	if (true == pSrcCollider->Get_OBB()->Intersects(*pDestCollider->Get_Sphere()))
		return true;

	return false;
}

HRESULT CCollision_Manager::TargetList_Sort()
{
	//if (false == m_TargetList[TARGET_ENEMY].empty())
	//	for (auto& iter : m_TargetList[TARGET_ENEMY])
	//		iter.second.sort(ColliderSort<CCollider*>);

	return S_OK;
}

list<CCollider*>* CCollision_Manager::Get_ColliderList(const _tchar * pColliderTag, TARGET eTarget)
{
	auto iter = find_if(m_TargetList[eTarget].begin(), m_TargetList[eTarget].end(), CTagFinder(pColliderTag));

	if (iter == m_TargetList[eTarget].end())
		return nullptr;

	return &(iter->second);
}

HRESULT CCollision_Manager::Add_Collider(const _tchar * pColliderTag, TARGET eTarget, CCollider* pCollider)
{
	if (nullptr == pCollider)
	{
		MSGBOX("nullptr == pCollider in CCollision_Manager::Add_Collider");
		return E_FAIL;
	}

	COLLIDERLIST* pColliderList = Get_ColliderList(pColliderTag, eTarget);
	if (nullptr == pColliderList)
	{
		COLLIDERLIST ColliderList;
		ColliderList.push_back(pCollider);

		m_TargetList[eTarget].emplace(pColliderTag, ColliderList);
	}
	else
		pColliderList->push_back(pCollider);

	return S_OK;
}

void CCollision_Manager::Clear_Collider()
{
	for (_uint i = 0; i < TARGET_END; i++)
	{
		for (auto& TargetList : m_TargetList[i])
			TargetList.second.clear();
		m_TargetList[i].clear();
	}
}

void CCollision_Manager::Free()
{
}
