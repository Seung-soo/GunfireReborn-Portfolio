#include "stdafx.h"
#include "..\public\Particle.h"
#include "GameInstance.h"

CParticle::CParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CParticle::CParticle(const CParticle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticle::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CParticle::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CParticle::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;
	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CParticle::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CParticle::NativeConstruct");
		return E_FAIL;
	}

	m_pTargetTransform = (CTransform*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 10.f, 0.f, 0.f));

	return S_OK;
}

_int CParticle::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CParticle::Tick");
		return DEAD;
	}

	return NOPROBLEM;
}

_int CParticle::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CParticle::LateTick");
		return DEAD;
	}

	m_pVIBufferCom->Update(TimeDelta);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 10.f, 0.f, 0.f));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return _int();
}

HRESULT CParticle::Render()
{
	if (nullptr == m_pVIBufferCom)
	{
		MSGBOX("nullptr == m_pVIBufferCom in CParticle::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CParticle::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CParticle::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CParticle::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_PointInstance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CParticle::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/*For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_Snow"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CParticle::SetUp_Components(Texture)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	CVIBuffer_Point_Instance::PARTICLEDESC		ParticleDesc;

	ParticleDesc.fMinSpeed = 2.f;
	ParticleDesc.fMaxSpeed = 10.f;
	ParticleDesc.vMoveDir = _float3(1.f, -1.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_SideDrop"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &ParticleDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CParticle::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CParticle::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CParticle::SetUp_ConstantTable");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle::SetUp_ConstantTable(View)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CParticle::SetUp_ConstantTable");
		return E_FAIL;
	}

	_float4		vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle::SetUp_ConstantTable(g_vCamPosition)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}



CParticle * CParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CParticle*	pInstance = new CParticle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CParticle::Clone(void * pArg)
{
	CParticle*	pInstance = new CParticle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticle::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
