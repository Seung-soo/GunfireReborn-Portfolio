#include "stdafx.h"
#include "..\public\BossGun.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "BossGunBullet.h"
#include "Effect.h"

CBossGun::CBossGun(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBossGun::CBossGun(const CBossGun & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossGun::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBossGun::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossGun::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 1.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(20.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBossGun::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBossGun::NativeConstruct");
		return E_FAIL;
	}

	m_pModelCom->SetUp_AnimIndex(0);
	m_pModelCom->Update_CombinedTransformationMatrices(ANIM_ONETIME, 0.0001);
	m_pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")));

	return S_OK;
}

_int CBossGun::Tick(_double dTimeDelta)
{
	if (m_bDead)
	{
		m_fTime = 0.f;
		m_bDead = false;
		m_bFire = false;
		m_iAnimIndex = 0;

		m_pCombineMatrix = nullptr;
		m_pOffsetMatrix = nullptr;
		m_pModelCom->SetUp_AnimIndex(0);
		m_pModelCom->Set_PlayAcc(0.f);
		m_fDissolve = 1.f;
		return RETURN_OBJECTPOOL;
	}
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBossGun::Tick");
		return -1;
	}

	return _int();
}

_int CBossGun::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBossGun::LateTick");
		return -1;
	}

	if (1 < m_fTime)
	{
		m_pModelCom->Update_CombinedTransformationMatrices(ANIM_ONETIME, dTimeDelta);

		if (true == m_pModelCom->Get_EndAnimation(0))
			m_bDead = true;

		if (false == m_bFire && 0 < m_pModelCom->Get_PlayAcc() && 2 > m_pModelCom->Get_PlayAcc())
		{
			CBossGunBullet::BULLETDESC tBulletDesc = {};
			tBulletDesc.pTransform = m_pTransformCom;
			tBulletDesc.pModel = m_pModelCom;
			tBulletDesc.pSocketName = "Att";

			if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STAGE4_2, TEXT("Layer_BossGunBullet"), TEXT("Prototype_GameObject_BossGunBullet"), &tBulletDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBossGun::LateTick");
				return DEAD;
			}

			m_bFire = true;

			CHierarchyNode* pNode = tBulletDesc.pModel->Find_HierarchyNode(tBulletDesc.pSocketName);
			m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
			m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
			m_PivotMatrix = XMLoadFloat4x4(&tBulletDesc.pModel->Get_TransformMatrix());
			m_TransformMatrix = tBulletDesc.pTransform->Get_WorldMatrix();

			_float4x4 TransformMatrix;
			XMStoreFloat4x4(&TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix);

			CEffect::EFFECTDESC tEffectDesc;
			tEffectDesc.bNotLookCamera = true;
			tEffectDesc.eMove = CEffect::MOVE_STAND;
			tEffectDesc.eType = CEffect::TYPE_SPRITE;
			tEffectDesc.eWay = CEffect::WAY_FINAL;
			tEffectDesc.iSpriteNumX = 2;
			tEffectDesc.iSpriteNumY = 2;
			tEffectDesc.iSpriteStartX = 0;
			tEffectDesc.iSpriteStartY = 0;
			tEffectDesc.fSpriteSpeed = 32.f;
			tEffectDesc.iPass = 24;
			_matrix MyMatrix = m_pTransformCom->Get_WorldMatrix();
			tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, 0.f);
			tEffectDesc.pDead = nullptr;
			tEffectDesc.pParentWorldMatrix = &TransformMatrix;
			tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_GunMuzzle");
			tEffectDesc.vScale = XMVectorSet(4.f, 4.f, 1.f, 0.f);
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
			{
				MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBossGun::LateTick");
				return DEAD;
			}


		}
		else if (6 < m_pModelCom->Get_PlayAcc())
			m_bFire = false;

	}
	m_fDissolve -= (_float)dTimeDelta * 3.f;

	m_fTime += (_float)dTimeDelta;

	// ÀýµÎÃ¼
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return _int();
}




HRESULT CBossGun::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CBossGun::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBossGun::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 5, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CBossGun::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossGun::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossGun::SetUp_Components(Shader)");
		return E_FAIL;
	}


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_BossGun"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossGun::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture"), (CComponent**)&m_pMaskTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossGun::SetUp_Components(Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossGun::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBossGun::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBossGun::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossGun::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossGun::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pMaskTextureCom->SetUp_OnShader(m_pShaderCom, "g_ThirdTexture", 90)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAlpha", &m_fDissolve, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossGun::SetUp_ConstantTable(g_fDissolveAlpha)");
		return E_FAIL;
	}
	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in CBossGun::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossGun::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossGun::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossGun::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossGun::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossGun::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CBossGun * CBossGun::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossGun*	pInstance = new CBossGun(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBossGun");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossGun::Clone(void * pArg)
{
	CBossGun*	pInstance = new CBossGun(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBossGun");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossGun::Free()
{
	__super::Free();

	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
