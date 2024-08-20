#include "..\public\VIBuffer.h"
#include "Shader.h"
#include "Picking.h"
#include "GameInstance.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_VBDesc(rhs.m_VBDesc)
	, m_VBSubResourceData(rhs.m_VBSubResourceData)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_IBDesc(rhs.m_IBDesc)
	, m_IBSubResourceData(rhs.m_IBSubResourceData)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_iNumIndicesPerPrimitive(rhs.m_iNumIndicesPerPrimitive)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_eTopology(rhs.m_eTopology)
	, m_pVerticesPos(rhs.m_pVerticesPos)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);

}

HRESULT CVIBuffer::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render(CShader* pShader, _uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*	pVertexBuffers[] = { m_pVB };

	_uint			iStrides[] = { m_VBDesc.StructureByteStride };

	_uint			iOffsets[] = { 0, };

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	if (FAILED(pShader->Set_InputLayout(iPassIndex)))
	{
		MSGBOX("pShader->Set_InputLayout returned E_FAIL in CVIBuffer::Render");
		return E_FAIL;
	}
	if (FAILED(pShader->Apply(iPassIndex)))
	{
		MSGBOX("pShader->Apply returned E_FAIL in CVIBuffer::Render");
		return E_FAIL;
	}
	
	m_pDeviceContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);
	return S_OK;
}

_bool CVIBuffer::Pick(const _matrix& WorldMatrixInverse, _fvector * pOut)
{
	CPicking*		pPicking = GET_INSTANCE(CPicking);

	pPicking->Transform_ToLocalSpace(WorldMatrixInverse);

	_uint		iIndexByte = 0;

	if (m_eIndexFormat == DXGI_FORMAT_R16_UINT)
		iIndexByte = 2;
	else
		iIndexByte = 4;

	_bool		isPick = false;




	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	VTXNORTEX*	pVertices = (VTXNORTEX*)mappedResource.pData;
	m_pDeviceContext->Unmap(m_pVB, 0);

	D3D11_MAPPED_SUBRESOURCE mappedResourceIndex;
	ZeroMemory(&mappedResourceIndex, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResourceIndex);
	FACEINDICES32*	pIndices = (FACEINDICES32*)mappedResourceIndex.pData;
	m_pDeviceContext->Unmap(m_pIB, 0);

	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//_vector vCameraLook = static_cast<CTransform*>(pGameInstance->Get_Component(0, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK);

	//RELEASE_INSTANCE(CGameInstance);

	_vector vRay[2] = { pPicking->Get_RayDirLocal(), pPicking->Get_RayPosLocal()};
	//memcpy(vRay, pPicking->Get_Ray(), sizeof(_vector) * 2);
	_vector vNowPos;

	_uint StartX;
	_uint StartZ;
	_uint EndX;
	_uint EndZ;
	for (_int i = 0; i < 200; i++)
	{
		vNowPos = vRay[1] + vRay[0] * (_float)i;
		if ((XMVectorGetZ(vNowPos) < 0.f || XMVectorGetZ(vNowPos) > _float(m_iNumVerticesZ - 1))|| (XMVectorGetX(vNowPos) < 0.f || XMVectorGetX(vNowPos) > _float(m_iNumVerticesX - 1)))
			continue;
		_int	iIndex = (_int)XMVectorGetZ(vNowPos) * m_iNumVerticesX + (_int)XMVectorGetX(vNowPos);
		if (0 > iIndex || iIndex >= (_int)m_iNumVertices)
			continue;
		if (pVertices[iIndex].vPosition.y >= XMVectorGetY(vNowPos))
		{
			_vector vPastPos = vRay[1] + vRay[0] * (_float)(i - 1);

			StartX = XMVectorGetX(vPastPos) < pVertices[iIndex].vPosition.x ? (_int)XMVectorGetX(vPastPos) - 1 : (_int)pVertices[iIndex].vPosition.x - 1;
			EndX = XMVectorGetX(vPastPos) < pVertices[iIndex].vPosition.x ? (_int)pVertices[iIndex].vPosition.x + 2 : (_int)XMVectorGetX(vPastPos) + 2;
			StartZ = XMVectorGetZ(vPastPos) < pVertices[iIndex].vPosition.z ? (_int)XMVectorGetZ(vPastPos) - 1 : (_int)pVertices[iIndex].vPosition.z - 1;
			EndZ = XMVectorGetZ(vPastPos) < pVertices[iIndex].vPosition.z ? (_int)pVertices[iIndex].vPosition.z + 2 : (_int)XMVectorGetZ(vPastPos) + 2;

			if (StartZ < 0.f)
			{
				StartZ = 0;
				EndZ = 1;
			}
			if (EndZ > _float(m_iNumVerticesZ - 1))
			{
				StartZ = m_iNumVerticesZ - 2;
				EndZ = m_iNumVerticesZ - 1;
			}
			if (StartX < 0.f)
			{
				StartX = 0;
				EndX = 1;
			}
			if (EndX > _float(m_iNumVerticesX - 1))
			{
				StartX = m_iNumVerticesX - 2;
				EndX = m_iNumVerticesX - 1;
			}

			break;
		}
		if (i == 199)
		{
			RELEASE_INSTANCE(CPicking);
			return false;
		}
	}

	for (_uint i = StartZ; i < EndZ; ++i)
	{
		for (_uint j = StartX * 2; j < EndX * 2; j++)
		{
			_uint iIndex = i * (m_iNumVerticesX - 1) * 2 + j;
			_uint	iIndices[3] = { 0 };


			iIndices[0] = ((FACEINDICES32*)pIndices)[iIndex]._0;
			iIndices[1] = ((FACEINDICES32*)pIndices)[iIndex]._1;
			iIndices[2] = ((FACEINDICES32*)pIndices)[iIndex]._2;



			_vector		vPoint[3] = {
				XMLoadFloat3((_float3*)(((_byte*)pVertices) + m_VBDesc.StructureByteStride * iIndices[0])),
				XMLoadFloat3((_float3*)(((_byte*)pVertices) + m_VBDesc.StructureByteStride * iIndices[1])),
				XMLoadFloat3((_float3*)(((_byte*)pVertices) + m_VBDesc.StructureByteStride * iIndices[2]))
			};

			if (isPick = pPicking->isPick(vPoint, pOut))
			{
				_matrix		WorldMatrix = XMMatrixInverse(nullptr, WorldMatrixInverse);
				*(_vector*)pOut = XMVector3TransformCoord(*pOut, WorldMatrix);
				RELEASE_INSTANCE(CPicking);
				return true;
			}
		}
	}

	RELEASE_INSTANCE(CPicking);

	return isPick;
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubResourceData, &m_pVB);
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_IBDesc, &m_IBSubResourceData, &m_pIB);
}

void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pVerticesPos);
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);
	}

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
