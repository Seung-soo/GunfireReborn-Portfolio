#include "stdafx.h"
#include "..\public\MonkAttack.h"
#include "GameInstance.h"

CMonkAttack::CMonkAttack(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CMonkAttack::CMonkAttack(const CMonkAttack & rhs)
	: CGameObject(rhs)
{
}
//193 48 48
HRESULT CMonkAttack::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonkAttack::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 1.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CMonkAttack::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CMonkAttack::NativeConstruct");
		return E_FAIL;
	}

	_vector	vPos;
	memcpy(&vPos, pArg, sizeof(_float4));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_iTexIndex = 93;
	m_pTransformCom->Scaled(XMVectorSet(2.1f, 2.1f, 2.1f, 0.f));
	return S_OK;
}

_int CMonkAttack::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CMonkAttack::Tick");
		return DEAD;
	}

	if (true == m_bDead)
		return DEAD;

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

	m_fAddNoiseUV += (_float)TimeDelta * 2.f;


	return _int();
}

_int CMonkAttack::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CMonkAttack::LateTick");
		return DEAD;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}




HRESULT CMonkAttack::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CMonkAttack::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CMonkAttack::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 8, i);
	}


	return S_OK;
}

HRESULT CMonkAttack::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonkAttack::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonkAttack::SetUp_Components(Shader");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_1, TEXT("Prototype_Component_Model_MonkAttack"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonkAttack::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonkAttack::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonkAttack::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CMonkAttack::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CMonkAttack::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonkAttack::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonkAttack::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CMonkAttack::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonkAttack::SetUp_ConstantTable(NoiseUV)");
		return E_FAIL;
	}
	
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CMonkAttack * CMonkAttack::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMonkAttack*	pInstance = new CMonkAttack(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMonkAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonkAttack::Clone(void * pArg)
{
	CMonkAttack*	pInstance = new CMonkAttack(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMonkAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonkAttack::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
