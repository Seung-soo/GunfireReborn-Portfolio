#include "stdafx.h"
#include "..\public\SkillBox.h"
#include "GameInstance.h"
#include "MonsterWave.h"
#include "Item_Coin.h"
#include "Particle_Effect.h"

CSkillBox::CSkillBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CSkillBox::CSkillBox(const CSkillBox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkillBox::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CSkillBox::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSkillBox::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 1.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(20.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CSkillBox::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CSkillBox::NativeConstruct");
		return E_FAIL;
	}
	
	_float4x4 WorldMatrix = *(_float4x4*)pArg;

	_bool bNavi = false;
	if (0.5f < WorldMatrix._42)
		bNavi = true;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	if (false == bNavi)
		if (FAILED(SetUp_OnTerrain()))
		{
			MSGBOX("__super::SetUp_OnTerrain returned E_FAIL in CSkillBox::LateTick");
			return DEAD;
		}

	m_pModelCom->SetUp_AnimIndex(m_iAnimIndex);
	m_pModelCom->Update_CombinedTransformationMatrices(ANIM_ONETIME, 0.016);
	m_pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")));

	return S_OK;
}

_int CSkillBox::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CSkillBox::Tick");
		return -1;
	}
	_float fDist = 0.f;
	if (nullptr != m_pColliderCom && false == m_bOpen && 0 == CMonsterWave::GetInstance()->Get_MonsterSize())
	{
		if (true == g_pGameInstance->Collision_RayToSphere(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION), m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), m_pColliderCom, &fDist))
		{
			if (2.f > fDist)
			{
				if (false == m_bMakeUI)
				{
					INFO_UI	tInfoUI;
					tInfoUI.fPositionX = 740.f;
					tInfoUI.fPositionY = 420.f;
					tInfoUI.fScaleX = 128.f;
					tInfoUI.fScaleY = 128.f;
					tInfoUI.iTextureIndex = 91;
					tInfoUI.fDepth = 5.f;
					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
					{
						__debugbreak();
						return DEAD;
					}
					m_pUI = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_UI"));
					m_bMakeUI = true;
				}
				
				if (true == g_pGameInstance->Get_KeyEnter(DIK_F))
				{
					g_pGameInstance->StopSound(CSoundMgr::EFFECT10);
					g_pGameInstance->LetsPlaySound(TEXT("Box.mp3"), CSoundMgr::EFFECT10, 1.f);

					m_bOpen = true;
					if (nullptr != m_pUI)
					{
						m_pUI->Set_Dead(true);
						m_pUI = nullptr;
						m_bMakeUI = false;
					}
				}
			}
			else if (nullptr != m_pUI)
			{
				m_pUI->Set_Dead(true);
				m_pUI = nullptr;
				m_bMakeUI = false;
			}
		}
		else if (nullptr != m_pUI)
		{
			m_pUI->Set_Dead(true);
			m_pUI = nullptr;
			m_bMakeUI = false;
		}
	}

	if (false == m_bItemDrop && 35.f < m_pModelCom->Get_PlayAcc())
	{
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Goblet"), TEXT("Prototype_GameObject_Goblet"), m_pTransformCom)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CSkillBox::Tick");
			return DEAD;
		}

		_float4 vPosition;
		XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		CParticle_Effect::PARTICLEDESC tParticleDesc;
		tParticleDesc.iTexIndex = 50;
		tParticleDesc.fMinSpeed = 10.f;
		tParticleDesc.fMaxSpeed = 20.f;
		tParticleDesc.fOriginScale = 0.3f;
		tParticleDesc.fScaleSpeed = 0.19f;
		tParticleDesc.fGravityPower = 40.f;
		tParticleDesc.fLiveTime = 0.5f;
		tParticleDesc.fMakeRange = 2.f;
		tParticleDesc.fMaxDistance = 20.f;
		tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
		tParticleDesc.pParentPosition = &vPosition;
		tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
		tParticleDesc.bUseEasing = false;
		tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
		tParticleDesc.bChase = false;
		tParticleDesc.bRoof = false;
		tParticleDesc.bTrail = false;
		tParticleDesc.fTrailInterval = 0.f;
		tParticleDesc.pDead = nullptr;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CSkillBox::Tick");
			return E_FAIL;
		}

		tParticleDesc.iTexIndex = 41;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CSkillBox::Tick");
			return E_FAIL;
		}

		tParticleDesc.iTexIndex = 42;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CSkillBox::Tick");
			return E_FAIL;
		}

		m_bItemDrop = true;
	}

	return _int();
}

_int CSkillBox::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CSkillBox::LateTick");
		return -1;
	}

	if (true == m_bOpen)
		m_pModelCom->Update_CombinedTransformationMatrices(ANIM_ONETIME, dTimeDelta);
	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	// 절두체
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return _int();
}




HRESULT CSkillBox::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CSkillBox::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CSkillBox::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 6, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CSkillBox::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSkillBox::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSkillBox::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SkillBox"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSkillBox::SetUp_Components(Model)");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.5f, 0.f, 1.f);

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSkillBox::SetUp_Components(Colider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSkillBox::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CSkillBox::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CSkillBox::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSkillBox::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSkillBox::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in CSkillBox::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSkillBox::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSkillBox::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSkillBox::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSkillBox::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSkillBox::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CSkillBox::SetUp_OnTerrain()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CSkillBox::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CSkillBox::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	// 로컬 포지션 구하기
	_matrix	WorldMatrixInverse = pTerrainTransform->Get_WorldMatrixInverse();

	_vector	vLocalPos = XMVector3TransformCoord(vPosition, WorldMatrixInverse);
	_float3 vLocalPosFloat3;
	XMStoreFloat3(&vLocalPosFloat3, vLocalPos);

	vPosition = XMVector3TransformCoord(vLocalPos, pTerrainTransform->Get_WorldMatrix());

	_float fTerrainY = pVIBuffer_Terrain->Compute_Y(vLocalPosFloat3);

	vPosition = XMVectorSetY(vPosition, fTerrainY);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CSkillBox * CSkillBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSkillBox*	pInstance = new CSkillBox(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CSkillBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkillBox::Clone(void * pArg)
{
	CSkillBox*	pInstance = new CSkillBox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CSkillBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillBox::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
