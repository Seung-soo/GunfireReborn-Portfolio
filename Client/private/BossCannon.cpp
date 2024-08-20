#include "stdafx.h"
#include "..\public\BossCannon.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "BossCannonBullet.h"

CBossCannon::CBossCannon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBossCannon::CBossCannon(const CBossCannon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossCannon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBossCannon::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossCannon::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 1.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(20.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBossCannon::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBossCannon::NativeConstruct");
		return E_FAIL;
	}

	if (nullptr != pArg)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vector*)pArg);

	m_pBossTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STAGE4_2, TEXT("Layer_Boss"), TEXT("Com_Transform")));
	m_pTransformCom->MatchIt_XZ(m_pBossTransform->Get_State(CTransform::STATE_POSITION));
	m_pModelCom->SetUp_AnimIndex(m_iAnimIndex);


	return S_OK;
}

_int CBossCannon::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBossCannon::Tick");
		return -1;
	}

	m_pTransformCom->Revolution(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pBossTransform->Get_State(CTransform::STATE_POSITION), dTimeDelta);
	m_pTransformCom->MatchIt_XZ(m_pBossTransform->Get_State(CTransform::STATE_POSITION));

	if (true == m_pModelCom->Get_EndAnimation(m_iAnimIndex))
	{
		if (0 == m_iAnimIndex)
			m_iAnimIndex = 1;
		else
			m_iAnimIndex = 0;

		m_pModelCom->SetUp_AnimIndex(m_iAnimIndex);
	}

	if (false == m_bFire && 0 < m_pModelCom->Get_PlayAcc() && 2 > m_pModelCom->Get_PlayAcc())
	{
		CBossCannonBullet::BULLETDESC tBulletDesc = {};
		tBulletDesc.pTransform = m_pTransformCom;
		tBulletDesc.pModel = m_pModelCom;
		tBulletDesc.pSocketName = "1409_Bone050";

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_2, TEXT("Layer_BossCannonBullet"), TEXT("Prototype_GameObject_BossCannonBullet"), &tBulletDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBossCannon::Fire");
			return DEAD;
		}
		g_pGameInstance->StopSound(CSoundMgr::MONSTER1);
		g_pGameInstance->LetsPlaySound(TEXT("Boss2Fire.mp3"), CSoundMgr::MONSTER1, 0.4f);
		m_bFire = true;
	}
	else if (6 < m_pModelCom->Get_PlayAcc())
		m_bFire = false;

	return _int();
}

_int CBossCannon::LateTick(_double dTimeDelta)
{
	if (m_bDead)
		return RETURN_OBJECTPOOL;

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBossCannon::LateTick");
		return -1;
	}
	m_pModelCom->Update_CombinedTransformationMatrices(ANIM_ROOF, dTimeDelta);

	// ÀýµÎÃ¼
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return _int();
}




HRESULT CBossCannon::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CBossCannon::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBossCannon::Render");
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

HRESULT CBossCannon::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossCannon::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossCannon::SetUp_Components(Shader)");
		return E_FAIL;
	}


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_BossCannon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CSwordPower::SetUp_Components(Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossCannon::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBossCannon::SetUp_ConstantTable");
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


	//if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
	//{
	//	MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CWave::SetUp_ConstantTable(DiffuseTexture)");
	//	return E_FAIL;
	//}

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in CSwordPower::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPower::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPower::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPower::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CSwordPower::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMesh::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CBossCannon * CBossCannon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossCannon*	pInstance = new CBossCannon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBossCannon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossCannon::Clone(void * pArg)
{
	CBossCannon*	pInstance = new CBossCannon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBossCannon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossCannon::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
