#include "stdafx.h"
#include "..\public\KappaFire.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Particle_Effect.h"

CKappaFire::CKappaFire(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CKappaFire::CKappaFire(const CKappaFire & rhs)
	: CGameObject(rhs)
{
}

HRESULT CKappaFire::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CKappaFire::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CKappaFire::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CKappaFire::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CKappaFire::NativeConstruct");
		return E_FAIL;
	}

	_matrix pKappaMatrix = *(_matrix*)pArg;
	_float4x4 pFireMatrix;
	XMStoreFloat4x4(&pFireMatrix, pKappaMatrix);

	m_pTransformCom->Set_WorldMatrix(pFireMatrix);

	m_pTransformCom->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));

	m_iTexIndex = 37;

	// 올라오는 불파티클
	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 7;
	tParticleDesc.fMinSpeed = 0.09f;
	tParticleDesc.fMaxSpeed = 0.24f;
	tParticleDesc.fOriginScale = 0.12f;
	tParticleDesc.fScaleSpeed = 0.f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 300.f;
	tParticleDesc.fMakeRange = 0.3f;
	tParticleDesc.fMaxDistance = 7.5f;
	tParticleDesc.eDir = CParticle_Effect::DIR_ONE;
	tParticleDesc.vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	tParticleDesc.pParentPosition = &m_vPosition;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.fTrailInterval = 0.1f;
	tParticleDesc.pDead = &m_bDead;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CKappaFire::NativeConstruct");
		return E_FAIL;
	}

	return S_OK;
}

_int CKappaFire::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		return DEAD;
	}
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CKappaFire::Tick");
		return E_FAIL;
	}
	//m_fSprite += (_float)TimeDelta * 8.f;
	//if (2.f <= m_fSprite)
	//{
	//	m_fSprite = 0.f;
	//	m_iSpriteY++;
	//	if (2 <= m_iSpriteY)
	//		m_iSpriteY = 0;
	//}

	if (true == g_pGameInstance->Get_KeyEnter(DIK_RIGHT))
	{
		m_iTexIndex++;
		if (m_iTexIndex > 98)
			m_iTexIndex = 0;
	}
	else if (true == g_pGameInstance->Get_KeyEnter(DIK_LEFT))
	{
		m_iTexIndex--;
		if (m_iTexIndex < 0)
			m_iTexIndex = 98;
	}

	m_fAddNoiseUV += (_float)TimeDelta * 3.f;
	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));



	return _int();
}

_int CKappaFire::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CKappaFire::LateTick");
		return E_FAIL;
	}


	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}


	return _int();
}




HRESULT CKappaFire::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CKappaFire::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 25)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CKappaFire::Render");
		return E_FAIL;
	}

	return S_OK;
}

void CKappaFire::Go_Fire(CElementFire::FIRETYPE eFireType)
{
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_ElementFire"), TEXT("Prototype_GameObject_ElementFire"), &m_pTransformCom->Get_WorldMatrix())))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CKappaFire::Go_Fire");
		return;
	}
	static_cast<CElementFire*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_ElementFire")))->Set_FireType(eFireType);

	m_bDead = true;
}

HRESULT CKappaFire::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CKappaFire::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CKappaFire::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CKappaFire::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_1, TEXT("Prototype_Component_Texture_KappaFire"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CKappaFire::SetUp_Components(Texture)");
		return E_FAIL;
	}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture_Sour"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CKappaFire::SetUp_Components(Texture)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CKappaFire::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CKappaFire::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CKappaFire::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CKappaFire::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CKappaFire::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CKappaFire::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CKappaFire::SetUp_ConstantTable");
		return E_FAIL;
	}
	//_int iSprite = (_int)m_fSprite;


	//if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteIndex", &iSprite, sizeof(_int))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CKappaFire::SetUp_ConstantTable(g_iSpriteIndex)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteYIndex", &m_iSpriteY, sizeof(_int))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CKappaFire::SetUp_ConstantTable(g_iSpriteYIndex)");
	//	return E_FAIL;
	//}
	if (FAILED(m_pShaderCom->Set_RawValue("g_AddNoiseUV", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CKappaFire::SetUp_ConstantTable(g_AddNoiseUV)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CKappaFire * CKappaFire::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CKappaFire*	pInstance = new CKappaFire(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CKappaFire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CKappaFire::Clone(void * pArg)
{
	CKappaFire*	pInstance = new CKappaFire(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CKappaFire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKappaFire::Free()
{
	__super::Free();

	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
