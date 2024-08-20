#include "..\public\VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_fTileNumX(rhs.m_fTileNumX)
	, m_fTileNumZ(rhs.m_fTileNumZ)
{
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(_uint iNumX, _uint iNumZ)
{
	m_iNumVerticesX = iNumX;
	m_iNumVerticesZ = iNumZ;
#pragma region VERTEX_BUFFER
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	m_VBDesc.ByteWidth = sizeof(VTXNORTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXNORTEX);

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);
	m_pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((_float)j, 0.0f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 1.f, 0.f);
			pVertices[iIndex].vTexUV = _float2((_float)j / (m_iNumVerticesX - 1) * m_fTileNumX, (_float)i / (m_iNumVerticesZ - 1) * m_fTileNumZ);
			((VTXNORTEX*)m_pVertices)[iIndex] = pVertices[iIndex];
		}
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;



#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	m_iIndicesSize = sizeof(FACEINDICES32);
	
	FACEINDICES32*	pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, m_iIndicesSize * m_iNumPrimitive);
	_uint		iNumPrimitive = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[1];
			pIndices[iNumPrimitive]._2 = iIndices[2];

			_vector		vSour, vDest, vNormal;
			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._0]);
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;


			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[2];
			pIndices[iNumPrimitive]._2 = iIndices[3];

			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._0]);
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;


	//memcpy(m_pVertices, pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);
	memcpy(m_pIndices, pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(const _tchar* pHeightMap)
{

#pragma region VERTEX_BUFFER

	HANDLE		hFile = CreateFile(pHeightMap, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong					dwByte = 0;
	/*BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;
	
	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	_ulong*		pPixel = new _ulong[m_iNumVerticesX * m_iNumVerticesZ];
	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);*/

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	m_VBDesc.ByteWidth = sizeof(VTXNORTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXNORTEX);

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	ReadFile(hFile, &m_iNumVerticesX, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumVerticesZ, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_fTileNumX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fTileNumZ, sizeof(_float), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			_float		fPosY = /*_float((pPixel[iIndex] & 0xff000000) >> 24) + _float((pPixel[iIndex] & 0x00ff0000) >> 16) / 100.f*/0;
			ReadFile(hFile, &fPosY, sizeof(_float), &dwByte, nullptr);
			//if (200.f < fPosY)
			//	fPosY = 0.f;
			//_float		fTileX = _float((pPixel[iIndex] & 0x0000ff00) >> 8);
			//_float		fTileZ = _float((pPixel[iIndex] & 0x000000ff));
			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((_float)j, fPosY, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / ((_float)m_iNumVerticesX - 1.f) * m_fTileNumX, i / ((_float)m_iNumVerticesZ - 1.f) * m_fTileNumZ);
		}
	}



	//pVertices[0].vPosition.y = 0.f;

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	//Safe_Delete_Array(pPixel);
	CloseHandle(hFile);

#pragma endregion


#pragma region INDEX_BUFFER
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	FACEINDICES32*	pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumPrimitive = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[1];
			pIndices[iNumPrimitive]._2 = iIndices[2];

			_vector		vSour, vDest, vNormal;
			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._0]);
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;


			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[2];
			pIndices[iNumPrimitive]._2 = iIndices[3];

			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._0]);
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}


HRESULT CVIBuffer_Terrain::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::MakeTerrainDefault(_float4 vBrushPos, _float fBrushRadius
	, _int iSharpness, _float fDivisibleValue, _float fMaxY, _float fMinY)
{

#pragma region VERTEX_BUFFER
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	
	if (FAILED(m_pDeviceContext->Map(m_pVB, D3D11CalcSubresource(0,0,1)
		, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
	{
		MSGBOX("!");
		return E_FAIL;
	}

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_int StartZ = _int(vBrushPos.z - fBrushRadius);
	if (0 > StartZ)
		StartZ = 0;

	_int EndZ = _int(vBrushPos.z + fBrushRadius + 1);
	if ((_int)m_iNumVerticesZ - 1 < EndZ)
		EndZ = m_iNumVerticesZ;

	_int StartX = _int(vBrushPos.x - fBrushRadius);
	if (0 > StartX)
		StartX = 0;

	_int EndX = _int(vBrushPos.x + fBrushRadius + 1);
	if ((_int)m_iNumVerticesX - 1 < EndX)
		EndX = m_iNumVerticesX;

	VTXNORTEX*		pVertices = (VTXNORTEX*)mappedResource.pData;

	for (_int i = StartZ; i < EndZ; ++i)
	{
		for (_int j = StartX; j < EndX; ++j)
		{
			_int		iIndex = i * m_iNumVerticesX + j;

			_float fDistance = XMVectorGetY(XMVector4Length(XMLoadFloat4(&vBrushPos) 
				- XMVectorSet((_float)j, vBrushPos.y, (_float)i, 1.f)));

			if (fDistance > fBrushRadius)
				continue;

			_float fAddValue = fBrushRadius - fDistance ;
			if (0 > iSharpness)
				fAddValue = pow(fAddValue, -iSharpness);
			else
				for (_int sqrtNum = 0; sqrtNum < iSharpness; sqrtNum++)
					fAddValue = sqrtf(fAddValue);

			pVertices[iIndex].vPosition.y += fAddValue / fDivisibleValue;

			if (pVertices[iIndex].vPosition.y > fMaxY)
				pVertices[iIndex].vPosition.y = fMaxY;
			else if (pVertices[iIndex].vPosition.y < fMinY)
				pVertices[iIndex].vPosition.y = fMinY;
		}
	}
	m_pDeviceContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::MakeTerrainSlope(_float4 vBrushPos, _float fBrushRadius, _float fSlope)
{
#pragma region VERTEX_BUFFER


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (FAILED(m_pDeviceContext->Map(m_pVB, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
	{
		MSGBOX("!");
		return E_FAIL;
	}

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_int StartZ = _int(vBrushPos.z - fBrushRadius);
	if (0 > StartZ)
		StartZ = 0;

	_int EndZ = _int(vBrushPos.z + fBrushRadius + 1);
	if ((_int)m_iNumVerticesZ - 1 < EndZ)
		EndZ = m_iNumVerticesZ;

	_int StartX = _int(vBrushPos.x - fBrushRadius);
	if (0 > StartX)
		StartX = 0;

	_int EndX = _int(vBrushPos.x + fBrushRadius + 1);
	if ((_int)m_iNumVerticesX - 1 < EndX)
		EndX = m_iNumVerticesX;

	VTXNORTEX*		pVertices = (VTXNORTEX*)mappedResource.pData;

	for (_int i = StartZ; i < EndZ; ++i)
	{
		for (_int j = StartX; j < EndX; ++j)
		{
			_int		iIndex = i * m_iNumVerticesX + j;

			_float fDistance = XMVectorGetY(XMVector4Length(XMLoadFloat4(&vBrushPos)
				- XMVectorSet((_float)j, vBrushPos.y, (_float)i, 1.f)));

			if (fDistance > fBrushRadius)
				continue;


			pVertices[iIndex].vPosition.y = fSlope;
		}
	}

	m_pDeviceContext->Unmap(m_pVB, 0);



	return S_OK;
}

HRESULT CVIBuffer_Terrain::MakeTerrainAngular(_float4 vBrushPos, _float fBrushRadius)
{


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (FAILED(m_pDeviceContext->Map(m_pVB, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
	{
		MSGBOX("!");
		return E_FAIL;
	}

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_int StartZ = _int(vBrushPos.z - fBrushRadius);
	if (0 > StartZ)
		StartZ = 0;

	_int EndZ = _int(vBrushPos.z + fBrushRadius + 1);
	if ((_int)m_iNumVerticesZ - 1 < EndZ)
		EndZ = m_iNumVerticesZ;

	_int StartX = _int(vBrushPos.x - fBrushRadius);
	if (0 > StartX)
		StartX = 0;

	_int EndX = _int(vBrushPos.x + fBrushRadius + 1);
	if ((_int)m_iNumVerticesX - 1 < EndX)
		EndX = m_iNumVerticesX;

	VTXNORTEX*		pVertices = (VTXNORTEX*)mappedResource.pData;

	for (_int i = StartZ; i < EndZ; ++i)
	{
		for (_int j = StartX; j < EndX; ++j)
		{
			_int		iIndex = i * m_iNumVerticesX + j;

			_float fDistance = XMVectorGetY(XMVector4Length(XMLoadFloat4(&vBrushPos) - XMVectorSet((_float)j, vBrushPos.y, (_float)i, 1.f)));

			if (fDistance > 0.3f)
				continue;

			pVertices[iIndex].vPosition.y = pVertices[iIndex].vPosition.y - 0.05f;
		}
	}

	m_pDeviceContext->Unmap(m_pVB, 0);


	return S_OK;
}

HRESULT CVIBuffer_Terrain::MakeTerrainTwoPointSlope(_float4 vFirstPoint, _float4 vSecondPoint, _float fBrushRadius)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (FAILED(m_pDeviceContext->Map(m_pVB, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
	{
		MSGBOX("!");
		return E_FAIL;
	}

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_int StartX = 0;
	_int EndX = 0;
	_int StartZ = 0;
	_int EndZ = 0;
	if (vFirstPoint.x <= vSecondPoint.x)
	{
		StartX = _int(vFirstPoint.x - fBrushRadius);
		if (0 > StartX)
			StartX = 0;

		EndX = _int(vSecondPoint.x + fBrushRadius + 1);
		if ((_int)m_iNumVerticesX - 1 < EndX)
			EndX = m_iNumVerticesX;
	}
	else
	{
		StartX = _int(vSecondPoint.x - fBrushRadius);
		if (0 > StartX)
			StartX = 0;

		EndX = _int(vFirstPoint.x + fBrushRadius + 1);
		if ((_int)m_iNumVerticesX - 1 < EndX)
			EndX = m_iNumVerticesX;
	}

	if (vFirstPoint.z <= vSecondPoint.z)
	{
		StartZ = _int(vFirstPoint.z - fBrushRadius);
		if (0 > StartZ)
			StartZ = 0;

		EndZ = _int(vSecondPoint.z + fBrushRadius + 1);
		if ((_int)m_iNumVerticesZ - 1 < EndZ)
			EndZ = m_iNumVerticesZ;
	}
	else
	{
		StartZ = _int(vSecondPoint.z - fBrushRadius);
		if (0 > StartZ)
			StartZ = 0;

		EndZ = _int(vFirstPoint.z + fBrushRadius + 1);
		if ((_int)m_iNumVerticesZ - 1 < EndZ)
			EndZ = m_iNumVerticesZ;
	}

	VTXNORTEX*		pVertices = (VTXNORTEX*)mappedResource.pData;
	vSecondPoint.y = 0.f;
	vFirstPoint.y = 0.f;
	_vector vDir = XMLoadFloat4(&vSecondPoint) - XMLoadFloat4(&vFirstPoint);

	for (_int i = StartZ; i < EndZ; ++i)
	{
		for (_int j = StartX; j < EndX; ++j)
		{
			_int		iIndex = i * m_iNumVerticesX + j;

			_vector vPointDir = XMVectorSet((_float)j, 0.f, (_float)j, 1.f) - XMLoadFloat4(&vFirstPoint);
			_float fScale = XMVectorGetX(XMVector4Length(vPointDir));

			_float fDistance = fabsf(XMVectorGetX((XMVector4Dot(XMVector4Normalize(vDir), XMVector4Normalize(vPointDir))))) * fScale;

			//_float fDistance = XMVectorGetY(XMVector4Length(XMLoadFloat4(&vBrushPos) - XMVectorSet((_float)j, vBrushPos.y, (_float)i, 1.f)));

			/*if (fDistance > fBrushRadius)
				continue;*/

			pVertices[iIndex].vPosition.y = fDistance * 0.1f;
		}
	}

	m_pDeviceContext->Unmap(m_pVB, 0);
	return S_OK;
}

HRESULT CVIBuffer_Terrain::MakeLight()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (FAILED(m_pDeviceContext->Map(m_pVB, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
	{
		MSGBOX("!");
		return E_FAIL;
	}
	VTXNORTEX*		pVertices = (VTXNORTEX*)mappedResource.pData;

	D3D11_MAPPED_SUBRESOURCE mappedResourceIndex;
	ZeroMemory(&mappedResourceIndex, sizeof(mappedResourceIndex));

	m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResourceIndex);
	FACEINDICES32*	pIndices = (FACEINDICES32*)mappedResourceIndex.pData;
	_uint		iNumPrimitive = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSour, vDest, vNormal;
			vSour = XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;

			vSour = XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;
		}
	}

	m_pDeviceContext->Unmap(m_pIB, 0);
	m_pDeviceContext->Unmap(m_pVB, 0);

	return S_OK;;
}

HRESULT CVIBuffer_Terrain::Change_Terrain(_uint iNumVerticesX, _uint iNumVerticesZ, _float fTileNumX, _float fTileNumZ)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_fTileNumX = fTileNumX;
	m_fTileNumZ = fTileNumZ;
#pragma region VERTEX_BUFFER

	m_iNumVertices = iNumVerticesX * iNumVerticesZ;
	m_VBDesc.ByteWidth = sizeof(VTXNORTEX) * m_iNumVertices;
	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < iNumVerticesX; ++j)
		{
			_uint iIndex = i * iNumVerticesX + j;

			pVertices[iIndex].vPosition = _float3((_float)j, 0.0f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2((_float)j / (iNumVerticesX - 1) * fTileNumX, (_float)i / (iNumVerticesZ - 1) * fTileNumZ);

		}
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitive = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2;

	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;

	FACEINDICES32*	pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);
	_uint		iNumPrimitive = 0;

	for (_uint i = 0; i < iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + iNumVerticesX,
				iIndex + iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[1];
			pIndices[iNumPrimitive]._2 = iIndices[2];
			++iNumPrimitive;

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[2];
			pIndices[iNumPrimitive]._2 = iIndices[3];
			++iNumPrimitive;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

_float CVIBuffer_Terrain::Compute_Y(const _float3& vPoint)
{

	_uint		iIndex = (_uint)vPoint.z * m_iNumVerticesX + (_uint)vPoint.x;

	if ((_uint)vPoint.x < 0 || (_uint)vPoint.x >= m_iNumVerticesX - 1 || (_uint)vPoint.z < 0 || (_uint)vPoint.z >= m_iNumVerticesZ - 1)
		return 0.f;
	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex
	};

	_float		fWidth = vPoint.x - m_pVerticesPos[iIndices[0]].x;
	_float		fDepth = m_pVerticesPos[iIndices[0]].z - vPoint.z;

	_float4		vPlane;

	if (fWidth > fDepth)
	{
		XMStoreFloat4(&vPlane,XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[1]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]])));
	}
	else
	{
		XMStoreFloat4(&vPlane, XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[3]])));
	}

	return (-vPlane.x * vPoint.x - vPlane.z * vPoint.z - vPlane.w) / vPlane.y;
}

HRESULT CVIBuffer_Terrain::Save_Y(const char* szTerrainSaveFileName)
{
	char szFileName[MAX_PATH] = "../Data/";
	strcat_s(szFileName, szTerrainSaveFileName);
	strcat_s(szFileName, ".dat");
	_tchar wszTerrainSaveFileName[MAX_PATH] = L"";

	MultiByteToWideChar(CP_ACP, 0, szFileName, MAX_PATH, wszTerrainSaveFileName, sizeof(_tchar) * MAX_PATH);

	HANDLE		hFile = CreateFile(wszTerrainSaveFileName,
		GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// 파일을 저장할 수 없으면
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSGBOX("Can't Save");
		return E_FAIL;
	}
	DWORD		dwByte = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (FAILED(m_pDeviceContext->Map(m_pVB, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
	{
		MSGBOX("!");
		return E_FAIL;
	}
	VTXNORTEX*		pVertices = (VTXNORTEX*)mappedResource.pData;

	WriteFile(hFile, &(m_iNumVerticesX), sizeof(_uint), &dwByte, NULL);
	WriteFile(hFile, &(m_iNumVerticesZ), sizeof(_uint), &dwByte, NULL);
	WriteFile(hFile, &(m_fTileNumX), sizeof(_float), &dwByte, NULL);
	WriteFile(hFile, &(m_fTileNumZ), sizeof(_float), &dwByte, NULL);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			WriteFile(hFile, &(pVertices[iIndex].vPosition.y), sizeof(_float), &dwByte, NULL);
		}
	}

	m_pDeviceContext->Unmap(m_pVB, 0);

	CloseHandle(hFile);

	return S_OK;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iNumX, _uint iNumZ)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iNumX, iNumZ)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain in CVIBuffer_Terrain::Create");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pHeightMap)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pHeightMap)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain in CVIBuffer_Terrain::Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
	//Safe_Release(m_pSRV);
}
