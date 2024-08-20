#include "stdafx.h"
#include "..\public\BossCannonBullet.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Hand.h"
#include "PoleMonarch.h"
#include "Effect.h"

CBossCannonBullet::CBossCannonBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBossCannonBullet::CBossCannonBullet(const CBossCannonBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossCannonBullet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBossCannonBullet::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossCannonBullet::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBossCannonBullet::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBossCannonBullet::NativeConstruct");
		return E_FAIL;
	}
	if (nullptr != pArg)
		memcpy(&m_tBulletDesc, pArg, sizeof(BULLETDESC));



	CHierarchyNode* pNode = m_tBulletDesc.pModel->Find_HierarchyNode(m_tBulletDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tBulletDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tBulletDesc.pTransform->Get_WorldMatrix();

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);
	m_vDir = XMVector4Normalize(XMVectorSetY(TransformMatrix.r[3] - static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STAGE4_2, TEXT("Layer_Boss"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION), 0.f));

	m_pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));
	m_pPoleMornarch = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Boss"));
	return S_OK;
}

_int CBossCannonBullet::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBossCannonBullet::Tick");
		return E_FAIL;
	}

	if (0 > --m_iLiveTime)
		m_bDead = true;

	m_pTransformCom->Go_Vector(m_vDir, TimeDelta);

	if (0.f > m_fLightningTime)
	{
		_int iRandom = rand() % 3;

		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.eMove = CEffect::MOVE_CHASE;
		tEffectDesc.eType = CEffect::TYPE_SPRITE;
		tEffectDesc.eWay = CEffect::WAY_FINAL;
		tEffectDesc.iSpriteStartX = 0;
		tEffectDesc.iSpriteStartY = 0;
		tEffectDesc.iPass = 20;
		tEffectDesc.pDead = &m_bDead;
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_BossCannonBulletLightning");
		tEffectDesc.vScale = XMVectorSet(0.5f, 0.5f, 1.f, 0.f);
		tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
		_vector	vTranslation;
		switch (iRandom)
		{
		case 0:
			tEffectDesc.fSpriteSpeed = 64.f;
			tEffectDesc.iSpriteNumX = 4;
			tEffectDesc.iSpriteNumY = 4;
			vTranslation = XMVector4Normalize(XMVectorSet(_float(rand() % 101 - 50), _float(rand() % 101 - 50), _float(rand() % 101 - 50), 0.f)) * 0.5f;
			tEffectDesc.MyWorldMatrix = XMMatrixTranslation(XMVectorGetX(vTranslation), XMVectorGetY(vTranslation), XMVectorGetZ(vTranslation));
			tEffectDesc.iNumTexIndex = 0;

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBossCannonBullet::Tick");
				return DEAD;
			}
			break;
		case 1:
			tEffectDesc.fSpriteSpeed = 72.f;
			tEffectDesc.iSpriteNumX = 4;
			tEffectDesc.iSpriteNumY = 4;
			vTranslation = XMVector4Normalize(XMVectorSet(_float(rand() % 101 - 50), _float(rand() % 101 - 50), _float(rand() % 101 - 50), 0.f)) * 0.5f;
			tEffectDesc.MyWorldMatrix = XMMatrixTranslation(XMVectorGetX(vTranslation), XMVectorGetY(vTranslation), XMVectorGetZ(vTranslation));
			tEffectDesc.iNumTexIndex = 1;
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBossCannonBullet::Tick");
				return DEAD;
			}
			break;
		case 2:
			tEffectDesc.iSpriteNumX = 2;
			tEffectDesc.iSpriteNumY = 2;
			tEffectDesc.fSpriteSpeed = 24.f;
			vTranslation = XMVector4Normalize(XMVectorSet(_float(rand() % 101 - 50), _float(rand() % 101 - 50), _float(rand() % 101 - 50), 0.f)) * 0.5f;
			tEffectDesc.MyWorldMatrix = XMMatrixTranslation(XMVectorGetX(vTranslation), XMVectorGetY(vTranslation), XMVectorGetZ(vTranslation));
			tEffectDesc.iNumTexIndex = 2;
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBossCannonBullet::Tick");
				return DEAD;
			}
			break;
		}

		m_fLightningTime = 0.5f;
	}
	m_fLightningTime -= (_float)TimeDelta;

	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CBossCannonBullet::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBossCannonBullet::LateTick");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (true == pGameInstance->Collision_Sphere(m_pColliderCom, m_pPlayerCollider))
	{
		static_cast<CPlayer_Hand*>(m_pPlayerCollider->Get_Parent())->Hit(20.f);
		m_bDead = true;
		static_cast<CPoleMonarch*>(m_pPoleMornarch)->IncreaseForce(3);
	}

	_matrix		ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}




HRESULT CBossCannonBullet::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBossCannonBullet::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 0)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CBossCannonBullet::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossCannonBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossCannonBullet::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossCannonBullet::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossCannonBullet::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_BossCannonBullet"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossCannonBullet::SetUp_Components(Texture_BossCannonBullet)");
		return E_FAIL;
	}
	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(0.8f, 0.8f, 0.8f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossCannonBullet::SetUp_Components(Collider)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBossCannonBullet::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBossCannonBullet::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBossCannonBullet::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossCannonBullet::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossCannonBullet::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBossCannonBullet::SetUp_ConstantTable");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}
CBossCannonBullet * CBossCannonBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossCannonBullet*	pInstance = new CBossCannonBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBossCannonBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossCannonBullet::Clone(void * pArg)
{
	CBossCannonBullet*	pInstance = new CBossCannonBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBossCannonBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossCannonBullet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
