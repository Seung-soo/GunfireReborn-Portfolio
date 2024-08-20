#include "stdafx.h"
#include "..\public\YachaBullet.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Player_Hand.h"
#include "MonsterWave.h"
#include "Monster.h"
#include "Particle_Effect.h"

CYachaBullet::CYachaBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CYachaBullet::CYachaBullet(const CYachaBullet & rhs)
	: CGameObject(rhs)
{
}

void CYachaBullet::Set_Manipulation(CGameObject* pTarget)
{
	m_bManipulation = true;
	m_pTarget = pTarget;
	Safe_AddRef(m_pTarget);
	m_vDir = XMVector4Normalize(XMVectorSetY(static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, -m_vDir);
	m_pTransformCom->ArrangeMatrix();
}

HRESULT CYachaBullet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CYachaBullet::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_matrix pYachaMatrix = *(_matrix*)pArg;
	_float4x4 pFireMatrix;
	XMStoreFloat4x4(&pFireMatrix, pYachaMatrix);
	//pFireMatrix._42 += 1.f;

	m_pTransformCom->Set_WorldMatrix(pFireMatrix);
	m_vDir = XMVector4Normalize(XMVectorSetY(static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, -m_vDir);
	m_pTransformCom->ArrangeMatrix();
	m_iPass = 8;
	m_pTransformCom->Scaled(XMVectorSet(1.5f, 4.f, 4.f, 0.f));


	// 물튀기기 파티클
	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) + XMVectorSet(0.f, 0.5f, 0.f, 0.f));

	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 9;
	tParticleDesc.fMinSpeed = 0.01f;
	tParticleDesc.fMaxSpeed = 0.02f;
	tParticleDesc.fOriginScale = 0.36f;
	tParticleDesc.fScaleSpeed = 0.19f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 100.f;
	tParticleDesc.fMakeRange = 2.f;
	tParticleDesc.fMaxDistance = 5.4f;
	tParticleDesc.eDir = CParticle_Effect::DIR_ONE;
	tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	tParticleDesc.pParentPosition = &m_vPosition;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead = &m_bDead;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CYachaBullet::NativeConstruct");
		return E_FAIL;
	}

	return S_OK;
}

_int CYachaBullet::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (0 > --m_iLiveTime)
		m_bDead = true;

	m_pTransformCom->Go_Vector(m_vDir, TimeDelta * 2.f);
	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	m_fMoveUV += _float(TimeDelta) * 6.f;

	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

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

	return _int();
}

_int CYachaBullet::LateTick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::LateTick(TimeDelta))
		return -1;

	if (false == m_bManipulation)
	{
		if (true == g_pGameInstance->Collision_Sphere(m_pColliderCom, (CCollider*)g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider"))))
		{
			static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Hit(70.f);
			m_bDead = true;
		}
	}
	else
	{
		auto ColliderList = g_pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
		if (nullptr != ColliderList)
		{
			for (auto pCollider : *ColliderList)
			{
				if (static_cast<CCollider*>(m_pTarget->Get_Component(TEXT("Com_Collider_Body"))) == pCollider)
					continue;
				if (true == g_pGameInstance->Collision_Sphere(m_pColliderCom, pCollider))
				{
					static_cast<CMonster*>(pCollider->Get_Parent())->Hit(70.f, 1.f);
					m_bDead = true;
				}
			}
		}
	}

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG	
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif // _DEBUG

	}
	return _int();
}




HRESULT CYachaBullet::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CYachaBullet::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i);
	}

	return S_OK;
}

HRESULT CYachaBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CYachaBullet::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CYachaBullet::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_YachaWaterAttack"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CYachaBullet::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CYachaBullet::SetUp_Components(Texture");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CYachaBullet::SetUp_Components(Collider)");
		return E_FAIL;
	}
	m_pColliderCom->Set_Parent(this);

	return S_OK;
}

HRESULT CYachaBullet::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CYachaBullet::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CYachaBullet::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CYachaBullet::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CYachaBullet::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CYachaBullet::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CYachaBullet::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonsterSwordTrail::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CYachaBullet * CYachaBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CYachaBullet*	pInstance = new CYachaBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CYachaBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CYachaBullet::Clone(void * pArg)
{
	CYachaBullet*	pInstance = new CYachaBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CYachaBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CYachaBullet::Free()
{
	__super::Free();

	Safe_Release(m_pTarget);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
