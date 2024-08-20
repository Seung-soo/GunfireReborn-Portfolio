#include "stdafx.h"
#include "..\public\ElementFire.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Player_Hand.h"
#include "Effect.h"
#include "Particle_Effect.h"

CElementFire::CElementFire(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CElementFire::CElementFire(const CElementFire & rhs)
	: CGameObject(rhs)
{
}

HRESULT CElementFire::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CElementFire::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 10.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_matrix pMatrix = *(_matrix*)pArg;
	_float4x4 pFireMatrix;
	XMStoreFloat4x4(&pFireMatrix, pMatrix);


	m_pTransformCom->Set_WorldMatrix(pFireMatrix);
	m_pTransformCom->LookAt(static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION));

	m_pTransformCom->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));


	// 떨어지는 불파티클
	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 7;
	tParticleDesc.fMinSpeed = 0.01f;
	tParticleDesc.fMaxSpeed = 0.02f;
	tParticleDesc.fOriginScale = 0.12f;
	tParticleDesc.fScaleSpeed = 0.02f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 300.f;
	tParticleDesc.fMakeRange = 0.3f;
	tParticleDesc.fMaxDistance = 7.5f;
	tParticleDesc.eDir = CParticle_Effect::DIR_ONE;
	tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
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
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CElementFire::NativeConstruct");
		return E_FAIL;
	}

	return S_OK;
}

_int CElementFire::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{

		//CEffect::EFFECTDESC tEffectDesc;
		//tEffectDesc.bNotLookCamera = false;
		//tEffectDesc.eMove = CEffect::MOVE_STAND;
		//tEffectDesc.eType = CEffect::TYPE_SPRITE;
		//tEffectDesc.eWay = CEffect::WAY_FINAL;
		//tEffectDesc.fSpriteSpeed = 24.f;
		//tEffectDesc.iPass = 20;
		//tEffectDesc.iSpriteNumX = 4;
		//tEffectDesc.iSpriteNumY = 4;
		//tEffectDesc.iSpriteStartX = 0;
		//tEffectDesc.iSpriteStartY = 1;
		//tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
		//tEffectDesc.pDead = nullptr;
		//tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
		//tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_KappaFireBomb");
		//tEffectDesc.vScale = XMVectorSet(4.f, 4.f, 4.f, 0.f);
		//if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		//{
		//	MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CElementFire::Tick");
		//	return DEAD;
		//}
		return DEAD;
	}

	if (0 > __super::Tick(TimeDelta))
		return DEAD;

	if (0 > --m_iLiveTime)
		m_bDead = true;

	//m_fSprite += (_float)TimeDelta * 8.f;
	//if (2.f <= m_fSprite)
	//{
	//	m_fSprite = 0.f;
	//	m_iSpriteY++;
	//	if (2 <= m_iSpriteY)
	//		m_iSpriteY = 0;
	//}
	m_fAddNoiseUV += (_float)TimeDelta * 3.f;


	m_pTransformCom->LookAt(static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Go_Straight(TimeDelta);
	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
		g_pGameInstance->Add_Collider(TEXT("Monster_Bullet"), CCollision_Manager::TARGET_ENEMY, m_pColliderCom);
	return NOPROBLEM;
}

_int CElementFire::LateTick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.bNotLookCamera = false;
		tEffectDesc.eMove = CEffect::MOVE_STAND;
		tEffectDesc.eType = CEffect::TYPE_SPRITE;
		tEffectDesc.eWay = CEffect::WAY_FINAL;
		tEffectDesc.fSpriteSpeed = 24.f;
		tEffectDesc.iPass = 20;
		tEffectDesc.iSpriteNumX = 4;
		tEffectDesc.iSpriteNumY = 4;
		tEffectDesc.iSpriteStartX = 0;
		tEffectDesc.iSpriteStartY = 1;
		tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
		tEffectDesc.pDead = nullptr;
		tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_KappaFireBomb");
		tEffectDesc.vScale = XMVectorSet(4.f, 4.f, 4.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CElementFire::LateTick");
			return DEAD;
		}
		return DEAD;
	}

	if (0 > __super::LateTick(TimeDelta))
		return DEAD;


	if (true == g_pGameInstance->Collision_Sphere(m_pColliderCom, (CCollider*)g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider"))))
	{
		static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Hit(50.f);
		m_bDead = true;
	}


	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return NOPROBLEM;
}




HRESULT CElementFire::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CElementFire::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 25)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CElementFire::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CElementFire::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CElementFire::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CElementFire::SetUp_Components(Shader");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CElementFire::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_1, TEXT("Prototype_Component_Texture_KappaFire"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CElementFire::SetUp_Components(Texture)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture_Sour"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CElementFire::SetUp_Components(Texture_Noise)");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CElementFire::SetUp_Components(Collider)");
		return E_FAIL;
	}
	m_pColliderCom->Set_Parent(this);

	return S_OK;
}

HRESULT CElementFire::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CElementFire::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CElementFire::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CElementFire::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CElementFire::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CElementFire::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 36)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CElementFire::SetUp_ConstantTable");
		return E_FAIL;
	}

	//_int iSprite = (_int)m_fSprite;


	//if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteIndex", &iSprite, sizeof(_int))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CElementFire::SetUp_ConstantTable(g_iSpriteIndex)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteYIndex", &m_iSpriteY, sizeof(_int))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CElementFire::SetUp_ConstantTable(g_iSpriteYIndex)");
	//	return E_FAIL;
	//}
	if (FAILED(m_pShaderCom->Set_RawValue("g_AddNoiseUV", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CElementFire::SetUp_ConstantTable(g_AddNoiseUV)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CElementFire * CElementFire::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CElementFire*	pInstance = new CElementFire(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CElementFire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CElementFire::Clone(void * pArg)
{
	CElementFire*	pInstance = new CElementFire(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CElementFire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CElementFire::Free()
{
	__super::Free();

	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
