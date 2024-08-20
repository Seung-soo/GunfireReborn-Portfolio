#include "..\public\Camera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::NativeConstruct_Prototype()
{


	return S_OK;
}

HRESULT CCamera::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	if (FAILED(__super::NativeConstruct(&m_CameraDesc.TransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CCamera::NativeConstruct");
		return E_FAIL;
	}
	_vector		vLook = XMLoadFloat3(&m_CameraDesc.vAt) - XMLoadFloat3(&m_CameraDesc.vEye);
	vLook = XMVector3Normalize(vLook);

	_vector		vRight = XMVector3Cross(XMLoadFloat3(&m_CameraDesc.vAxisY), vLook);
	vRight = XMVector3Normalize(vRight);

	_vector		vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMLoadFloat4(&_float4(m_CameraDesc.vEye, 1.f)));

	return S_OK;
}

_int CCamera::Tick(_double dTimeDelta)
{
	if (nullptr == m_pDevice ||
		nullptr == m_pDeviceContext)
	{
		MSGBOX("m_pDevice or m_pDeviceContext is nullptr in CCamera::Tick");
		return -1;
	}

	if (FAILED(m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInverse())))
	{
		MSGBOX("m_pPipeLine->Set_Transform returned E_FAIL in CCamera::Tick(View)");
		return -1;
	}

	_matrix		ProjMatrix = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);

	if (FAILED(m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix)))
	{
		MSGBOX("m_pPipeLine->Set_Transform returned E_FAIL in CCamera::Tick(Proj)");
		return -1;
	}

	return _int();
}

_int CCamera::LateTick(_double dTimeDelta)
{
	return _int();
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
}
