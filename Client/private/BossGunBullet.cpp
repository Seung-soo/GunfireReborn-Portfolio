#include "stdafx.h"
#include "..\public\BossGunBullet.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Hand.h"
#include "PoleMonarch.h"

CBossGunBullet::CBossGunBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBossGunBullet::CBossGunBullet(const CBossGunBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossGunBullet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBossGunBullet::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossGunBullet::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 20.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBossGunBullet::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBossGunBullet::NativeConstruct");
		return E_FAIL;
	}
	if (nullptr != pArg)
		memcpy(&m_tBulletDesc, pArg, sizeof(BULLETDESC));


	m_vDir = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK);

	CHierarchyNode* pNode = m_tBulletDesc.pModel->Find_HierarchyNode(m_tBulletDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tBulletDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tBulletDesc.pTransform->Get_WorldMatrix();
	_vector vCamPos = g_pGameInstance->Get_CamPosition();

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(vCamPos - TransformMatrix.r[3]));
	m_vDir = XMVector4Normalize(vCamPos + XMVectorSet(0.f, -0.2f, 0.f, 0.f) - TransformMatrix.r[3]);
	// ¿ÜÀû

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vCamPos))));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pTransformCom->Get_State(CTransform::STATE_RIGHT))));

	m_pTransformCom->Scaled(XMVectorSet(0.25f, 1.f, 1.f, 0.f));

	m_pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));
	m_pPoleMornarch = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Boss"));

	g_pGameInstance->StopSound(CSoundMgr::EFFECT12);
	g_pGameInstance->LetsPlaySound(TEXT("Boss2Shot.mp3"), CSoundMgr::EFFECT12, 1.f);

	return S_OK;
}

_int CBossGunBullet::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBossGunBullet::Tick");
		return E_FAIL;
	}

	if (0 > --m_iLiveTime)
	{
		return DEAD;
	}

	m_pTransformCom->Go_Vector(m_vDir, TimeDelta);

	_matrix TransformMatrix = m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pColliderCom->Update_Transform(TransformMatrix);

	return _int();
}

_int CBossGunBullet::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBossGunBullet::LateTick");
		return E_FAIL;
	}


	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - g_pGameInstance->Get_CamPosition()))));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pTransformCom->Get_State(CTransform::STATE_RIGHT))));
	m_pTransformCom->Scaled(XMVectorSet(0.25f, 1.f, 1.f, 0.f));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == g_pGameInstance->Collision_Sphere(m_pColliderCom, m_pPlayerCollider))
	{
		static_cast<CPlayer_Hand*>(m_pPlayerCollider->Get_Parent())->Hit(60.f);
		static_cast<CPoleMonarch*>(m_pPoleMornarch)->IncreaseForce(3);
		RELEASE_INSTANCE(CGameInstance);
		return DEAD;
	}

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




HRESULT CBossGunBullet::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBossGunBullet::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 0)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CBossGunBullet::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossGunBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossGunBullet::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossGunBullet::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectXZ"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossGunBullet::SetUp_Components(VIBuffer_RectXZ)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_BossGunBullet"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Texture_BossGunBullet)");
		return E_FAIL;
	}
	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(0.25f, 0.25f, 0.25f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.375f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossGunBullet::SetUp_Components(Collider)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBossGunBullet::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBossGunBullet::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBossGunBullet::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossGunBullet::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossGunBullet::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBossGunBullet::SetUp_ConstantTable");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CBossGunBullet * CBossGunBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossGunBullet*	pInstance = new CBossGunBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBossGunBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossGunBullet::Clone(void * pArg)
{
	CBossGunBullet*	pInstance = new CBossGunBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBossGunBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossGunBullet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
