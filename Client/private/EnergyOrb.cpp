#include "stdafx.h"
#include "..\public\EnergyOrb.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Effect.h"
#include "Monster.h"
#include "Damage.h"
#include "Player_Hand.h"

CEnergyOrb::CEnergyOrb(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CEnergyOrb::CEnergyOrb(const CEnergyOrb & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnergyOrb::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CEnergyOrb::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnergyOrb::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 40.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(1000.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CEnergyOrb::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CEnergyOrb::NativeConstruct");
		return E_FAIL;
	}

	m_vDir = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, g_pGameInstance->Get_CamPosition() + m_vDir);

	CEffect::EFFECTDESC tEffectDesc;
	tEffectDesc.eType = CEffect::TYPE_2D;
	tEffectDesc.eMove = CEffect::MOVE_CHASE;
	tEffectDesc.iPass = 0;
	tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
	tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
	tEffectDesc.vScale = XMVectorSet(1.5f, 1.5f, 1.f, 0.f);
	tEffectDesc.pDead = &m_bDead;
	tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_LightCircle");
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CEnergyOrb::NativeConstruct");
		return E_FAIL;
	}

	m_tEffectDesc.eMove = CEffect::MOVE_STAND;
	m_tEffectDesc.eType = CEffect::TYPE_SPRITE;
	m_tEffectDesc.eWay = CEffect::WAY_FINAL;
	m_tEffectDesc.fSpriteSpeed = 24.f;
	m_tEffectDesc.iPass = 20;
	m_tEffectDesc.iSpriteNumX = 4;
	m_tEffectDesc.iSpriteNumY = 3;
	m_tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 20 - 10) / 20.f, _float(rand() % 20 - 10) / 20.f, 0.f);
	m_tEffectDesc.pDead = &m_bDead;
	m_tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
	m_tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_EnergyOrb_Lightning");
	m_tEffectDesc.vScale = XMVectorSet(2.4f, 3.2f, 1.f, 0.f);

	if (true == static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_Talents()[5])
		m_fFreezeTime += 3;

	return S_OK;
}

_int CEnergyOrb::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CEnergyOrb::Tick");
		return DEAD;
	}

	if (0 > --m_iLiveTime)
		m_bDead = true;

	m_fLightningTime += (_float)TimeDelta;
	if (0.1f < m_fLightningTime)
	{
		m_tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 20 - 10) / 20.f, _float(rand() % 20 - 10) / 20.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &m_tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CEnergyOrb::Tick");
			return E_FAIL;
		}
		m_fLightningTime = 0.f;
	}

	if (false == m_bExplotion)
	{
		m_pTransformCom->Go_Vector(m_vDir, TimeDelta);
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -TimeDelta);
	}

	if (false == m_bCollision)
	{
		auto MonsterWeakColliderList = g_pGameInstance->Get_ColliderList(TEXT("Weakness"), CCollision_Manager::TARGET_ENEMY);
		if (nullptr != MonsterWeakColliderList)
			for (auto& pMonsterCollider : *MonsterWeakColliderList)
			{
				if (true == g_pGameInstance->Collision_Sphere(m_pColliderCom, pMonsterCollider))
				{
					if (false == m_bExplotion)
					{
						m_bExplotion = true;
						Explotion();
						if (false == m_bBigCollider)
						{
							m_bBigCollider = true;
							CCollider::COLLIDERDESC tColliderDesc;
							tColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
							m_pColliderCom->Set_ColliderDesc(tColliderDesc);
							m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());
						}
					}
				}
			}

		auto MonsterBodyColliderList = g_pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
		if (nullptr != MonsterBodyColliderList)
			for (auto& pMonsterCollider : *MonsterBodyColliderList)
			{
				if (true == g_pGameInstance->Collision_Sphere(m_pColliderCom, pMonsterCollider))
				{
					if (false == m_bExplotion)
					{
						m_bExplotion = true;
						Explotion();
						if (false == m_bBigCollider)
						{
							m_bBigCollider = true;
							CCollider::COLLIDERDESC tColliderDesc;
							tColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
							m_pColliderCom->Set_ColliderDesc(tColliderDesc);
							m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());
						}
					}
					else
					{
						CDamage::NUMBERDESC tNumberDesc;
						tNumberDesc.eType = CDamage::TYPE_ORIGIN;
						tNumberDesc.iDamage = static_cast<CMonster*>(pMonsterCollider->Get_Parent())->Hit(600.f, 1.f);
						tNumberDesc.vPosition = static_cast<CTransform*>(pMonsterCollider->Get_Parent()->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.3f, 0.f, 0.f);

						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CEnergyOrb::Tick");
							return E_FAIL;
						}
						
						static_cast<CMonster*>(pMonsterCollider->Get_Parent())->Freeze(m_fFreezeTime);
						if (LEVEL_STAGE3_2 == g_eCurrentLevel || LEVEL_STAGE4_2 == g_eCurrentLevel)
							break;
						m_bCollision = true;

					}
				}
			}
	}
	

	if (true == m_bExplotion)
	{
		m_fScale += 48 * (_float)TimeDelta;
		m_pTransformCom->Scaled(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));
		if (7.f < m_fScale)
			m_bDead = true;
	}

	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	return NOPROBLEM;
}

_int CEnergyOrb::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CEnergyOrb::LateTick");
		return DEAD;
	}

	if (LEVEL_STAGE3_2 == g_eCurrentLevel && 3.f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_bDead = true;
	else if (LEVEL_STAGE4_2 == g_eCurrentLevel && 41.5f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_bDead = true;

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return _int();
}




HRESULT CEnergyOrb::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CEnergyOrb::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 5, i);
	}

	return S_OK;
}

HRESULT CEnergyOrb::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CEnergyOrb::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CEnergyOrb::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EnergyOrb"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CEnergyOrb::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CEnergyOrb::SetUp_Components(Collider)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEnergyOrb::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CEnergyOrb::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CEnergyOrb::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CEnergyOrb::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CEnergyOrb::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEnergyOrb::Explotion()
{
	Safe_Release(m_pModelCom);

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_OrbExplotion"), TEXT("Com_Model_Explotion"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CEnergyOrb::Explotion(Model)");
		return E_FAIL;
	}
	m_tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 20 - 10) / 5.f, _float(rand() % 20 - 5) / 5.f, -5.f);
	m_tEffectDesc.vScale = XMVectorSet(7.2f, 9.6f, 1.f, 0.f);
	m_tEffectDesc.iSpriteStartX = 0;
	m_tEffectDesc.iSpriteStartY = 0;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &m_tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CEnergyOrb::Tick");
		return E_FAIL;
	}
	m_tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 20 - 10) / 5.f, _float(rand() % 20 - 5) / 5.f, -5.f);
	m_tEffectDesc.iSpriteStartX = 0;
	m_tEffectDesc.iSpriteStartY = 1;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &m_tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CEnergyOrb::Tick");
		return E_FAIL;
	}
	m_tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 20 - 10) / 5.f, _float(rand() % 20 - 5) / 5.f, -5.f);
	m_tEffectDesc.iSpriteStartX = 2;
	m_tEffectDesc.iSpriteStartY = 1;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &m_tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CEnergyOrb::Tick");
		return E_FAIL;
	}
	m_tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 20 - 10) / 5.f, _float(rand() % 20 - 5) / 5.f, -5.f);
	m_tEffectDesc.iSpriteStartX = 0;
	m_tEffectDesc.iSpriteStartY = 2;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &m_tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CEnergyOrb::Tick");
		return E_FAIL;
	}
	m_tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 20 - 10) / 5.f, _float(rand() % 20 - 5) / 5.f, -5.f);
	m_tEffectDesc.iSpriteStartX = 0;
	m_tEffectDesc.iSpriteStartY = 3;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &m_tEffectDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CEnergyOrb::Tick");
		return E_FAIL;
	}

	m_pTransformCom->Scaled(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));

	return S_OK;
}



CEnergyOrb * CEnergyOrb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEnergyOrb*	pInstance = new CEnergyOrb(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CEnergyOrb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnergyOrb::Clone(void * pArg)
{
	CEnergyOrb*	pInstance = new CEnergyOrb(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CEnergyOrb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnergyOrb::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
