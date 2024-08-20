#include "stdafx.h"
#include "..\public\GhostBlinkEffect.h"
#include "GameInstance.h"

CGhostBlinkEffect::CGhostBlinkEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CGhostBlinkEffect::CGhostBlinkEffect(const CGhostBlinkEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGhostBlinkEffect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CGhostBlinkEffect::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGhostBlinkEffect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 40.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CGhostBlinkEffect::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CGhostBlinkEffect::NativeConstruct");
		return E_FAIL;
	}

	m_pGhostMatrix = (_float4x4*)pArg;
	_matrix GhostWorldMatrix = XMLoadFloat4x4(m_pGhostMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, GhostWorldMatrix.r[3]);

	return S_OK;
}

_int CGhostBlinkEffect::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CGhostBlinkEffect::Tick");
		return DEAD;
	}
	if (false == m_bEffectDown)
	{
		m_fMoveUV_X -= _float(TimeDelta) * 10.f;
		m_fMoveUV_Y += _float(TimeDelta) * 5.f;
		if (1.f <= m_fMoveUV_Y)
			m_fMoveUV_Y = 1.f;
	}
	else
	{

		m_fMoveUV_X += _float(TimeDelta) * 10.f;
		m_fMoveUV_Y -= _float(TimeDelta) * 5.f;
	}
	if (m_fMoveUV_Y <= -1.2f)
		return DEAD;

	_matrix GhostWorldMatrix = XMLoadFloat4x4(m_pGhostMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, GhostWorldMatrix.r[3]);


	return NOPROBLEM;
}

_int CGhostBlinkEffect::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CGhostBlinkEffect::LateTick");
		return DEAD;
	}

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}


	return _int();
}




HRESULT CGhostBlinkEffect::Render()
{


	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CGhostBlinkEffect::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 12, i);
	}

	return S_OK;
}

HRESULT CGhostBlinkEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CGhostBlinkEffect::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CGhostBlinkEffect::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_GhostBlinkEffect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CGhostBlinkEffect::SetUp_Components(Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGhostBlinkEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CGhostBlinkEffect::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CGhostBlinkEffect::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CGhostBlinkEffect::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CGhostBlinkEffect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &m_fMoveUV_X, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CGhostBlinkEffect::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &m_fMoveUV_Y, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CGhostBlinkEffect::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CGhostBlinkEffect * CGhostBlinkEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGhostBlinkEffect*	pInstance = new CGhostBlinkEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CGhostBlinkEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGhostBlinkEffect::Clone(void * pArg)
{
	CGhostBlinkEffect*	pInstance = new CGhostBlinkEffect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CGhostBlinkEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGhostBlinkEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
