#include "..\public\Collider.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "DebugDraw.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
#ifdef _DEBUG
	, m_pBasicEffect(rhs.m_pBasicEffect)
	, m_pBatch(rhs.m_pBatch)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
	, m_eType(rhs.m_eType)
{
	for (_uint i = 0; i < BOUNDING_END; i++)
	{
		if (nullptr != rhs.m_pAABB[BOUNDING_ORIGINAL])
			m_pAABB[i] = new BoundingBox(*rhs.m_pAABB[BOUNDING_ORIGINAL]);

		if (nullptr != rhs.m_pOBB[BOUNDING_ORIGINAL])
			m_pOBB[i] = new BoundingOrientedBox(*rhs.m_pOBB[BOUNDING_ORIGINAL]);

		if (nullptr != rhs.m_pSphere[BOUNDING_ORIGINAL])
			m_pSphere[i] = new BoundingSphere(*rhs.m_pSphere[BOUNDING_ORIGINAL]);
	}
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif

}

void CCollider::Set_ColliderDesc(COLLIDERDESC tColliderDesc)
{
	//memcpy(&m_ColliderDesc, &tColliderDesc, sizeof(COLLIDERDESC));

	//m_pSphere[BOUNDING_ORIGINAL]->Radius = m_ColliderDesc.vScale.x * 0.5f;
	_matrix TransformMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&tColliderDesc.vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&m_ColliderDesc.vRotation), XMLoadFloat4(&m_ColliderDesc.vTranslation));
	//if (nullptr != m_pAABB[BOUNDING_ORIGINAL])
	//	m_pAABB[BOUNDING_ORIGINAL]->Transform(*m_pAABB[BOUNDING_ORIGINAL], TransformMatrix);
	//if (nullptr != m_pOBB[BOUNDING_ORIGINAL])
	//	m_pOBB[BOUNDING_ORIGINAL]->Transform(*m_pOBB[BOUNDING_ORIGINAL], TransformMatrix);
	if (nullptr != m_pSphere[BOUNDING_ORIGINAL])
		m_pSphere[BOUNDING_ORIGINAL]->Transform(*m_pSphere[BOUNDING_ORIGINAL], TransformMatrix);
}

void CCollider::Set_ColliderTranslation(_fvector vPosition)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, vPosition);
	if (nullptr != m_pSphere[BOUNDING_ORIGINAL])
		m_pSphere[BOUNDING_ORIGINAL]->Center = vPos;
}

CGameObject * CCollider::Get_Parent()
{
	return m_pParent;
}

HRESULT CCollider::NativeConstruct_Prototype(TYPE eType)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CCollider::NativeConstruct_Prototype");
		return E_FAIL;
	}

	m_eType = eType;

	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB[BOUNDING_ORIGINAL] = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		break;
	case TYPE_OBB:
		m_pOBB[BOUNDING_ORIGINAL] = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		break;
	case TYPE_SPHERE:
		m_pSphere[BOUNDING_ORIGINAL] = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		break;
	}

#ifdef _DEBUG
	m_pBasicEffect = new BasicEffect(m_pDevice);
	m_pBasicEffect->SetVertexColorEnabled(true);	// 색을 받아오기 위한 셰이더를 사용할 것이다.

	const void*	pShaderByteCode = nullptr;
	size_t		iShaderByteCodeLength = 0;

	m_pBasicEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
	{
		MSGBOX("m_pDevice->CreateInputLayout returned E_FAIL in CCollider::NativeConstruct_Prototype");
		return E_FAIL;
	}

	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pDeviceContext);

#endif


	return S_OK;
}

HRESULT CCollider::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CCollider::NativeConstruct");
		return E_FAIL;
	}

	_matrix		TransformMatrix;

	if (nullptr != pArg)
	{
		memcpy(&m_ColliderDesc, pArg, sizeof(COLLIDERDESC));
		TransformMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&m_ColliderDesc.vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&m_ColliderDesc.vRotation), XMLoadFloat4(&m_ColliderDesc.vTranslation));
	}
	else
		TransformMatrix = XMMatrixIdentity();

	if (nullptr != m_pAABB[BOUNDING_ORIGINAL])
		m_pAABB[BOUNDING_ORIGINAL]->Transform(*m_pAABB[BOUNDING_ORIGINAL], TransformMatrix);
	if (nullptr != m_pOBB[BOUNDING_ORIGINAL])
		m_pOBB[BOUNDING_ORIGINAL]->Transform(*m_pOBB[BOUNDING_ORIGINAL], TransformMatrix);
	if (nullptr != m_pSphere[BOUNDING_ORIGINAL])
		m_pSphere[BOUNDING_ORIGINAL]->Transform(*m_pSphere[BOUNDING_ORIGINAL], TransformMatrix);

	return S_OK;
}

void CCollider::Update_Transform(_fmatrix TransformMatrix)
{
	if (nullptr != m_pAABB[BOUNDING_ORIGINAL])
		m_pAABB[BOUNDING_ORIGINAL]->Transform(*m_pAABB[BOUNDING_TRANSFORM], Remove_Rotation(TransformMatrix));

	if (nullptr != m_pOBB[BOUNDING_ORIGINAL])
		m_pOBB[BOUNDING_ORIGINAL]->Transform(*m_pOBB[BOUNDING_TRANSFORM], TransformMatrix);

	if (nullptr != m_pSphere[BOUNDING_ORIGINAL])
		m_pSphere[BOUNDING_ORIGINAL]->Transform(*m_pSphere[BOUNDING_TRANSFORM], TransformMatrix);
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pBasicEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pBasicEffect->SetView(XMLoadFloat4x4(&pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	m_pBasicEffect->SetProjection(XMLoadFloat4x4(&pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	RELEASE_INSTANCE(CPipeLine);

	m_pBasicEffect->Apply(m_pDeviceContext);


	_vector			vColliderColor = m_isCollision == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f);

	m_pBatch->Begin();

	if (nullptr != m_pAABB[BOUNDING_TRANSFORM])
		DX::Draw(m_pBatch, *m_pAABB[BOUNDING_TRANSFORM], vColliderColor);
	if (nullptr != m_pOBB[BOUNDING_TRANSFORM])
		DX::Draw(m_pBatch, *m_pOBB[BOUNDING_TRANSFORM], vColliderColor);
	if (nullptr != m_pSphere[BOUNDING_TRANSFORM])
		DX::Draw(m_pBatch, *m_pSphere[BOUNDING_TRANSFORM], vColliderColor);

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		Matrix = TransformMatrix;

	Matrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	Matrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	Matrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return Matrix;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eType)
{
	CCollider*	pInstance = new CCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType)))
	{
		MSGBOX("Failed to Creating CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*	pInstance = new CCollider(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	for (_uint i = 0; i < BOUNDING_END; i++)
	{
		Safe_Delete(m_pAABB[i]);
		Safe_Delete(m_pOBB[i]);
		Safe_Delete(m_pSphere[i]);
	}

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBasicEffect);
		Safe_Delete(m_pBatch);
	}
#endif
}
