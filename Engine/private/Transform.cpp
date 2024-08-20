#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_vector CTransform::Get_Scale(STATE eState) const
{
	return XMVector3Length(XMLoadFloat3((_float3*)&m_WorldMatrix.m[eState][0]));
}

_vector CTransform::Get_AllScale() const
{
	return XMVectorSet(XMVectorGetX(Get_Scale(STATE_RIGHT)), XMVectorGetX(Get_Scale(STATE_UP)), XMVectorGetX(Get_Scale(STATE_LOOK)), 0.f);
}

_vector CTransform::Get_State(STATE eState) const
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
}

_matrix CTransform::Get_WorldMatrix() const
{
	return XMLoadFloat4x4(&m_WorldMatrix);
}

_matrix CTransform::Get_WorldMatrixInverse() const
{
	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
}

_matrix CTransform::Get_OldWorldMatrix() const
{
	return XMLoadFloat4x4(&m_OldWorldMatrix);
}

_matrix CTransform::Get_OldWorldMatrixInverse() const
{
	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_OldWorldMatrix));
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], vState);
}

HRESULT CTransform::NativeConstruct_Prototype()
{
	//if (FAILED(__super::NativeConstruct_Prototype()))
	//{
	//	MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CTransform::NativeConstruct_Prototype");
	//	return E_FAIL;
	//}
	//XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OldWorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CTransform::NativeConstruct");
		return E_FAIL;
	}

	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));



	return S_OK;
}

HRESULT CTransform::Bind_OnShader(CShader * pShader, const char * pValueName)
{
	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	m_OldWorldMatrix = m_WorldMatrix;
	return pShader->Set_RawValue(pValueName, &WorldMatrix, sizeof(_float4x4));
}

HRESULT CTransform::Bind_OnOldShader(CShader * pShader, const char * pValueName)
{
	_float4x4		OldWorldMatrix;

	XMStoreFloat4x4(&OldWorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_OldWorldMatrix)));

	return pShader->Set_RawValue(pValueName, &OldWorldMatrix, sizeof(_float4x4));
}

HRESULT CTransform::Go_Straight(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	_float3*	pSlidingVector = nullptr;

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (nullptr != pSlidingVector)
	{
		
		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = XMVector4Normalize(-Get_State(STATE_LOOK));
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));

		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta , vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta);
		}
	}
	return S_OK;
}

HRESULT CTransform::Go_Right(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;
	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	_float3*	pSlidingVector = nullptr;

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (nullptr != pSlidingVector)
	{

		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = XMVector4Normalize(-Get_State(STATE_RIGHT));
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));
		//vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta, vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTransform::Go_Left(_double TimeDelta, CNavigation* pNavigation)
{

	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	_float3*	pSlidingVector = nullptr;

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (nullptr != pSlidingVector)
	{

		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = XMVector4Normalize(Get_State(STATE_RIGHT));
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));
		//vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta, vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTransform::Go_Backward(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	_float3*	pSlidingVector = nullptr;

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (nullptr != pSlidingVector)
	{

		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = XMVector4Normalize(Get_State(STATE_LOOK));
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));
		//vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta, vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTransform::Go_FrontLeft(_double TimeDelta, CNavigation * pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vDirection = XMVector4Normalize(vLook - vRight);

	vPosition += vDirection * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	_float3*	pSlidingVector = nullptr;

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (nullptr != pSlidingVector)
	{

		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = -vDirection;
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));

		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta, vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTransform::Go_FrontRight(_double TimeDelta, CNavigation * pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vDirection = XMVector4Normalize(vLook + vRight);

	vPosition += vDirection * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	_float3*	pSlidingVector = nullptr;

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (nullptr != pSlidingVector)
	{

		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = -vDirection;
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));

		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta, vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTransform::Go_BackLeft(_double TimeDelta, CNavigation * pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vDirection = XMVector4Normalize(-vLook - vRight);

	vPosition += vDirection * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	_float3*	pSlidingVector = nullptr;

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (nullptr != pSlidingVector)
	{

		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = -vDirection;
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));

		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta, vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTransform::Go_BackRight(_double TimeDelta, CNavigation * pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vDirection = XMVector4Normalize(-vLook + vRight);

	vPosition += vDirection * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	_float3*	pSlidingVector = nullptr;

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (nullptr != pSlidingVector)
	{

		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = -vDirection;
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));

		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta, vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTransform::Go_Up(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vUp = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}

	return S_OK;
}

HRESULT CTransform::Go_Down(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vUp = Get_State(CTransform::STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}

	return S_OK;
}

HRESULT CTransform::Go_Vector(_fvector vDirection, _double dTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;

	vPosition += XMVector3Normalize(vDirection) * m_TransformDesc.fSpeedPerSec * (_float)dTimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	_float3*	pSlidingVector = nullptr;

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (nullptr != pSlidingVector)
	{

		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = XMVector4Normalize(-vDirection);
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));
		//vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)dTimeDelta, vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition += vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)dTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	// vRight = XMVectorSetW(vRight, 0.f);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::CameraTurn(_fvector vAxis, _double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	_float fRadian = XMVectorGetX(XMVector3Dot(vUp, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (0.01f < fRadian)
	{
		Set_State(CTransform::STATE_RIGHT, vRight);
		Set_State(CTransform::STATE_UP, vUp);
		Set_State(CTransform::STATE_LOOK, vLook);
	}

	return S_OK;
}

HRESULT CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT)));
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_UP)));
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_LOOK)));

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::NowRotation(_fvector vAxis, _float fRadian)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::AllRotation(_fvector vRotationNum)
{
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT)));
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_UP)));
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_LOOK)));

	_matrix		RotationMatrix = XMMatrixRotationX(XMVectorGetX(vRotationNum));
	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	RotationMatrix = XMMatrixRotationY(XMVectorGetY(vRotationNum));
	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	RotationMatrix = XMMatrixRotationZ(XMVectorGetZ(vRotationNum));
	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Revolution(_fvector vAxis, _fvector vTarget, _double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPos = Get_State(CTransform::STATE_POSITION);
	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vPos -= vTarget;
	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);
	vPos = XMVector3TransformNormal(vPos, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	vPos += vTarget;
	Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}


HRESULT CTransform::Scaled(_fvector vScale)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);


	vRight = XMVector3Normalize(vRight) * XMVectorGetX(vScale);
	vUp = XMVector3Normalize(vUp) * XMVectorGetY(vScale);
	vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::OneScaled(STATE eState, _float fScale)
{
	_vector vTarget;
	vTarget = Get_State(eState);
	vTarget = XMVector3Normalize(vTarget) * fScale;

	Set_State(eState, vTarget);

	return S_OK;
}

HRESULT CTransform::Scaling(_double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vRight += XMVector3Normalize(vRight) * m_TransformDesc.fScalingPerSec * (_float)TimeDelta;
	vUp += XMVector3Normalize(vUp) * m_TransformDesc.fScalingPerSec * (_float)TimeDelta;
	vLook += XMVector3Normalize(vLook) * m_TransformDesc.fScalingPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::OneScaling(STATE eState, _double TimeDelta)
{
	_vector		vTargetScale = Get_State(eState);

	vTargetScale += XMVector3Normalize(vTargetScale) * m_TransformDesc.fScalingPerSec * (_float)TimeDelta;

	Set_State(eState, vTargetScale);

	return S_OK;
}

HRESULT CTransform::Chase(_fvector vTargetPos, _double TimeDelta)
{
	return S_OK;
}

HRESULT CTransform::LookAt(_fvector vTargetPos)
{
	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector vRight, vUp, vLook;

	vLook = vTargetPos - vPosition;
	vLook = XMVector4Normalize(vLook);

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight = XMVector4Normalize(vRight);

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(CTransform::STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(CTransform::STATE_LOOK, vLook * Get_Scale(STATE_LOOK));

	return S_OK;
}

HRESULT CTransform::LookAtXZ(_fvector vTargetPos)
{
	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector vRight, vUp, vLook;

	vLook = vTargetPos - vPosition;
	vLook = XMVectorSetY(vLook, 0.f);
	vLook = XMVector4Normalize(vLook);

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight = XMVector4Normalize(vRight);

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(CTransform::STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(CTransform::STATE_LOOK, vLook * Get_Scale(STATE_LOOK));

	return S_OK;
}

HRESULT CTransform::MatchIt_XZ(_fvector vTargetPos)
{
	_vector	vPosition = Get_State(CTransform::STATE_POSITION);
	_vector	vScale = Get_Scale(CTransform::STATE_RIGHT);

	_vector vRight, vUp, vLook;


	vLook = vPosition - vTargetPos;
	vLook = XMVectorSetY(vLook, 0.f);
	vLook = XMVector4Normalize(vLook);

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight = XMVector4Normalize(vRight);

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * XMVectorGetX(vScale));
	Set_State(CTransform::STATE_UP, vUp * XMVectorGetY(vScale));
	Set_State(CTransform::STATE_LOOK, vLook * XMVectorGetX(vScale));

	return S_OK;
}

HRESULT CTransform::LookTurn(_fvector vAxis, _fvector vTarget, _double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPos = Get_State(CTransform::STATE_POSITION);

	_vector vDir = vTarget - vPos;
	vDir = XMVector4Normalize(XMVectorSetY(vDir, 0.f));

	_float fAngle = XMConvertToDegrees(acos(XMVectorGetX(XMVector4Dot(vDir, vLook))));

	_float d = -(XMVectorGetX(vRight) * XMVectorGetX(vPos) + XMVectorGetY(vRight) * XMVectorGetY(vPos) + XMVectorGetZ(vRight) * XMVectorGetZ(vPos));
	_float LR = XMVectorGetX(vRight) * XMVectorGetX(vTarget) + XMVectorGetY(vRight) * XMVectorGetY(vTarget) + XMVectorGetZ(vRight) * XMVectorGetZ(vTarget) + d;
	if (0 > LR)
		fAngle = -fAngle;
	fAngle = XMConvertToRadians(fAngle);

	if (5.f > XMConvertToDegrees(fAngle) && -5.f < XMConvertToDegrees(fAngle) || XMVector3Equal(vAxis, XMVectorSet(0.f,0.f,0.f,0.f)))
	{
		LookAtXZ(vTarget);
	}
	else
	{
		_matrix	RotationMatrix;
		RotationMatrix = XMMatrixRotationAxis(vAxis, fAngle * (_float)TimeDelta);

		vRight = XMVector3TransformNormal(vRight, RotationMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationMatrix);

		Set_State(CTransform::STATE_RIGHT, vRight);
		Set_State(CTransform::STATE_UP, vUp);
		Set_State(CTransform::STATE_LOOK, vLook);
	}


	return S_OK;
}

void CTransform::ArrangeMatrix()
{

	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight = XMVector4Normalize(vRight);

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(CTransform::STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(CTransform::STATE_LOOK, vLook * Get_Scale(STATE_LOOK));

}

CTransform * CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTransform*	pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
