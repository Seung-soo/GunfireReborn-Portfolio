#include "stdafx.h"
#include "..\public\ReadyWaterBall.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CReadyWaterBall::CReadyWaterBall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CReadyWaterBall::CReadyWaterBall(const CReadyWaterBall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CReadyWaterBall::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CReadyWaterBall::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 1.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(1600.f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CReadyWaterBall::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CFoxBulletEffect::NativeConstruct");
		return E_FAIL;
	}
	memcpy(&m_tAimDesc, pArg, sizeof(AIMDESC));

	CHierarchyNode* pNode = m_tAimDesc.pModel->Find_HierarchyNode(m_tAimDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tAimDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tAimDesc.pTransform->Get_WorldMatrix();

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	TransformMatrix.r[3] = XMVectorSetW(TransformMatrix.r[3], 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_tAimDesc.pTransform->Get_State(CTransform::STATE_LOOK));
	m_pTransformCom->ArrangeMatrix();

	m_pTransformCom->Scaled(XMVectorSet(3.f, 3.f, 3.f, 0.f));
	return S_OK;
}

_int CReadyWaterBall::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CReadyWaterBall::Tick");
		return DEAD;
	}

	if (true == m_bDead)
		return DEAD;

	m_fAddNoiseUV += (_float)TimeDelta;
	m_fMoveUV -= (_float)TimeDelta * 6.f;

	if (-1.f > m_fMoveUV)
	{
		m_fMoveUV = 1.f;
		m_iCount++;
	}
	if (2 <= m_iCount)
		m_bDead = true;

	return _int();
}

_int CReadyWaterBall::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CReadyWaterBall::LateTick");
		return DEAD;
	}

	m_TransformMatrix = m_tAimDesc.pTransform->Get_WorldMatrix();

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	TransformMatrix.r[3] = XMVectorSetW(TransformMatrix.r[3], 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}




HRESULT CReadyWaterBall::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CReadyWaterBall::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CReadyWaterBall::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 21, i);
	}


	return S_OK;
}

HRESULT CReadyWaterBall::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CReadyWaterBall::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CReadyWaterBall::SetUp_Components(Shader");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_ReadyWaterBall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CReadyWaterBall::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CReadyWaterBall::SetUp_Components(Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CReadyWaterBall::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CReadyWaterBall::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CReadyWaterBall::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CReadyWaterBall::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CReadyWaterBall::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 6)))
	{
		__debugbreak();
		MSGBOX("m_pNoiseTextureCom->SetUp_OnShader returned E_FAIL in CReadyWaterBall::SetUp_ConstantTable");
		return E_FAIL;
	}
	_float fUVY = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CReadyWaterBall::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &fUVY, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CReadyWaterBall::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CReadyWaterBall::SetUp_ConstantTable(g_fMoveNoise_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &fUVY, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CReadyWaterBall::SetUp_ConstantTable(g_fMoveNoise_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_Texture("g_WindowTexture", m_pRendererCom->Get_SRV(TEXT("Target_Caculation")))))
	{
		MSGBOX("m_pShaderCom->Set_Texture returned E_FAIL in CReadyWaterBall::SetUp_ConstantTable(g_WindowTexture)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CReadyWaterBall * CReadyWaterBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CReadyWaterBall*	pInstance = new CReadyWaterBall(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CReadyWaterBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CReadyWaterBall::Clone(void * pArg)
{
	CReadyWaterBall*	pInstance = new CReadyWaterBall(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CReadyWaterBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CReadyWaterBall::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNoiseTextureCom);
}
