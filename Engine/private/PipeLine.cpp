#include "..\public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Set_Transform(TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix)
{

	XMStoreFloat4x4(&m_PipeLineMatrix[eStateType], TransformMatrix);
	return S_OK;
}

_matrix CPipeLine::Get_TransformMatrix(TRANSFORMSTATETYPE eStateType)
{
	return XMLoadFloat4x4(&m_PipeLineMatrix[eStateType]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4(TRANSFORMSTATETYPE eStateType)
{
	return m_PipeLineMatrix[eStateType];
}

_float4x4 CPipeLine::Get_TransformFloat4x4_TP(TRANSFORMSTATETYPE eStateType)
{
	_float4x4 TransposeMatrix;
	XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_PipeLineMatrix[eStateType])));
	return TransposeMatrix;
}

_vector CPipeLine::Get_CamPosition() const
{
	return XMLoadFloat4(&m_vCamPosition);
}

ContainmentType CPipeLine::Frustum(_vector vPosition, _float fRadius)
{
	_vector vFrustumPoint[8] = {
		XMVectorSet(-1.f, 1.f, -0.f, 1.f),
		XMVectorSet(1.f, 1.f, -0.f, 1.f),
		XMVectorSet(1.f, -1.f, 0.f, 1.f),
		XMVectorSet(-1.f, -1.f, 0.f, 1.f),
		XMVectorSet(-1.f, 1.f, 1.f, 1.f),
		XMVectorSet(1.f, 1.f, 1.f, 1.f),
		XMVectorSet(1.f, -1.f, 1.f, 1.f),
		XMVectorSet(-1.f, -1.f, 1.f, 1.f),
	};
	_matrix vProjInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PipeLineMatrix[D3DTS_PROJ]));
	_matrix vViewInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PipeLineMatrix[D3DTS_VIEW]));

	_matrix vGoWorld = vProjInverse * vViewInverse;

	for (_int i = 0; i < 8; i++)
	{
		vFrustumPoint[i] = XMVector3TransformCoord(vFrustumPoint[i], vGoWorld);
	}
	_vector vPlane[6] = {
		XMPlaneFromPoints(vFrustumPoint[6], vFrustumPoint[2], vFrustumPoint[1]), // right
		XMPlaneFromPoints(vFrustumPoint[0], vFrustumPoint[3], vFrustumPoint[7]), // left
		XMPlaneFromPoints(vFrustumPoint[5], vFrustumPoint[1], vFrustumPoint[0]), // top
		XMPlaneFromPoints(vFrustumPoint[3], vFrustumPoint[2], vFrustumPoint[6]), // bottom
		XMPlaneFromPoints(vFrustumPoint[0], vFrustumPoint[1], vFrustumPoint[2]), // near
		XMPlaneFromPoints(vFrustumPoint[6], vFrustumPoint[5], vFrustumPoint[4]), // far
	};

	_float3 vPos;
	XMStoreFloat3(&vPos, vPosition);
	BoundingSphere tSphere(vPos, fRadius);

	return tSphere.ContainedBy(vPlane[0], vPlane[1], vPlane[2], vPlane[3], vPlane[4], vPlane[5]);
}

void CPipeLine::Update()
{
	_matrix	ViewMatrix = XMLoadFloat4x4(&m_PipeLineMatrix[D3DTS_VIEW]);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	XMStoreFloat4(&m_vCamPosition, ViewMatrix.r[3]);
}

void CPipeLine::Free()
{
}
