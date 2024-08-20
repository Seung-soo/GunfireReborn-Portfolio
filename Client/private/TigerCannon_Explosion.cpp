#include "stdafx.h"
#include "..\public\TigerCannon_Explosion.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Damage.h"
#include "PoleMonarch.h"

CTigerCannon_Explosion::CTigerCannon_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CTigerCannon_Explosion::CTigerCannon_Explosion(const CTigerCannon_Explosion & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTigerCannon_Explosion::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CTigerCannon_Explosion::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTigerCannon_Explosion::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CTigerCannon_Explosion::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CTigerCannon_Explosion::NativeConstruct");
		return E_FAIL;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4((_float4*)pArg));

	m_pTransformCom->Scaled(XMVectorSet(12.f, 12.f, 12.f, 0.f));

	g_pGameInstance->StopSound(CSoundMgr::EFFECT1);
	g_pGameInstance->LetsPlaySound(TEXT("TigerCannonBomb.mp3"), CSoundMgr::EFFECT1, 0.6f);

	return S_OK;
}

_int CTigerCannon_Explosion::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CTigerCannon_Explosion::Tick");
		return E_FAIL;
	}

	m_fSprite += (_float)TimeDelta * 32.f;
	if (4.f <= m_fSprite)
	{
		m_fSprite = 0.f;
		m_iSpriteY++;
		if (4 <= m_iSpriteY)
			return DEAD;
	}

	_matrix TransformMatrix = m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pColliderCom->Update_Transform(TransformMatrix);

	return _int();
}

_int CTigerCannon_Explosion::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CTigerCannon_Explosion::LateTick");
		return E_FAIL;
	}

	m_iCollCount++;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (m_iCollCount < 5)
	{

		list<CCollider*>* pWeakColliderList = pGameInstance->Get_ColliderList(TEXT("Weakness"), CCollision_Manager::TARGET_ENEMY);
		list<CCollider*>* pBodyColliderList = pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
		list<CCollider*>* pBulletColliderList = pGameInstance->Get_ColliderList(TEXT("Monster_Bullet"), CCollision_Manager::TARGET_ENEMY);

		if (nullptr != pBulletColliderList)
		{
			for (auto& pBulletCollider : *pBulletColliderList)
			{
				_float Dist = 0.f;
				if (true == pGameInstance->Collision_RayToSphere(g_pGameInstance->Get_CamPosition(), static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK), pBulletCollider, &Dist))
				{
					pBulletCollider->Get_Parent()->Set_Dead(true);
				}
			}
		}
		if (nullptr != pWeakColliderList && nullptr != pBodyColliderList)
		{
			for (auto& pWeakCollider : *pWeakColliderList)
			{
				_bool bColl = false;
				for (auto& pMonsterCollision : m_vCollisionMonster)
				{
					if (pMonsterCollision == pWeakCollider->Get_Parent())
					{
						bColl = true;
						break;
					}
				}
				if (true == bColl)
					continue;
				if (true == pGameInstance->Collision_Sphere(m_pColliderCom, pWeakCollider))
				{
					CDamage::NUMBERDESC tNumberDesc;
					tNumberDesc.eType = CDamage::TYPE_CRITICAL;
					tNumberDesc.iDamage = static_cast<CMonster*>(pWeakCollider->Get_Parent())->Hit(500.f, 4.f, true);
					tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

					if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pWeakCollider->Get_Parent())->Get_Strong()))
						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTigerCannon_Explosion::LateTick");
							return DEAD;
						}
					m_vCollisionMonster.push_back(static_cast<CMonster*>(pWeakCollider->Get_Parent()));
					m_bDead = true;
				}
			}

			for (auto& pBodyCollider : *pBodyColliderList)
			{
				_bool bColl = false;
				for (auto& pMonsterCollision : m_vCollisionMonster)
				{
					if (pMonsterCollision == pBodyCollider->Get_Parent())
					{
						bColl = true;
						break;
					}
				}
				if (true == bColl)
					continue;
				if (true == pGameInstance->Collision_Sphere(m_pColliderCom, pBodyCollider))
				{
					CDamage::NUMBERDESC tNumberDesc;
					tNumberDesc.eType = CDamage::TYPE_ORIGIN;
					tNumberDesc.iDamage = static_cast<CMonster*>(pBodyCollider->Get_Parent())->Hit(500.f, 1.f, true);
					tNumberDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

					if (!(LEVEL_STAGE4_2 == g_eCurrentLevel && true == static_cast<CPoleMonarch*>(pBodyCollider->Get_Parent())->Get_Strong()))
						if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Damage"), TEXT("Prototype_GameObject_Damage"), &tNumberDesc)))
						{
							MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTigerCannon_Explosion::LateTick");
							return DEAD;
						}
					m_vCollisionMonster.push_back(static_cast<CMonster*>(pBodyCollider->Get_Parent()));
					m_bDead = true;
				}
			}
		}
	}

	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
#ifdef _DEBUG
		if (m_iCollCount < 5)
			m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}




HRESULT CTigerCannon_Explosion::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CTigerCannon_Explosion::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 7)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CTigerCannon_Explosion::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTigerCannon_Explosion::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTigerCannon_Explosion::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTigerCannon_Explosion::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTigerCannon_Explosion::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TigerCannon_Explosion"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTigerCannon_Explosion::SetUp_Components(Texture_Bullet_TigerCannon)");
		return E_FAIL;
	}
	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(12.f, 12.f, 12.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTigerCannon_Explosion::SetUp_Components(Collider)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CTigerCannon_Explosion::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CTigerCannon_Explosion::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CTigerCannon_Explosion::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTigerCannon_Explosion::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTigerCannon_Explosion::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CTigerCannon_Explosion::SetUp_ConstantTable");
		return E_FAIL;
	}

	_int iSprite = (_int)m_fSprite;


	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteIndex", &iSprite, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTigerCannon_Explosion::SetUp_ConstantTable(g_iSpriteIndex)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteYIndex", &m_iSpriteY, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTigerCannon_Explosion::SetUp_ConstantTable(g_iSpriteYIndex)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CTigerCannon_Explosion * CTigerCannon_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTigerCannon_Explosion*	pInstance = new CTigerCannon_Explosion(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTigerCannon_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTigerCannon_Explosion::Clone(void * pArg)
{
	CTigerCannon_Explosion*	pInstance = new CTigerCannon_Explosion(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTigerCannon_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTigerCannon_Explosion::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
