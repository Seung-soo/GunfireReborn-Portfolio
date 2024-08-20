#include "..\public\Cell.h"
#include "VIBuffer_Triangle.h"
#include "Shader.h"
#include "GameObject.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}



CGameObject** CCell::Get_CellPoints()
{
	return m_pCellPoint;
}

HRESULT CCell::NativeConstruct(const _float3 * pPoints, _uint iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	XMStoreFloat3(&m_vLineDir[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	XMStoreFloat3(&m_vLineDir[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	XMStoreFloat3(&m_vLineDir[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));

	m_iIndex = iIndex;

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
			return true;
	}
	return false;
}

_bool CCell::isIn(_fvector vPosition, _int * pNeighborIndex, _float3** pOut)
{
	for (_uint i = 0; i < LINE_END; i++)
	{
		_vector		vDestDir = XMVector3Normalize(vPosition 
			- XMLoadFloat3(&m_vPoints[i]));
		_vector		vSourDir = XMVector3Normalize(XMLoadFloat4(
			&_float4(m_vLineDir[i].z * -1.f, 0.f, m_vLineDir[i].x, 0.f)));

		if (0 < XMVectorGetX(XMVector3Dot(vDestDir, vSourDir)))
		{
			if (0 <= m_iNeighbor[i])
				*pNeighborIndex = m_iNeighbor[i];
			if (nullptr != pOut)
				*pOut = &m_vLineDir[i];
			return false;
		}
	}
	return true;
}
#ifdef _DEBUG
void CCell::CellPointsDead()
{
	for (_uint i = 0; i < POINT_END; i++)
	{
		m_pCellPoint[i]->Set_Dead(true);
	}
}

HRESULT CCell::Render(CVIBuffer_Triangle * pVIBuffer, CShader * pShader, _float4 vColor)
{
	if (FAILED(pVIBuffer->Update(m_vPoints)))
	{
		MSGBOX("pVIBuffer->Update returned E_FAIL in CCell::Render");
		return E_FAIL;
	}

	if (TYPE_ROAD == m_eType && false == XMVector3Equal(XMLoadFloat4(&vColor), XMVectorSet(1.f, 0.f, 0.f, 1.f)))
		vColor = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CCell::Render");
		return E_FAIL;
	}

	if (FAILED(pVIBuffer->Render(pShader, 0)))
	{
		MSGBOX("pVIBuffer->Render returned E_FAIL in CCell::Render");
		return E_FAIL;
	}

	return S_OK;
}
#endif
CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _float3 * pPoints, _uint iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pPoints, iIndex)))
	{
		MSGBOX("Failed to Creating CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
