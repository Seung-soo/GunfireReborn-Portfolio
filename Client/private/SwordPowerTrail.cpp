#include "stdafx.h"
#include "..\public\SwordPowerTrail.h"
#include "GameInstance.h"
#include "Player_Hand.h"
#include "HierarchyNode.h"
#include "PoleMonarch.h"

CSwordPowerTrail::CSwordPowerTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CSwordPowerTrail::CSwordPowerTrail(const CSwordPowerTrail & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSwordPowerTrail::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CSwordPowerTrail::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSwordPowerTrail::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CSwordPowerTrail::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CSwordPowerTrail::NativeConstruct");
		return E_FAIL;
	}
	m_pSwordPower = (CGameObject*)pArg;
	m_pSwordPowerTransform = static_cast<CTransform*>(m_pSwordPower->Get_Component(TEXT("Com_Transform")));

	m_pTransformCom->Set_WorldMatrix(m_pSwordPowerTransform->Get_WorldFloat4x4());
	m_pTransformCom->Scaled(XMVectorSet(1.f, 1.f, m_fScale, 0.f));
	return S_OK;
}

_int CSwordPowerTrail::Tick(_double dTimeDelta)
{
	if (true == m_pSwordPower->Get_Dead())
		return DEAD;
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CSwordPowerTrail::Tick");
		return -1;
	}

	m_fAddNoiseUV -= (_float)dTimeDelta * 0.5f;

	if (1.f > m_fScale)
	{
		m_fScale += (_float)dTimeDelta * 2.f;
		if (1.f < m_fScale)
			m_fScale = 1.f;
	}
	return _int();
}

_int CSwordPowerTrail::LateTick(_double dTimeDelta)
{
	if (true == m_pSwordPower->Get_Dead())
		return DEAD;
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CSwordPowerTrail::LateTick");
		return -1;
	}

	m_pTransformCom->Set_WorldMatrix(m_pSwordPowerTransform->Get_WorldFloat4x4());

	// ÀýµÎÃ¼
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRadius))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}

	return _int();
}




HRESULT CSwordPowerTrail::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CSwordPowerTrail::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CSwordPowerTrail::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 23, i);
	}

	return S_OK;
}

HRESULT CSwordPowerTrail::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordPowerTrail::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordPowerTrail::SetUp_Components(Shader)");
		return E_FAIL;
	}


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_SwordPowerTrail"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordPowerTrail::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_NoiseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordPowerTrail::SetUp_Components(Texture)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CSwordPowerTrail::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CSwordPowerTrail::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CSwordPowerTrail::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPowerTrail::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPowerTrail::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 74)))
	{
		MSGBOX("m_pNoiseTextureCom->SetUp_OnShader returned E_FAIL in CSwordPowerTrail::SetUp_ConstantTable(DiffuseTexture)");
		return E_FAIL;
	}


	_float	fMoveUVX = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPowerTrail::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPowerTrail::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPowerTrail::SetUp_ConstantTable(g_fMoveNoise_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPowerTrail::SetUp_ConstantTable(g_fMoveNoise_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_Texture("g_WindowTexture", m_pRendererCom->Get_SRV(TEXT("Target_Caculation")))))
	{
		MSGBOX("m_pShaderCom->Set_Texture returned E_FAIL in CSwordPowerTrail::SetUp_ConstantTable(g_WindowTexture)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CSwordPowerTrail * CSwordPowerTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSwordPowerTrail*	pInstance = new CSwordPowerTrail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CSwordPowerTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSwordPowerTrail::Clone(void * pArg)
{
	CSwordPowerTrail*	pInstance = new CSwordPowerTrail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CSwordPowerTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSwordPowerTrail::Free()
{
	__super::Free();

	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
