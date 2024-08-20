#include "stdafx.h"
#include "..\public\Mini.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"
#include "Player_Hand.h"
#include "AbyssalSerpent.h"

CMini::CMini(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{

}

CMini::CMini(const CMini & rhs)
	: CMonster(rhs)
{
}

HRESULT CMini::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CMini::NativeContruct_Prototype");
		return E_FAIL;
	}

	m_tStatus.fMaxHp = 400.f;
	m_tStatus.fCurrentHp = m_tStatus.fMaxHp;
	m_tStatus.fMaxShield = 0.f;
	m_tStatus.fCurrentShield = m_tStatus.fMaxShield;
	m_tStatus.fAttack = 50.f;
	m_tStatus.fDefend = 10.f;

	return S_OK;
}

HRESULT CMini::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 15.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CMini::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CMini::NativeConstruct");
		return E_FAIL;
	}
	m_pModelCom->SetUp_AnimIndex(m_eState);
	m_pModelCom->SetUp_AnimIndex(m_eState);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4((_float4*)pArg));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, -1.f, 0.f));
	m_pTransformCom->ArrangeMatrix();

	m_bIsBoss = true;

	m_pPlayerPosition = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);


	return S_OK;
}

_int CMini::Tick(_double dTimeDelta)
{
	if (true == m_bDead || CAbyssalSerpent::DIE == static_cast<CAbyssalSerpent*>(g_pGameInstance->Get_Front(g_eCurrentLevel, TEXT("Layer_Boss")))->Get_State())
	{
		m_bDead = false;
		m_bStart = true;

		m_pMiniMap_Icon->Set_Dead(true);
		m_pMiniMap_Icon = nullptr;
		return RETURN_OBJECTPOOL;
	}

	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CMini::Tick");
		return DEAD;
	}
	if (false == m_bFreeze)
		if (FAILED(State_Manage(dTimeDelta)))
		{
			MSGBOX("State_Manage returned E_FAIL in CMini::Tick");
			return DEAD;
		}

	if (true == m_bStart)
	{
		INFO_UI tInfoUI = {};
		tInfoUI.fScaleX = 16.f;
		tInfoUI.fScaleY = 16.f;
		tInfoUI.fDepth = 3.f;
		tInfoUI.iTextureIndex = CMiniMap_Icon::TARGET_MONSTER;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon"), TEXT("Prototype_GameObject_MiniMap_Icon"), &tInfoUI)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMini::NativeConstruct");
			return E_FAIL;
		}
		m_pMiniMap_Icon = static_cast<CMiniMap_Icon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon")));
		m_pMiniMap_Icon->Set_Target(m_pTransformCom);

		m_bStart = false;
	}

	m_pPlayerPosition = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	for (auto& pBodyCollider : m_BodyCollider)
	{
		g_pGameInstance->Add_Collider(TEXT("Body"), CCollision_Manager::TARGET_ENEMY, pBodyCollider);
	}

	return NOPROBLEM;
}

_int CMini::LateTick(_double dTimeDelta)
{
	if (true == m_bDead || CAbyssalSerpent::DIE == static_cast<CAbyssalSerpent*>(g_pGameInstance->Get_Front(g_eCurrentLevel, TEXT("Layer_Boss")))->Get_State())
		return RETURN_OBJECTPOOL;

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CMini::LateTick");
		return DEAD;
	}
	if (false == m_bFreeze)
		m_pModelCom->Update_CombinedTransformationMatrices(m_iAnimationType, dTimeDelta);

	if (FAILED(Collider_Update()))
	{
		MSGBOX("Collider_Update returned E_FAIL in CAbyssalSerpent::LateTick");
		return DEAD;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
	for (auto& pBodyCollider : m_BodyCollider)
		m_pRendererCom->Add_Debug(pBodyCollider);
#endif

	return NOPROBLEM;
}




HRESULT CMini::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pRendererCom)
	{
		MSGBOX("nullptr == m_pShaderCom or nullptr == m_pRendererCom in CMini::Render");
		return E_FAIL;
	}

	if (FAILED(__super::SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CMini::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 0, i, "g_BoneMatrices");
	}

	return S_OK;
}


void CMini::Up(_double TimeDelta)
{
	if (true == m_pModelCom->Get_EndAnimation(UP))
	{
		m_eState = STAND;
		m_iAnimationType = ANIM_ROOF;
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}

void CMini::Stand(_double TimeDelta)
{
	m_dAttackInterver += TimeDelta;
	m_pTransformCom->LookAtXZ(m_pPlayerPosition);
	if (5.0 < m_dAttackInterver)
	{
		m_eState = FIRE_UP;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_dAttackInterver = 0.0;
	}
}

void CMini::Fire(double TimeDelta)
{
	m_pTransformCom->LookAtXZ(m_pPlayerPosition);

	if (false == m_bFire && 0.14 < m_pModelCom->Get_PlayAcc())
	{
		CHierarchyNode* pNode = nullptr;

		pNode = m_pModelCom->Find_HierarchyNode("muzzle");

		if (nullptr == pNode)
		{
			MSGBOX("nullptr == pNode in CMini::Fire");
			return;
		}
		_matrix TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniBullet"), TEXT("Prototype_GameObject_MiniBullet"), &TransformMatrix)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CMini::Fire");
			return;
		}
		m_bFire = true;
	}


	if (true == m_pModelCom->Get_EndAnimation(FIRE))
	{
		m_eState = STAND;
		m_iAnimationType = ANIM_ROOF;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_bFire = false;
	}


}

void CMini::Fire_Up(_double TimeDelta)
{
	m_pTransformCom->LookAtXZ(m_pPlayerPosition);
	if (true == m_pModelCom->Get_EndAnimation(FIRE_UP))
	{
		m_eState = FIRE;
		m_iAnimationType = ANIM_ONETIME;
		m_pModelCom->SetUp_AnimIndex(m_eState);
	}
}


void CMini::Die(_double TimeDelta)
{
	if (m_pModelCom->Get_EndAnimation(DIE))
	{
		m_bDead = true;
	}
}

_int CMini::Hit(_float fAttack, _float fCriticalValue, _bool bHeavy)
{
	_float fDamage = fAttack * (1.f - m_tStatus.fDefend / (m_tStatus.fDefend + 100.f)) * fCriticalValue;
	fDamage += ((rand() % _int(fDamage * 0.2f)) - fDamage * 0.1f);

	if (true == static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_Talents()[0])
		fDamage *= 1.6f;
	if (m_tStatus.fCurrentShield > 0)
	{
		fDamage /= 3.f;
		if (m_tStatus.fCurrentShield < fDamage)
		{
			fDamage -= m_tStatus.fCurrentShield;
			fDamage *= 3.f;

			m_tStatus.fCurrentShield = 0;
			m_tStatus.fCurrentHp -= fDamage;
		}
		else
		{
			m_tStatus.fCurrentShield -= fDamage;
		}
	}
	else
	{
		m_tStatus.fCurrentHp -= fDamage;
	}

	if (0.f >= m_tStatus.fCurrentHp)
	{
		m_tStatus.fCurrentHp = 0.f;
		m_eState = DIE;
		m_pModelCom->SetUp_AnimIndex(m_eState);
		m_iAnimationType = ANIM_ONETIME;
		if (true == m_bBurning || true == m_bShock || true == m_bDecay || true == m_bManipulation || true == m_bCombustion || true == m_bMiasma)
			static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_Oil();
	}
	return (_int)fDamage;
}

HRESULT CMini::StartSetting()
{
	return S_OK;
}

HRESULT CMini::Add_ElementIcon()
{
	return E_NOTIMPL;
}

HRESULT CMini::SetUp_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_Mini"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CMini::SetUp_Components(Model)");
		return E_FAIL;
	}
	m_BodyCollider.resize(3);

	/* For.Com_Collder */

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(0.75f, 0.75f, 0.75f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body0"), (CComponent**)&m_BodyCollider[0], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body0)");
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body1"), (CComponent**)&m_BodyCollider[1], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body1)");
		return E_FAIL;
	}

	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Body2"), (CComponent**)&m_BodyCollider[2], &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CAbyssalSerpent::SetUp_Components(Collider_Body2)");
		return E_FAIL;
	}
	for (auto& pBodyCollider : m_BodyCollider)
		pBodyCollider->Set_Parent(this);

	return S_OK;
}

HRESULT CMini::Collider_Update()
{
	for (auto& pBodyCollider : m_BodyCollider)
	{
		if (nullptr == pBodyCollider)
		{
			MSGBOX("nullptr == pBodyCollider in CAbyssalSerpent::Collider_Update");
			return E_FAIL;
		}
	}
	
	CHierarchyNode* pNode = nullptr;

#pragma region Body_Collider
	pNode = m_pModelCom->Find_HierarchyNode("Bone002");
	SelectCollider_Update(pNode, m_BodyCollider, 0);

	pNode = m_pModelCom->Find_HierarchyNode("Bone006");
	SelectCollider_Update(pNode, m_BodyCollider, 1);

	pNode = m_pModelCom->Find_HierarchyNode("Bone024");
	SelectCollider_Update(pNode, m_BodyCollider, 2);
#pragma endregion


	return S_OK;
}

HRESULT CMini::SelectCollider_Update(CHierarchyNode * pNode, vector<CCollider*> ColliderContainer, _uint iIndex)
{
	if (nullptr == pNode)
	{
		MSGBOX("nullptr == pNode in CMini::SelectCollider_Update");
		return E_FAIL;
	}
	_matrix			TransformMatrix = XMMatrixTranspose(pNode->Get_OffsetMatrix()) * pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	ColliderContainer[iIndex]->Update_Transform(TransformMatrix);

	return S_OK;
}

HRESULT CMini::State_Manage(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nullptr in CMini::State_Manage");
		return E_FAIL;
	}

	switch (m_eState)
	{

	case STAND:
		Stand(TimeDelta);
		break;
	case DIE:
		Die(TimeDelta);
		break;
	case UP:
		Up(TimeDelta);
		break;
	case FIRE:
		Fire(TimeDelta);
		break;
	case FIRE_UP:
		Fire_Up(TimeDelta);
		break;
	}

	return S_OK;
}

CMini * CMini::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMini*	pInstance = new CMini(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMini");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMini::Clone(void * pArg)
{
	CMini*	pInstance = new CMini(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMini");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMini::Free()
{
	__super::Free();

	for (auto& pBodyCollider : m_BodyCollider)
		Safe_Release(pBodyCollider);
	Safe_Release(m_pModelCom);
}



