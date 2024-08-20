#include "stdafx.h"
#include "..\public\Ocean.h"
#include "GameInstance.h"


COcean::COcean(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

COcean::COcean(const COcean & rhs)
	: CGameObject(rhs)
{
}

HRESULT COcean::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in COcean::NativeContruct_Prototype");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT COcean::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in COcean::NativeConstruct");
		return E_FAIL;
	}

	if (nullptr != pArg)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vector*)pArg);


	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in COcean::NativeConstruct");
		return E_FAIL;
	}
	m_iTexIndex = 22;


	return S_OK;
}

_int COcean::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in COcean::Tick");
		return -1;
	}

	return _int();
}

_int COcean::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in COcean::LateTick");
		return -1;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return _int();
}




HRESULT COcean::Render()
{
	if (nullptr == m_pVIBufferCom)
	{
		MSGBOX("nullptr == m_pVIBufferCom in COcean::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in COcean::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 3);

	return S_OK;
}

HRESULT COcean::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in COcean::SetUp_Components(Renderer)");
		return E_FAIL;
	}


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in COcean::SetUp_Components(Shader)");
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_Ocean"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in COcean::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture_Diffuse */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in COcean::SetUp_Components(Texture)");
		return E_FAIL;
	}

	/* For.Com_NoiseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_NoiseTexture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in COcean::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT COcean::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in COcean::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in COcean::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in COcean::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in COcean::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	// 터레인 텍스쳐
	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 7)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in COcean::SetUp_ConstantTable(DiffuseTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourDiffuseTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in COcean::SetUp_ConstantTable(DiffuseTexture)");
		return E_FAIL;
	}

	//const LIGHTDESC*	pLightDesc = nullptr;
	//if (LEVEL_LOBY == g_eCurrentLevel)
	//	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (LEVEL_STAGE3_2 == g_eCurrentLevel)
	//	pLightDesc = pGameInstance->Get_LightDesc(1);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in COcean::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in COcean::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in COcean::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in COcean::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in COcean::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in COcean::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	static _int fWave = -1;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fWaveValue", &fWave, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in COcean::SetUp_ConstantTable(g_fWaveValue)");
		return E_FAIL;
	}
	fWave--;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

COcean * COcean::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	COcean*	pInstance = new COcean(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created COcean");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * COcean::Clone(void * pArg)
{
	COcean*	pInstance = new COcean(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created COcean");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COcean::Free()
{
	__super::Free();

	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	for (auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);

	Safe_Release(m_pShaderCom);
}
