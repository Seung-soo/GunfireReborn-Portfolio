#include "stdafx.h"
#include "..\public\MonkSmallEffect.h"
#include "GameInstance.h"

CMonkSmallEffect::CMonkSmallEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CMonkSmallEffect::CMonkSmallEffect(const CMonkSmallEffect & rhs)
	: CGameObject(rhs)
{
}

void CMonkSmallEffect::Set_Disappear()
{
	m_iPass = 16;
	m_fMoveUV -= _float(_int(m_fMoveUV));
}

HRESULT CMonkSmallEffect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonkSmallEffect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 1.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 0.5f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CTransform*	pMonkTransform = (CTransform*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pMonkTransform->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

_int CMonkSmallEffect::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		return DEAD;
	}
	if (16 == m_iPass && 1.f < m_fMoveUV)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
		return -1;

	m_pTransformCom->Scaling(-TimeDelta);

	//if (true == g_pGameInstance->Get_KeyEnter(DIK_RIGHT))
	//{
	//	m_iTexIndex++;
	//	if (m_iTexIndex > 98)
	//		m_iTexIndex = 0;
	//}
	//else if (true == g_pGameInstance->Get_KeyEnter(DIK_LEFT))
	//{
	//	m_iTexIndex--;
	//	if (m_iTexIndex < 0)
	//		m_iTexIndex = 98;
	//}

	m_fMoveNoise += (_float)TimeDelta * 1.f;
	m_fMoveUV += (_float)TimeDelta * 2.f;

	return _int();
}

_int CMonkSmallEffect::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}

	return _int();
}




HRESULT CMonkSmallEffect::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CMonkSmallEffect::Render");
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

HRESULT CMonkSmallEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonkSmallEffect::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_ModelShader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonkSmallEffect::SetUp_Components(Shader");
		return E_FAIL;
	}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture_Sour"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonkSmallEffect::SetUp_Components(Texture");
		return E_FAIL;
	}
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_1, TEXT("Prototype_Component_Model_MonkSmallEffect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonkSmallEffect::SetUp_Components(Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonkSmallEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CMonkSmallEffect::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CMonkSmallEffect::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CMonkSmallEffect::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CMonkSmallEffect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 2)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CMonkSmallEffect::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &m_fMoveNoise, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonkSmallEffect::SetUp_ConstantTable(g_AddNoiseUV)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonkSmallEffect::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CMonkSmallEffect * CMonkSmallEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMonkSmallEffect*	pInstance = new CMonkSmallEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMonkSmallEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonkSmallEffect::Clone(void * pArg)
{
	CMonkSmallEffect*	pInstance = new CMonkSmallEffect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMonkSmallEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonkSmallEffect::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
