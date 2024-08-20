#include "stdafx.h"
#include "..\public\Chain.h"
#include "GameInstance.h"
#include "Player_Hand.h"
#include "HierarchyNode.h"

CChain::CChain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CChain::CChain(const CChain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CChain::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CChain::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CChain::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(360.f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CChain::NativeConstruct");
		return E_FAIL;
	}

	if (nullptr != pArg)
		memcpy(&m_tChainDesc, pArg, sizeof(CHAINDESC));

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CChain::NativeConstruct");
		return E_FAIL;
	}

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_tChainDesc.MyWorldMatrix * m_tChainDesc.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	if (TYPE_PIECE_0 <= m_tChainDesc.eType)
	{
		m_vDir = XMVectorSet(_float(rand() % 20 - 10) + 0.5f, _float(rand() % 14 - 4) - 0.5f, _float(rand() % 20 - 10) + 0.5f, 0.f);
		m_fSpeed = _float(rand() % 10 + 10);
		m_vAxis = XMVector4Normalize(XMVectorSet(-XMVectorGetZ(m_vDir), 0.f, XMVectorGetX(m_vDir), 0.f));
	}

	return S_OK;
}

_int CChain::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CChain::Tick");
		return -1;
	}

	if (TYPE_PIECE_0 > m_tChainDesc.eType && false == *m_tChainDesc.pChain)
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT5);
		g_pGameInstance->LetsPlaySound(TEXT("EnergyOrbBreak.mp3"), CSoundMgr::EFFECT5, 0.5f);

		CChain::CHAINDESC tChainDesc;
		tChainDesc.eType = CChain::TYPE_PIECE_0;
		tChainDesc.MyWorldMatrix = m_tChainDesc.MyWorldMatrix;
		tChainDesc.WorldMatrix = m_tChainDesc.WorldMatrix;
		tChainDesc.pChain = m_tChainDesc.pChain;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CChain::Tick");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_PIECE_1;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CChain::Tick");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_PIECE_2;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CChain::Tick");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_PIECE_3;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CChain::Tick");
			return E_FAIL;
		}
		tChainDesc.eType = CChain::TYPE_PIECE_4;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Chain"), TEXT("Prototype_GameObject_ChainCircle"), &tChainDesc)))
		{
			MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CChain::Tick");
			return E_FAIL;
		}
		return DEAD;
	}

	if (TYPE_PIECE_0 <= m_tChainDesc.eType)
	{
		m_fLiveTime -= (_float)dTimeDelta;
		m_pTransformCom->Go_Vector(m_vDir, dTimeDelta * m_fSpeed);
		m_pTransformCom->Turn(m_vAxis, dTimeDelta);
		if (0.f > m_fLiveTime)
		{
			return DEAD;
		}
	}

	return _int();
}

_int CChain::LateTick(_double dTimeDelta)
{
	if (m_bDead)
		return DEAD;

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CChain::LateTick");
		return -1;
	}

	// ÀýµÎÃ¼
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}




HRESULT CChain::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CChain::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CChain::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 5, i);
	}

	return S_OK;
}

HRESULT CChain::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CChain::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CChain::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	switch (m_tChainDesc.eType)
	{
	case TYPE_CIRCLE:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainCircle"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CChain::SetUp_Components(Model)");
			return E_FAIL;
		}
		break;
	case TYPE_LINE:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainLine"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CChain::SetUp_Components(Model)");
			return E_FAIL;
		}
		break;
	case TYPE_PIECE_0:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainPiece0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CChain::SetUp_Components(Model)");
			return E_FAIL;
		}
		break;
	case TYPE_PIECE_1:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainPiece1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CChain::SetUp_Components(Model)");
			return E_FAIL;
		}
		break;
	case TYPE_PIECE_2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainPiece2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CChain::SetUp_Components(Model)");
			return E_FAIL;
		}
		break;
	case TYPE_PIECE_3:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainPiece3"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CChain::SetUp_Components(Model)");
			return E_FAIL;
		}
		break;
	case TYPE_PIECE_4:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainPiece4"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CChain::SetUp_Components(Model)");
			return E_FAIL;
		}
		break;
	}



	return S_OK;
}

HRESULT CChain::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CChain::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CChain::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CChain::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CChain::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in CChain::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CChain::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CChain::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CChain::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CChain::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CChain::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CChain * CChain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CChain*	pInstance = new CChain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CChain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChain::Clone(void * pArg)
{
	CChain*	pInstance = new CChain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CChain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChain::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
