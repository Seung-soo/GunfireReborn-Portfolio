#include "stdafx.h"
#include "..\public\RevolutionRock.h"
#include "GameInstance.h"
#include "Player_Hand.h"
#include "HierarchyNode.h"
#include "PoleMonarch.h"
#include "RockWind.h"
#include "BreakRock.h"
#include "BearAttackSmoke.h"
#include "RockWarning.h"
#include "Camera_Dynamic.h"

CRevolutionRock::CRevolutionRock(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CRevolutionRock::CRevolutionRock(const CRevolutionRock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRevolutionRock::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CRevolutionRock::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRevolutionRock::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(_float(rand() % 10 + 20));
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CRevolutionRock::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CRevolutionRock::NativeConstruct");
		return E_FAIL;
	}

	m_vStartPosition = XMVectorSet(54.f, 48.f + _float(rand() % 40) * 0.1f, 37.f, 1.f) + XMVector4Normalize(XMVectorSet(_float(rand() % 360) - 180.f, 0.f, _float(rand() % 360) - 180.f, 0.f )) * 30.f;
	//m_vAxis = XMVector4Normalize(XMVectorSet(_float(rand() % 360) - 180.f, _float(rand() % 360) - 180.f, _float(rand() % 360) - 180.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vStartPosition);
	m_pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")));
	return S_OK;
}

_int CRevolutionRock::Tick(_double dTimeDelta)
{
	static _int iSound = 0;
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CRevolutionRock::Tick");
		return -1;
	}
	if (true == m_bDead)
	{
		CBreakRock::ROCKDESC tRockDesc;
		tRockDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		CBearAttackSmoke::SMOKEDESC	tSmokeDesc;
		tSmokeDesc.pTransform = m_pTransformCom;

		for (_uint i = 0; i < 5; i++)
		{
			tRockDesc.vDir = XMVector4Transform(XMVector4Normalize(XMVectorSet(0.f, 1.f, 1.f, 0.f)), XMMatrixRotationAxis(XMVectorSet(0.f,1.f,0.f,0.f), XMConvertToRadians(_float(rand() % 360))));
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BreakRock"), &tRockDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CRevolutionRock::Tick");
				return DEAD;
			}
			tSmokeDesc.vDir = XMVector4Normalize(XMVectorSet(_float(rand() % 361 - 180), 0.f, _float(rand() % 361 - 180), 0.f));
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BearAttackSmoke"), &tSmokeDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CRevolutionRock::Tick");
				return DEAD;
			}
		}

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vStartPosition);
		m_bDead = false;
		m_bThrow = false;
		m_bTargetSetting = false;

		switch (iSound)
		{
		case 0:
			g_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT2);
			g_pGameInstance->LetsPlaySound(TEXT("Boss2BreakRock.mp3"), CSoundMgr::SYSTEM_EFFECT2, 0.5f);
			break;
		case 1:
			g_pGameInstance->StopSound(CSoundMgr::MONSTER3);
			g_pGameInstance->LetsPlaySound(TEXT("Boss2BreakRock.mp3"), CSoundMgr::MONSTER3, 0.5f);
			break;
		case 2:
			g_pGameInstance->StopSound(CSoundMgr::MONSTER4);
			g_pGameInstance->LetsPlaySound(TEXT("Boss2BreakRock.mp3"), CSoundMgr::MONSTER4, 0.5f);
			break;
		case 3:
			g_pGameInstance->StopSound(CSoundMgr::MONSTER5);
			g_pGameInstance->LetsPlaySound(TEXT("Boss2BreakRock.mp3"), CSoundMgr::MONSTER5, 0.5f);
			break;
		}

		iSound++;
		if (3 < iSound)
			iSound = 0;

		static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Set_VerticalShake(0.15f, 2.f);
	}

	if (41.5f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_bDead = true;

	if (true == m_bThrow && false == m_bTargetSetting)
	{
		_vector	vTargetPoint = XMVectorSet(54.f, 41.5f, 37.f, 1.f) + XMVector4Normalize(XMVectorSet(_float(rand() % 360) - 180.f, 0.f, _float(rand() % 360) - 180.f, 0.f)) * (_float(rand() % 1400) / 100.f);
		m_vTargetDir = vTargetPoint - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_vAxis = XMVector4Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector4Normalize(m_vTargetDir)));

		CRockWind::ROCKWINDDESC tRockWindDesc;
		tRockWindDesc.pDead = &m_bDead;
		tRockWindDesc.pRockWindTransform = m_pTransformCom;
		tRockWindDesc.vDir = XMVector4Normalize(m_vTargetDir);

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockWind"), &tRockWindDesc)))
		{
			__debugbreak();
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CRevolutionRock::Tick");
			return DEAD;
		}

		CRockWarning::WARNINGDESC tWarningDesc;
		tWarningDesc.pDead = &m_bDead;
		tWarningDesc.vPosition = vTargetPoint;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockWarning"), &tWarningDesc)))
		{
			__debugbreak();
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CRevolutionRock::Tick");
			return DEAD;
		}

		m_bTargetSetting = true;
		//g_pGameInstance->FrontChange(g_eCurrentLevel, TEXT("Layer_RevolutionRock"));
	}

	if (false == m_bThrow)
	{
		m_pTransformCom->Revolution(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVectorSet(54.f, 41.3f, 37.f, 1.f), dTimeDelta);
		m_pTransformCom->LookAt(XMVectorSet(54.f, 41.3f, 37.f, 1.f));
	}
	else
	{
		m_pTransformCom->Go_Vector(m_vTargetDir, dTimeDelta * 1.7f);
		m_pTransformCom->Turn(m_vAxis, dTimeDelta * 5.f);
	}
	

	if (true == m_bDead)
	{
		CCollider* pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));
		if (3.f > DISTANCE(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
			static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(180.f);
		//if (nullptr != m_pColliderCom)
		//{
		//	CCollider::COLLIDERDESC tColliderDesc;
		//	tColliderDesc.vScale = _float3(2.5f, 2.5f, 2.5f);
		//	m_pColliderCom->Set_ColliderDesc(tColliderDesc);
		//	if (true == g_pGameInstance->Collision_Sphere(m_pColliderCom, pPlayerCollider))
		//	{
		//		static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(180.f);
		//	}
		//	tColliderDesc.vScale = _float3(0.4f, 0.4f, 0.4f);
		//	m_pColliderCom->Set_ColliderDesc(tColliderDesc);
		//}
	}


	return _int();
}

_int CRevolutionRock::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CRevolutionRock::LateTick");
		return -1;
	}
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	// ÀýµÎÃ¼
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 6.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return _int();
}




HRESULT CRevolutionRock::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CRevolutionRock::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CRevolutionRock::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 25, i);
	}

	return S_OK;
}

HRESULT CRevolutionRock::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRevolutionRock::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRevolutionRock::SetUp_Components(Shader)");
		return E_FAIL;
	}

	_tchar wszModel[MAX_PATH] = TEXT("");

	_int iRandomModel = rand() % 5 + 1;
	switch (iRandomModel)
	{
	case 1:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_Crystal1"));
		break;
	case 2:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_Crystal2"));
		break;
	case 3:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_Crystal3"));
		break;
	case 4:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_Crystal4"));
		break;
	case 5:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_Crystal5"));
		break;
	}
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, wszModel, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRevolutionRock::SetUp_Components(Model)");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRevolutionRock::Set_Collider(Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRevolutionRock::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CRevolutionRock::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CRevolutionRock::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRevolutionRock::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRevolutionRock::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in CRevolutionRock::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRevolutionRock::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRevolutionRock::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &_float4(1.f,1.f,1.f,1.f), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRevolutionRock::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRevolutionRock::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRevolutionRock::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CRevolutionRock * CRevolutionRock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRevolutionRock*	pInstance = new CRevolutionRock(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CRevolutionRock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRevolutionRock::Clone(void * pArg)
{
	CRevolutionRock*	pInstance = new CRevolutionRock(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CRevolutionRock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRevolutionRock::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
