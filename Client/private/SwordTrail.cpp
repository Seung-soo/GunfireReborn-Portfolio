#include "stdafx.h"
#include "..\public\SwordTrail.h"
#include "GameInstance.h"

CSwordTrail::CSwordTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CSwordTrail::CSwordTrail(const CSwordTrail & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSwordTrail::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CSwordTrail::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSwordTrail::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 40.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(1000.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CSwordTrail::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CSwordTrail::NativeConstruct");
		return E_FAIL;
	}
	SWORDTRAIL	tSwordTrail;
	memcpy(&tSwordTrail, pArg, sizeof(SWORDTRAIL));

	m_pCamTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
	m_LocalMatrix = XMMatrixRotationAxis(tSwordTrail.vAxis, XMConvertToRadians(tSwordTrail.fAngle)) * XMMatrixTranslation(tSwordTrail.vTranslation.x, tSwordTrail.vTranslation.y, tSwordTrail.vTranslation.z);

	_float4x4	WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_LocalMatrix * m_pCamTransform->Get_WorldMatrix());
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

_int CSwordTrail::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CSwordTrail::Tick");
		return DEAD;
	}

	m_fCurrentSpriteIndexX += _float(TimeDelta) * 5.f;
	if (m_fCurrentSpriteIndexX >= (_float)m_iSpriteMaxXIndex)
		return DEAD;


	return NOPROBLEM;
}

_int CSwordTrail::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CSwordTrail::LateTick");
		return DEAD;
	}

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}


	return _int();
}




HRESULT CSwordTrail::Render()
{


	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CSwordTrail::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 6, i);
	}

	_float4x4	WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_LocalMatrix * m_pCamTransform->Get_WorldMatrix());
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

HRESULT CSwordTrail::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordTrail::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordTrail::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SwordTrail"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordTrail::SetUp_Components(Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSwordTrail::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CSwordTrail::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CSwordTrail::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordTrail::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordTrail::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	_int	iIndexX = (_int)m_fCurrentSpriteIndexX;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fSpriteIndex", &m_fCurrentSpriteIndexX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordTrail::SetUp_ConstantTable(g_iSpriteIndex)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteYIndex", &m_iCurrentSpriteIndexY, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordTrail::SetUp_ConstantTable(g_iSpriteYIndex)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteMaxXIndex", &m_iSpriteMaxXIndex, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordTrail::SetUp_ConstantTable(g_iSpriteMaxXIndex)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteMaxYIndex", &m_iSpriteMaxYIndex, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordTrail::SetUp_ConstantTable(g_iSpriteMaxYIndex)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CSwordTrail * CSwordTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSwordTrail*	pInstance = new CSwordTrail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CSwordTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSwordTrail::Clone(void * pArg)
{
	CSwordTrail*	pInstance = new CSwordTrail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CSwordTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSwordTrail::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
