#include "stdafx.h"
#include "..\public\SwordPower.h"
#include "GameInstance.h"
#include "Player_Hand.h"
#include "HierarchyNode.h"
#include "PoleMonarch.h"
#include "Particle_Effect.h"
#include "Camera_Dynamic.h"

CSwordPower::CSwordPower(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CSwordPower::CSwordPower(const CSwordPower & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSwordPower::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CSwordPower::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSwordPower::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CSwordPower::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CSwordPower::NativeConstruct");
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

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_tBulletDesc.pTransform->Get_State(CTransform::STATE_LOOK));
	m_pTransformCom->ArrangeMatrix();

	m_vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_matrix RotMat = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_tBulletDesc.fDist));

	m_vDir = XMVector3TransformNormal(m_vDir, RotMat);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_vDir);
	m_pTransformCom->ArrangeMatrix();

	m_pPoleMornarch = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Boss"));
	m_pPhase4 = static_cast<CPoleMonarch*>(m_pPoleMornarch)->Get_Phase4();

	if (true == *m_pPhase4)
		m_pTransformCom->Scaled(XMVectorSet(0.5f, 1.f, 1.f, 0.f));

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_SwordPowerTrail"), this)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CSwordPower::NativeConstruct");
		return E_FAIL;
	}

	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 1.5f);

	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 46 + rand() % 4;
	tParticleDesc.fMinSpeed = 10.f;
	tParticleDesc.fMaxSpeed = 15.f;
	tParticleDesc.fOriginScale = 0.3f;
	tParticleDesc.fScaleSpeed = 0.05f;
	tParticleDesc.fGravityPower = 45.f;
	tParticleDesc.fLiveTime = 0.7f;
	tParticleDesc.fMakeRange = 2.f;
	tParticleDesc.fMaxDistance = 20.f;
	tParticleDesc.eDir = CParticle_Effect::DIR_ONE;
	tParticleDesc.vDir = -m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pTransformCom->Get_State(CTransform::STATE_RIGHT) + XMVectorSet(0.f, 1.f, 0.f, 0.f);
	tParticleDesc.pParentPosition = &m_vPosition;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CParticle_Effect::OUTCUBIC;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead = &m_bDead;
	for (_uint i = 0; i < 5; i++)
	{
		tParticleDesc.iTexIndex = 46 + rand() % 4;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CSwordPower::NativeConstruct");
			return E_FAIL;
		}
	}

	tParticleDesc.vDir = -m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) + XMVectorSet(0.f, 1.f, 0.f, 0.f);
	for (_uint i = 0; i < 5; i++)
	{
		tParticleDesc.iTexIndex = 46 + rand() % 4;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CSwordPower::NativeConstruct");
			return E_FAIL;
		}
	}

	static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Set_VerticalShake(0.4f, 1.f);

	return S_OK;
}

_int CSwordPower::Tick(_double dTimeDelta)
{
	if (true == m_bDead)
		return DEAD;
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CSwordPower::Tick");
		return -1;
	}
	if (1.0 < m_dLiveTime)
		m_bDead = true;

	m_pTransformCom->Go_Vector(m_vDir, dTimeDelta * 2.f);

	m_dLiveTime += dTimeDelta;

	CCollider* pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));
	if (nullptr != m_pColliderCom)
	{
		if (false == m_bCollision && true == g_pGameInstance->Collision_AABBToSphere(m_pColliderCom, pPlayerCollider))
		{
			static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(120.f);
			m_bCollision = true;
			static_cast<CPoleMonarch*>(m_pPoleMornarch)->IncreaseForce(5);
			m_bDead = true;
		}
	}
	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 1.5f);
	m_fAddNoiseUV -= (_float)dTimeDelta * 0.5f;

	return _int();
}

_int CSwordPower::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CSwordPower::LateTick");
		return -1;
	}
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	// ÀýµÎÃ¼
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRadius))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return _int();
}




HRESULT CSwordPower::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CSwordPower::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CSwordPower::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 21, i);
	}

	return S_OK;
}

HRESULT CSwordPower::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordPower::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordPower::SetUp_Components(Shader)");
		return E_FAIL;
	}


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_SwordPower"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordPower::SetUp_Components(Model)");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(2.f, 8.f, 2.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordPower::Set_Collider(Collider)");
		return E_FAIL;
	}

	/* For.Com_NoiseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordPower::SetUp_Components(Texture)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CSwordPower::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CSwordPower::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CSwordPower::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPower::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPower::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 74)))
	{
		MSGBOX("m_pNoiseTextureCom->SetUp_OnShader returned E_FAIL in CSwordPower::SetUp_ConstantTable(DiffuseTexture)");
		return E_FAIL;
	}


	_float	fMoveUVX = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPower::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPower::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPower::SetUp_ConstantTable(g_fMoveNoise_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPower::SetUp_ConstantTable(g_fMoveNoise_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_Texture("g_WindowTexture", m_pRendererCom->Get_SRV(TEXT("Target_Caculation")))))
	{
		MSGBOX("m_pShaderCom->Set_Texture returned E_FAIL in CSwordPower::SetUp_ConstantTable(g_WindowTexture)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CSwordPower * CSwordPower::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSwordPower*	pInstance = new CSwordPower(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CSwordPower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSwordPower::Clone(void * pArg)
{
	CSwordPower*	pInstance = new CSwordPower(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CSwordPower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSwordPower::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
