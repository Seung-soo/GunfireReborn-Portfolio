#include "stdafx.h"
#include "..\public\RushWarning.h"
#include "GameInstance.h"

CRushWarning::CRushWarning(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CRushWarning::CRushWarning(const CRushWarning & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRushWarning::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CRushWarning::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRushWarning::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CRushWarning::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CRushWarning::NativeConstruct");
		return E_FAIL;
	}
	_vector	vPosition = XMVectorSetY(((CTransform*)pArg)->Get_State(CTransform::STATE_POSITION), 3.f) + ((CTransform*)pArg)->Get_State(CTransform::STATE_LOOK) * 25.f;
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ((CTransform*)pArg)->Get_State(CTransform::STATE_LOOK));
	m_pTransformCom->ArrangeMatrix();
	m_pTransformCom->Scaled(XMVectorSet(m_fScaleX, 1.f, 23.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	return S_OK;
}

_int CRushWarning::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CRushWarning::Tick");
		return E_FAIL;
	}

	if (6.f > m_fScaleX)
		m_fScaleX += (_float)TimeDelta * 12.f;

	m_pTransformCom->Scaled(XMVectorSet(m_fScaleX, 1.f, 23.f, 0.f));

	m_fMoveUV += (_float)TimeDelta;

	if (1.5f < m_fMoveUV)
		return DEAD;
	

	return _int();
}

_int CRushWarning::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CRushWarning::LateTick");
		return E_FAIL;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}

HRESULT CRushWarning::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CRushWarning::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 33)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CRushWarning::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRushWarning::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRushWarning::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRushWarning::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectXZ"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRushWarning::SetUp_Components(VIBuffer_RectXZ)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_RushWarning"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRushWarning::SetUp_Components(Prototype_Component_Texture_RushWarning)");
		return E_FAIL;
	}
	/* For.Com_MaskTexture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_MaskFallWater"), TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRushWarning::SetUp_Components(Prototype_Component_Texture_MaskFallWater)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRushWarning::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CRushWarning::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CRushWarning::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRushWarning::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRushWarning::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CRushWarning::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pMaskTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 0)))
	{
		MSGBOX("m_pMaskTextureCom->SetUp_OnShader returned E_FAIL in CRushWarning::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_AddNoiseUV", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRushWarning::SetUp_ConstantTable(g_AddNoiseUV)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CRushWarning * CRushWarning::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRushWarning*	pInstance = new CRushWarning(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CRushWarning");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRushWarning::Clone(void * pArg)
{
	CRushWarning*	pInstance = new CRushWarning(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CRushWarning");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRushWarning::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pShaderCom);
}
