#include "stdafx.h"
#include "..\public\NPC_Shop.h"
#include "GameInstance.h"
#include "Player_Hand.h"
#include "HierarchyNode.h"
#include "ShopUI.h"

CNPC_Shop::CNPC_Shop(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CNPC_Shop::CNPC_Shop(const CNPC_Shop & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNPC_Shop::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CNPC_Shop::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNPC_Shop::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CNPC_Shop::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CNPC_Shop::NativeConstruct");
		return E_FAIL;
	}

	_float4x4 WorldMatrix = *(_float4x4*)pArg;
	m_pModelCom->SetUp_AnimIndex(0);

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_pCameraTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
	return S_OK;
}

_int CNPC_Shop::Tick(_double dTimeDelta)
{
	if (true == g_bCameraAction)
		return NOPROBLEM;
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CNPC_Shop::Tick");
		return -1;
	}
	if (true == m_bDead)
		return DEAD;

	if (nullptr != m_pColliderCom)
	{
		_float fDist = 0.f;
		if (false == m_bCollision && true == g_pGameInstance->Collision_RayToSphere(m_pCameraTransform->Get_State(CTransform::STATE_POSITION), m_pCameraTransform->Get_State(CTransform::STATE_LOOK), m_pColliderCom, &fDist))
		{
			if (2.f > fDist)
			{
				if (false == m_bMakeUI)
				{
					INFO_UI	tInfoUI;
					tInfoUI.fPositionX = 740.f;
					tInfoUI.fPositionY = 420.f;
					tInfoUI.fScaleX = 150.f;
					tInfoUI.fScaleY = 150.f;
					tInfoUI.iTextureIndex = 94;
					tInfoUI.fDepth = 5.f;
					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
					{
						__debugbreak();
						return DEAD;
					}
					m_pUI = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_UI"));
					m_bMakeUI = true;
				}
			}
			
			if (2.f > fDist && true == g_pGameInstance->Get_KeyEnter(DIK_F))
			{
				// 유아이 뜨게

				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Shop"), TEXT("Prototype_GameObject_Shop"), m_bBuy)))
				{
					MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CNPC_Shop::Tick");
					return DEAD;
				}
				m_bCollision = true;
				if (nullptr != m_pUI)
				{
					m_pUI->Set_Dead(true);
					m_pUI = nullptr;
					m_bMakeUI = false;
				}
			}
			else if (2.f <= fDist && nullptr != m_pUI)
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

	if (true == m_bCollision && 0 == g_pGameInstance->Get_Size(g_eCurrentLevel, TEXT("Layer_Shop")))
		m_bCollision = false;

	return _int();
}

_int CNPC_Shop::LateTick(_double dTimeDelta)
{
	if (m_bDead)
		return DEAD;
	if (true == g_bCameraAction)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		return NOPROBLEM;
	}

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CNPC_Shop::LateTick");
		return -1;
	}

	m_pModelCom->Update_CombinedTransformationMatrices(ANIM_ROOF, dTimeDelta);
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	// 절두체
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		if (nullptr != m_pColliderCom)
			m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return _int();
}




HRESULT CNPC_Shop::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CNPC_Shop::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CNPC_Shop::Render");
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

HRESULT CNPC_Shop::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CNPC_Shop::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CNPC_Shop::SetUp_Components(Shader)");
		return E_FAIL;
	}


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Shop"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CNPC_Shop::SetUp_Components(Model)");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.9f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CNPC_Shop::Set_Collider(Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNPC_Shop::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CNPC_Shop::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CNPC_Shop::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNPC_Shop::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNPC_Shop::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in CNPC_Shop::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNPC_Shop::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNPC_Shop::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNPC_Shop::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNPC_Shop::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CNPC_Shop * CNPC_Shop::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNPC_Shop*	pInstance = new CNPC_Shop(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CNPC_Shop");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNPC_Shop::Clone(void * pArg)
{
	CNPC_Shop*	pInstance = new CNPC_Shop(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CNPC_Shop");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNPC_Shop::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
