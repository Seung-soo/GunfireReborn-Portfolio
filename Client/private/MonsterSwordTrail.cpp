#include "stdafx.h"
#include "..\public\MonsterSwordTrail.h"
#include "GameInstance.h"

CMonsterSwordTrail::CMonsterSwordTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CMonsterSwordTrail::CMonsterSwordTrail(const CMonsterSwordTrail & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterSwordTrail::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CMonsterSwordTrail::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterSwordTrail::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 40.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(100.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CMonsterSwordTrail::NativeConstruct");
		return E_FAIL;
	}
	memcpy(&m_tSwordTrail, pArg, sizeof(SWORDTRAIL));

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CMonsterSwordTrail::NativeConstruct");
		return E_FAIL;
	} 

	m_LocalMatrix = m_tSwordTrail.RotationMatrix * XMMatrixTranslation(m_tSwordTrail.vTranslation.x, m_tSwordTrail.vTranslation.y, m_tSwordTrail.vTranslation.z);

	_float4x4	WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_LocalMatrix * m_tSwordTrail.pParentTransform->Get_WorldMatrix());
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_pTransformCom->Scaled(m_tSwordTrail.vTrailScale);

	return S_OK;
}

_int CMonsterSwordTrail::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CMonsterSwordTrail::Tick");
		return DEAD;
	}

	//if (m_fMoveUV <= 0.f)
	//{
	//	if (0.f > m_fDelayTime)
	//		m_bDead = true;
	//	m_fDelayTime -= _float(TimeDelta) * 5.f;
	//}
	//else
	if (-1.f > m_tSwordTrail.fFirstUV || 1.f < m_tSwordTrail.fFirstUV)
		return DEAD;

	m_tSwordTrail.fFirstUV += _float(TimeDelta) * m_tSwordTrail.fTrailSpeed;
	m_tSwordTrail.fFirstUV_Y += _float(TimeDelta) * m_tSwordTrail.fTrailYSpeed;
	m_fAddNoiseUV -= _float(TimeDelta) * 10.f;
	return NOPROBLEM;
}

_int CMonsterSwordTrail::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CMonsterSwordTrail::LateTick");
		return DEAD;
	}

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}


	return _int();
}

HRESULT CMonsterSwordTrail::Render()
{


	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CMonsterSwordTrail::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
		{
			MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CMonsterSwordTrail::SetUp_ConstantTable");
			return E_FAIL;
		}
		if (false == m_tSwordTrail.bUseNoise)
			m_pModelCom->Render(m_pShaderCom, 7, i);
		else
			m_pModelCom->Render(m_pShaderCom, 21, i);
	}

	return S_OK;
}

HRESULT CMonsterSwordTrail::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonsterSwordTrail::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonsterSwordTrail::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tSwordTrail.pModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonsterSwordTrail::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tSwordTrail.pTextureTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonsterSwordTrail::SetUp_Components(Texture)");
		return E_FAIL;
	}

	/* For.Com_NoiseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_NoiseTexture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMonsterSwordTrail::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterSwordTrail::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CMonsterSwordTrail::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CMonsterSwordTrail::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonsterSwordTrail::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonsterSwordTrail::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &m_tSwordTrail.fFirstUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonsterSwordTrail::SetUp_ConstantTable(g_fMoveUV)");
		return E_FAIL;
	}
	_float fMoveUVY = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &m_tSwordTrail.fFirstUV_Y, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonsterSwordTrail::SetUp_ConstantTable(g_fMoveUV)");
		return E_FAIL;
	}
	if (true == m_tSwordTrail.bUseNoise)
	{
		if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 37)))
		{
			MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CMonsterSwordTrail::SetUp_ConstantTable");
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &fMoveUVY, sizeof(_float))))
		{
			MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonsterSwordTrail::SetUp_ConstantTable(g_fMoveNoise_X)");
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &m_fAddNoiseUV, sizeof(_float))))
		{
			MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonsterSwordTrail::SetUp_ConstantTable(g_fMoveNoise_Y)");
			return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CMonsterSwordTrail * CMonsterSwordTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMonsterSwordTrail*	pInstance = new CMonsterSwordTrail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMonsterSwordTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonsterSwordTrail::Clone(void * pArg)
{
	CMonsterSwordTrail*	pInstance = new CMonsterSwordTrail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMonsterSwordTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterSwordTrail::Free()
{
	__super::Free();

	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
