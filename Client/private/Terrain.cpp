#include "stdafx.h"
#include "..\public\Terrain.h"
#include "GameInstance.h"


CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

void CTerrain::Set_Info(_float3 vPosition, _int iTextureIndex, _int iPass)
{
	_float4 Pos = _float4(vPosition, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));

	m_iTexIndex = iTextureIndex;
	m_iPass = iPass;
}

HRESULT CTerrain::Set_Navigation(const _tchar* pPrototypeTag)
{
#ifndef USE_IMGUI
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pNavigationCom);
	m_Components.erase(TEXT("Com_Navigation"));
	/* For.Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.iCurrentIndex = -1;
	NaviDesc.pParent = m_pTransformCom;
	if (FAILED(__super::Add_Component(g_eCurrentLevel, pPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTerrain::Set_Navigation(Navigation)");
		return E_FAIL;
	}
#endif
	return S_OK;
}

HRESULT CTerrain::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CTerrain::NativeContruct_Prototype");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CTerrain::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CTerrain::NativeConstruct");
		return E_FAIL;
	}
	m_pInfoTerrain = (INFO_TERRAIN*)pArg;

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CTerrain::NativeConstruct");
		return E_FAIL;
	}

#ifdef USE_IMGUI
	//if (FAILED(Create_FilterTexture()))
	//{
	//	MSGBOX("Create_FilterTexture returned E_FAIL in CTerrain::NativeConstruct");
	//	return E_FAIL;
	//}
#endif

	return S_OK;
}

_int CTerrain::Tick(_double dTimeDelta)
{
	if (m_bDead)
		return -1;

	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CTerrain::Tick");
		return -1;
	}
#ifdef USE_IMGUI
	m_iTexIndex = m_pInfoTerrain->iTextureIndex;
	m_iPass = m_pInfoTerrain->iPass;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pInfoTerrain->vPosition));
#endif
	return _int();
}

_int CTerrain::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CPlCTerrainayer::LateTick");
		return -1;
	}



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
//#ifdef _DEBUG	
//	m_pRendererCom->Add_Debug(m_pNavigationCom);
//#endif // _DEBUG
	return _int();
}




HRESULT CTerrain::Render()
{
	if (nullptr == m_pVIBufferCom)
	{
		MSGBOX("nullptr == m_pVIBufferCom in CPlayer::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CTerrain::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, m_iPass);


	return S_OK;
}

HRESULT CTerrain::Terrain_UpDownDefault(_int iSharpness, _float fDivisibleValue, _float fMaxY, _float fMinY)
{

	_vector vMousePos;

	m_pVIBufferCom->Pick(m_pTransformCom->Get_WorldMatrixInverse(), &vMousePos);
	_float4	vMousePosition;
	XMStoreFloat4(&vMousePosition, vMousePos);
	m_vBrushPosition = XMLoadFloat4(&vMousePosition);
	if (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
	{
		m_pVIBufferCom->MakeTerrainDefault(vMousePosition, m_fRadius, iSharpness, fDivisibleValue, fMaxY, fMinY);
	}

	return S_OK;
}

HRESULT CTerrain::Terrain_UpDownSlope(_float fSlope)
{

	_vector vMousePos;

	m_pVIBufferCom->Pick(m_pTransformCom->Get_WorldMatrixInverse(), &vMousePos);
	_float4	vMousePosition;
	XMStoreFloat4(&vMousePosition, vMousePos);
	m_vBrushPosition = XMLoadFloat4(&vMousePosition);
	if (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
	{
		m_pVIBufferCom->MakeTerrainSlope(vMousePosition, m_fRadius, fSlope);
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CTerrain::Terrain_Angular()
{
	_vector vMousePos;

	m_pVIBufferCom->Pick(m_pTransformCom->Get_WorldMatrixInverse(), &vMousePos);
	_float4	vMousePosition;
	XMStoreFloat4(&vMousePosition, vMousePos);
	m_vBrushPosition = XMLoadFloat4(&vMousePosition);

	if (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
	{
		m_pVIBufferCom->MakeTerrainAngular(vMousePosition, m_fRadius);
	}

	return S_OK;

}

HRESULT CTerrain::Terrain_TwoPointSlope()
{



	if (g_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
	{
		_vector vMousePos;
		if (true == m_pVIBufferCom->Pick(m_pTransformCom->Get_WorldMatrixInverse(), &vMousePos))
		{
			if (false == m_bFirstPoint)
			{
				m_bFirstPoint = true;
				m_vBrushPosition = vMousePos;
				XMStoreFloat4(&m_vFirstPos, vMousePos);

				return S_OK;
			}
			else if (true == m_bFirstPoint)
			{
				m_bSecondPoint = true;
				m_vBrushPosition = vMousePos;
				XMStoreFloat4(&m_vSecondPos, vMousePos);
			}
		}
		if (true == m_bFirstPoint && true == m_bSecondPoint)
		{
			m_pVIBufferCom->MakeTerrainTwoPointSlope(m_vFirstPos, m_vSecondPos, m_fRadius);
			m_bFirstPoint = false;
			m_bSecondPoint = false;
		}
		_float4	vMousePosition;
		XMStoreFloat4(&vMousePosition, vMousePos);
		m_vBrushPosition = XMLoadFloat4(&vMousePosition);
	}

	return S_OK;
}

HRESULT CTerrain::Light_Update()
{
	if (g_pGameInstance->Get_KeyEnter(DIK_E))
		m_pVIBufferCom->MakeLight();
	return S_OK;
}

HRESULT CTerrain::Terrain_Update()
{
	if (g_pGameInstance->Get_KeyEnter(DIK_Q))
		m_pVIBufferCom->Change_Terrain(m_pInfoTerrain->iNumVerticesX, m_pInfoTerrain->iNumVerticesZ, m_pInfoTerrain->fTileNumX, m_pInfoTerrain->fTileNumZ);
	return S_OK;
}

_bool CTerrain::Pick_Terrain(_float3 * vMousePos)
{
	_vector vMousePosition = XMLoadFloat3(vMousePos);
	_bool isPick = false;
	isPick = m_pVIBufferCom->Pick(m_pTransformCom->Get_WorldMatrixInverse(), &vMousePosition);
	XMStoreFloat3(vMousePos, vMousePosition);
	return isPick;
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTerrain::SetUp_Components(Renderer)");
		return E_FAIL;
	}


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTerrain::SetUp_Components(Shader)");
		return E_FAIL;
	}


	/* For.Com_VIBuffer */
#ifdef USE_IMGUI
	char szPrototypeName[MAX_PATH] = "Prototype_Component_VIBuffer_Terrain_";
	strcat_s(szPrototypeName, m_pInfoTerrain->szChildName);

	_tchar wszPrototypeName[MAX_PATH] = L"";


	MultiByteToWideChar(CP_ACP, 0, szPrototypeName, MAX_PATH, wszPrototypeName, sizeof(_tchar) * MAX_PATH);
	if (false == m_pInfoTerrain->bDelete)
	{
		if (FAILED(__super::Add_Component(LEVEL_IMGUI, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CTerrain::SetUp_Components(VIBuffer)");
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_IMGUI, wszPrototypeName, TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CTerrain::SetUp_Components(VIBuffer_Test)");
			return E_FAIL;
		}
	}
#else
	if (FAILED(__super::Add_Component(g_eCurrentLevel, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTerrain::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}
	///* For.Com_Navigation*/
	//CNavigation::NAVIDESC		NaviDesc;
	//NaviDesc.iCurrentIndex = -1;
	//NaviDesc.pParent = m_pTransformCom;
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	//{
	//	MSGBOX("__super::Add_Component returned E_FAIL in CTerrain::SetUp_Components(Navigation)");
	//	return E_FAIL;
	//}
#endif



	/* For.Com_Texture_Diffuse */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer::SetUp_Components(Texture)");
		return E_FAIL;
	}
	///* For.Com_Texture_Filter */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Texture_Filter"), (CComponent**)&m_pTextureCom[TYPE_FILTER])))
	//{
	//	MSGBOX("__super::Add_Component returned E_FAIL in CPlayer::SetUp_Components(Texture_Filter)");
	//	return E_FAIL;
	//}
#ifdef USE_IMGUI
	/* For.Com_Texture_Brush */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Brush"), TEXT("Com_Texture_Brush"), (CComponent**)&m_pTextureCom[TYPE_BRUSH])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTerrain::SetUp_Components(Texture_Brush)");
		return E_FAIL;
	}
#endif
	return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CTerrain::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CTerrain::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTerrain::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTerrain::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	// 터레인 텍스쳐
	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CTerrain::SetUp_ConstantTable(DiffuseTexture)");
		return E_FAIL;
	}
	//// 필터 텍스쳐
	//if (FAILED(m_pTextureCom[TYPE_FILTER]->SetUp_OnShader(m_pShaderCom, "g_FilterTexture", 0)))
	//{
	//	MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CTerrain::SetUp_ConstantTable(FilterTexture)");
	//	return E_FAIL;
	//}
#ifdef USE_IMGUI
	// 브러쉬 텍스쳐
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->SetUp_OnShader(m_pShaderCom, "g_BrushTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CTerrain::SetUp_ConstantTable(BrushTexture)");
		return E_FAIL;
	}
	// 브러쉬 포지션
	_float4 vBrushPosition;
	XMStoreFloat4(&vBrushPosition, m_vBrushPosition);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushPos", &vBrushPosition, sizeof(_float4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTerrain::SetUp_ConstantTable(g_vBrushPos)");
		return E_FAIL;
	}

	// 브러쉬 반지름
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRadius", &m_fRadius, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTerrain::SetUp_ConstantTable(g_fRadius)");
		return E_FAIL;
	}
#endif
	//m_pShaderCom->Set_Texture("g_FilterTexture", m_pSRV);

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in CTerrain::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTerrain::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTerrain::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTerrain::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTerrain::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTerrain::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	static _int fWave = 1;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fWaveValue", &fWave, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTerrain::SetUp_ConstantTable(g_fWaveValue)");
		return E_FAIL;
	}
	fWave++;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CTerrain::Create_FilterTexture()
{
	/*ID3D11Texture2D*		pTexture;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = m_pInfoTerrain->iNumVerticesX;
	TextureDesc.Height = m_pInfoTerrain->iNumVerticesZ;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA			SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	_ulong*		pPixel = new _ulong[m_pInfoTerrain->iNumVerticesX * m_pInfoTerrain->iNumVerticesZ];
	ZeroMemory(pPixel, sizeof(_ulong) * m_pInfoTerrain->iNumVerticesX * m_pInfoTerrain->iNumVerticesZ);

	for (_int i = 0; i < m_pInfoTerrain->iNumVerticesZ; i++)
	{
		for (_int j = 0; j < m_pInfoTerrain->iNumVerticesX; j++)
		{
			_int	iIndex = i * m_pInfoTerrain->iNumVerticesX + j;

			pPixel[iIndex] = D3D11COLOR_ARGB(255, 255, 255, 255);

		}
	}

	pPixel[0] = D3D11COLOR_ARGB(255, 255, 255, 255);

	SubResourceData.pSysMem = pPixel;
	SubResourceData.SysMemPitch = sizeof(_ulong) * m_pInfoTerrain->iNumVerticesX;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture)))
	{
		MSGBOX("m_pDevice->CreateTexture2D returned E_FAIL in CTerrain::Create_FilterTexture");
		return E_FAIL;
	}
	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, nullptr, &m_pSRV)))
	{
		MSGBOX("m_pDevice->CreateShaderResourceView returned E_FAIL in CTerrain::Create_FilterTexture");
		return E_FAIL;
	}

	if (FAILED(SaveWICTextureToFile(m_pDeviceContext, pTexture, GUID_ContainerFormatPng, TEXT("../Bin/Texture/MyUI/Test.png"), &GUID_WICPixelFormat32bppBGRA)))
	{
		MSGBOX("SaveWICTextureToFile returned E_FAIL in CTerrain::Create_FilterTexture");
		return E_FAIL;
	}
	Safe_Release(pTexture);*/

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTerrain*	pInstance = new CTerrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	for (auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);

	Safe_Release(m_pSRV);
	Safe_Release(m_pShaderCom);
}
