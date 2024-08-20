#include "..\public\Navigation.h"
#include "Transform.h"
#include "VIBuffer_Triangle.h"
#include "Shader.h"
#include "PipeLine.h"
#include "Cell.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pVIBuffer(rhs.m_pVIBuffer)
	, m_pShader(rhs.m_pShader)
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
#ifdef _DEBUG
	Safe_AddRef(m_pVIBuffer);
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::NativeConstruct_Prototype(const _tchar * pNaviDataFilePath)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pNaviDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3];
	_uint		iType = 0;
	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		ReadFile(hFile, &iType, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell*	pCell = CCell::Create(m_pDevice, m_pDeviceContext, vPoints, (_uint)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;
		pCell->Set_Type((CCell::TYPE)iType);
		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbor()))
	{
		MSGBOX("SetUp_Neighbor returned E_FAIL in CNavigation::NativeConstruct_Prototype");
		return E_FAIL;
	}

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pVIBuffer)
	{
		MSGBOX("nullptr == m_pVIBuffer in CNavigation::NativeConstruct_Prototype");
		return E_FAIL;
	}

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pShader in CNavigation::NativeConstruct_Prototype");
		return E_FAIL;
	}
#endif

	return S_OK;
}

HRESULT CNavigation::NativeConstruct(void * pArg)
{
	if (nullptr == pArg)
	{
		MSGBOX("nullptr == pArg in CNavigation::NativeConstruct");
		return E_FAIL;
	}

	memcpy(&m_NaviDesc, pArg, sizeof(NAVIDESC));

	return S_OK;
}

_bool CNavigation::Move_OnNavigation(_fvector vPosition, _fvector vCurrentPosition, _float3** pSlidingVector)
{
	_int	iNeighborIndex = -1;

	if (false == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex, pSlidingVector))
	{
		// 나간 쪽에 이웃이 있으면
		if (0 <= iNeighborIndex && CCell::TYPE_NOTROAD != m_Cells[iNeighborIndex]->Get_Type())
		{
			while (true)
			{
				_int	iCurrnetNeighborIndex = -1;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iCurrnetNeighborIndex, pSlidingVector))
				{
					if (XMVectorGetY(vCurrentPosition) + 0.02f < Compute_Y(vPosition) && CCell::TYPE_ROAD ==  m_Cells[iNeighborIndex]->Get_Type())
						return false;
					if (CCell::TYPE_NOTROAD == m_Cells[iNeighborIndex]->Get_Type())
						return false;
					m_NaviDesc.iCurrentIndex = iNeighborIndex;
					*pSlidingVector = nullptr;
					break;
				}
				else
				{
					if (-1 == iCurrnetNeighborIndex)
						return false;
					if (CCell::TYPE_NOTROAD == m_Cells[iNeighborIndex]->Get_Type())
						return false;
				}
				iNeighborIndex = iCurrnetNeighborIndex;

			}
			return true;
		}
		else	// 이웃이 없으면
		{
			return false;	// 못가게 하든 슬라이딩 벡터를 넣든
		}
	}
	else
	{
		return true;
	}
}

_float CNavigation::Compute_Y(_fvector vPoint)
{
	_float4		vPlane;
	
	XMStoreFloat4(&vPlane, XMPlaneFromPoints(
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_A),
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_B),
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_C)
	));

	return (-vPlane.x * XMVectorGetX(vPoint) - vPlane.z * XMVectorGetZ(vPoint) - vPlane.w) / vPlane.y;
}

void CNavigation::Find_Cell(_fvector vPosition)
{
	for (size_t i = 0; i < m_Cells.size(); ++i)
	{
		_int	iCurrnetNeighborIndex = -1;
		_float3*	pNotUse = nullptr;
		if (true == m_Cells[i]->isIn(vPosition, &iCurrnetNeighborIndex, &pNotUse))
		{
			m_NaviDesc.iCurrentIndex = (_uint)i;
			break;
		}
	}
}
#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (nullptr == m_pVIBuffer)
	{
		MSGBOX("nullptr == m_pVIBuffer in CNavigation::Render");
		return E_FAIL;
	}

	CPipeLine*	pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4		WorldMatrix;

	if (nullptr == m_NaviDesc.pParent)
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	else
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_NaviDesc.pParent->Get_WorldMatrix()));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("pTransform->Bind_OnShader returned E_FAIL in CNavigation::Render(World)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CNavigation::Render(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CNavigation::Render(Proj)");
		return E_FAIL;
	}

	_uint	iIndex = 0;

	if (-1 == m_NaviDesc.iCurrentIndex)
	{
		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
			{
				pCell->Render(m_pVIBuffer, m_pShader, iIndex++ == m_NaviDesc.iCurrentIndex ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f));
			}
		}
	}
	else
	{
		m_Cells[m_NaviDesc.iCurrentIndex]->Render(m_pVIBuffer, m_pShader, _float4(1.f, 0.f, 0.f, 1.f));
	}

	RELEASE_INSTANCE(CPipeLine);

	return S_OK;
}
#endif
HRESULT CNavigation::SetUp_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_AB, pDestCell->Get_Index());
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_BC, pDestCell->Get_Index());
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_CA, pDestCell->Get_Index());
			}
		}
	}
	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pNaviDataFilePath)
{
	CNavigation*	pInstance = new CNavigation(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pNaviDataFilePath)))
	{
		MSGBOX("Failed to Creating CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*	pInstance = new CNavigation(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
#endif
}
