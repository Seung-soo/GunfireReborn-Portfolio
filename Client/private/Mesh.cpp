#include "stdafx.h"
#include "..\public\Mesh.h"
#include "GameInstance.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CMesh::CMesh(const CMesh & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMesh::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CMesh::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMesh::NativeConstruct(void * pArg)
{
	m_pInfoMesh = (INFO_MESH*)pArg;
	m_iPass = m_pInfoMesh->iPass;
	m_fFrustumRadius = m_pInfoMesh->fFrustumRadius;

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CMesh::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CMesh::NativeConstruct");
		return E_FAIL;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_WorldMatrix(m_pInfoMesh->vWorldMatrix);


	return S_OK;
}

_int CMesh::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CMesh::Tick");
		return -1;
	}
#ifdef USE_IMGUI
	m_iPass = m_pInfoMesh->iPass;
	m_fFrustumRadius = m_pInfoMesh->fFrustumRadius;
#endif
	return _int();
}

_int CMesh::LateTick(_double dTimeDelta)
{
	if (m_bDead)
		return -1;

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CMesh::LateTick");
		return -1;
	}

	// 절두체
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRadius))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return _int();
}




HRESULT CMesh::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pVIBufferCom in CMesh::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CMesh::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i);
	}

	return S_OK;
}

HRESULT CMesh::Set_Model(_tchar * pPrototypeTag)
{
	if (nullptr != m_pModelCom)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);

	}
	m_Components.erase(TEXT("Com_Model"));
#ifdef USE_IMGUI
	if (FAILED(__super::Add_Component(LEVEL_STATIC, pPrototypeTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMesh::Set_Model");
		return E_FAIL;
	}
#else
	if (FAILED(__super::Add_Component(g_eCurrentLevel, pPrototypeTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMesh::Set_Model");
		return E_FAIL;
	}
#endif
	return S_OK;
}

HRESULT CMesh::Set_Tex(_tchar * pPrototypeTag)
{
	if (nullptr != m_pTextureCom)
	{
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pTextureCom);

	}
	m_Components.erase(TEXT("Com_Texture"));
#ifdef USE_IMGUI
	if (FAILED(__super::Add_Component(LEVEL_IMGUI, pPrototypeTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMesh::Set_Tex");
		return E_FAIL;
	}
#else
	if (FAILED(__super::Add_Component(LEVEL_STATIC, pPrototypeTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMesh::Set_Tex");
		return E_FAIL;
	}
#endif
	return S_OK;
}

HRESULT CMesh::SetUp_OnTerrain()
{

	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)g_pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CMesh::SetUp_OnTerrain");
		return E_FAIL;
	}

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CTransform* pTerrainTransform = (CTransform*)g_pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CMesh::SetUp_OnTerrain");
		return E_FAIL;
	}

	// 로컬 포지션 구하기
	_matrix	WorldMatrixInverse = pTerrainTransform->Get_WorldMatrixInverse();

	_vector	vLocalPos = XMVector3TransformCoord(vPosition, WorldMatrixInverse);
	_float3 vLocalPosFloat3;
	XMStoreFloat3(&vLocalPosFloat3, vLocalPos);

	vPosition = XMVector3TransformCoord(vLocalPos, pTerrainTransform->Get_WorldMatrix());

	_float fTerrainY = pVIBuffer_Terrain->Compute_Y(vLocalPosFloat3);

	// 매쉬에 대입

	vPosition = XMVectorSetY(vPosition, fTerrainY);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pInfoMesh->vPosition[0] = XMVectorGetX(vPosition);
	m_pInfoMesh->vPosition[1] = XMVectorGetY(vPosition);
	m_pInfoMesh->vPosition[2] = XMVectorGetZ(vPosition);

	return S_OK;
}

HRESULT CMesh::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMesh::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMesh::SetUp_Components(Shader)");
		return E_FAIL;
	}
#ifndef USE_IMGUI
	// 불러온 모델들 프로토타입 선언
	char szFileName[MAX_PATH] = "";
	_splitpath_s(m_pInfoMesh->szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
	char szPrototypeNameFront[MAX_PATH] = "Prototype_Component_Texture_";
	strcat_s(szPrototypeNameFront, szFileName);

	_tchar wszPrototype[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, szPrototypeNameFront, sizeof(char) * MAX_PATH, wszPrototype, sizeof(char) * MAX_PATH);

	///* For.Com_Texture_Diffuse */
	//if (FAILED(__super::Add_Component(g_eCurrentLevel, wszPrototype, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	//{
	//	MSGBOX("__super::Add_Component returned E_FAIL in CMesh::SetUp_Components(Texture)");
	//	return E_FAIL;
	//}
#endif
	//_tchar
	//m_pInfoMesh->szFullPath
	///* For.Com_VIBuffer */
	//if (FAILED(__super::Add_Component(g_eCurrentLevel, TEXT("Prototype_Component_VIBuffer_Model"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
	//{
	//	MSGBOX("__super::Add_Component returned E_FAIL in CMesh::SetUp_Components(VIBuffer)");
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CMesh::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CMesh::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CMesh::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMesh::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMesh::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	// 터레인 텍스쳐
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CMesh::SetUp_ConstantTable(DiffuseTexture)");
		return E_FAIL;
	}

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in CMesh::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMesh::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMesh::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMesh::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMesh::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMesh::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMesh*	pInstance = new CMesh(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMesh::Clone(void * pArg)
{
	CMesh*	pInstance = new CMesh(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
