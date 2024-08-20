#include "stdafx.h"
#include "..\public\FireLight.h"
#include "GameInstance.h"

CFireLight::CFireLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CFireLight::CFireLight(const CFireLight & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFireLight::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireLight::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CFireLight::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CFireLight::NativeConstruct");
		return E_FAIL;
	}
	_float4	vPosition = *(_float4*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));

	m_pTransformCom->Scaled(XMVectorSet(2.5f, 2.5f, 1.f, 0.f));

	m_iTexIndex = 47;
	return S_OK;
}

_int CFireLight::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CFireLight::Tick");
		return DEAD;
	}

	if (true == g_pGameInstance->Get_KeyEnter(DIK_RIGHT))
	{
		m_iTexIndex++;
		if (m_iTexIndex > 98)
			m_iTexIndex = 0;
	}
	else if (true == g_pGameInstance->Get_KeyEnter(DIK_LEFT))
	{
		m_iTexIndex--;
		if (m_iTexIndex < 0)
			m_iTexIndex = 98;
	}
	m_fAddNoiseUV += (_float)TimeDelta;

	return _int();
}

_int CFireLight::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CFireLight::LateTick");
		return DEAD;
	}


	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.5f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}

HRESULT CFireLight::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CFireLight::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 25)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CFireLight::Render");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CFireLight::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFireLight::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFireLight::SetUp_Components(Shader");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFireLight::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_BlueLight"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom[0])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFireLight::SetUp_Components(Texture");
		return E_FAIL;
	}
	/* For.Com_Texture1 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_SourceTexture"), (CComponent**)&m_pTextureCom[1])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFireLight::SetUp_Components(Noise_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFireLight::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CFireLight::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CFireLight::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CFireLight::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CFireLight::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom[0]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iRealTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CFireLight::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom[1]->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CFireLight::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_AddNoiseUV", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CFireLight::SetUp_ConstantTable(g_AddNoiseUV)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CFireLight * CFireLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CFireLight*	pInstance = new CFireLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CFireLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFireLight::Clone(void * pArg)
{
	CFireLight*	pInstance = new CFireLight(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CFireLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireLight::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	for (auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);
	Safe_Release(m_pShaderCom);
}
