#pragma once

#include "Component.h"


BEGIN(Engine)
class CGameObject;
class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
	enum BOUNDING { BOUNDING_ORIGINAL, BOUNDING_TRANSFORM, BOUNDING_END };
public:
	typedef struct tagColliderDesc
	{
		_float3 vScale;
		_float4 vRotation;
		_float4 vTranslation;
	}COLLIDERDESC;
private:
	explicit CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;
public:
	void					Set_Parent(CGameObject* pParent) { m_pParent = pParent; }
	void					Set_ColliderDesc(COLLIDERDESC tColliderDesc);
	void					Set_ColliderTranslation(_fvector vPosition);
	void					Set_IsCollision(_bool bIsCollision) { m_isCollision = bIsCollision; }
	BoundingBox*			Get_AABB() { return m_pAABB[BOUNDING_TRANSFORM]; }
	BoundingOrientedBox*	Get_OBB() { return m_pOBB[BOUNDING_TRANSFORM]; }
	BoundingSphere*			Get_Sphere() { return m_pSphere[BOUNDING_TRANSFORM]; }
	CGameObject*			Get_Parent();
public:
	virtual HRESULT NativeConstruct_Prototype(TYPE eType);
	virtual HRESULT NativeConstruct(void* pArg) override;

	void			Update_Transform(_fmatrix TransformMatrix);
#ifdef _DEBUG
public:
	virtual HRESULT	Render() override;
private:
	// 정점들을 그릴 수 있는 셰이더를 제공한다.
	BasicEffect*									m_pBasicEffect = nullptr;
	// 기본적인 도형을 그려 나갈 수 있는 정점 버퍼, 인덱스 버퍼를 제공한다.
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = nullptr;

	ID3D11InputLayout*								m_pInputLayout = nullptr;
#endif
private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);

private:
	BoundingBox*			m_pAABB[BOUNDING_END] = { nullptr };
	BoundingOrientedBox*	m_pOBB[BOUNDING_END] = { nullptr };
	BoundingSphere*			m_pSphere[BOUNDING_END] = { nullptr };
	TYPE					m_eType = TYPE_END;
	COLLIDERDESC			m_ColliderDesc;
	_bool					m_isCollision = false;
private:
	CGameObject*			m_pParent = nullptr;
public:
	static	CCollider*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType);
	virtual CComponent*	Clone(void* pArg) override;
	virtual void		Free() override;
};

END