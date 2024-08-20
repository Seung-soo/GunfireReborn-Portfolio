#include "stdafx.h"
#include "..\public\RockWind.h"
#include "GameInstance.h"

CRockWind::CRockWind(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CRockWind::CRockWind(const CRockWind & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRockWind::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CRockWind::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRockWind::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CRockWind::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CRockWind::NativeConstruct");
		return E_FAIL;
	}

	memcpy(&m_tRockWindDesc, pArg, sizeof(ROCKWINDDESC));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tRockWindDesc.pRockWindTransform->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(m_tRockWindDesc.vDir));
	m_pTransformCom->ArrangeMatrix();
	
	m_pTransformCom->Scaled(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));
	return S_OK;
}

_int CRockWind::Tick(_double dTimeDelta)
{
	if (true == *m_tRockWindDesc.pDead)
		return DEAD;
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CRockWind::Tick");
		return -1;
	}

	m_fMoveUV += (_float)dTimeDelta * 5.f;

	return _int();
}

_int CRockWind::LateTick(_double dTimeDelta)
{
	if (true == *m_tRockWindDesc.pDead)
		return DEAD;
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CRockWind::LateTick");
		return -1;
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tRockWindDesc.pRockWindTransform->Get_State(CTransform::STATE_POSITION));

	// ÀýµÎÃ¼
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}

	return _int();
}




HRESULT CRockWind::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CRockWind::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CRockWind::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (0.f > m_fMoveUV)
			m_pModelCom->Render(m_pShaderCom, 12, i);
		else
			m_pModelCom->Render(m_pShaderCom, 26, i);
	}

	return S_OK;
}

HRESULT CRockWind::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRockWind::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRockWind::SetUp_Components(Shader)");
		return E_FAIL;
	}


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_RockWind"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRockWind::SetUp_Components(Model)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CRockWind::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CRockWind::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CRockWind::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRockWind::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRockWind::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}


	_float	fMoveUVX = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRockWind::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRockWind::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CRockWind * CRockWind::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRockWind*	pInstance = new CRockWind(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CRockWind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRockWind::Clone(void * pArg)
{
	CRockWind*	pInstance = new CRockWind(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CRockWind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRockWind::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
