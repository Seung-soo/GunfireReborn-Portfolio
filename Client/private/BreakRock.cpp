#include "stdafx.h"
#include "..\public\BreakRock.h"
#include "GameInstance.h"

CBreakRock::CBreakRock(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBreakRock::CBreakRock(const CBreakRock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBreakRock::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBreakRock::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBreakRock::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(_float(rand() % 10 + 20));
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBreakRock::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBreakRock::NativeConstruct");
		return E_FAIL;
	}
	memcpy(&m_tRockWindDesc, pArg, sizeof(tagRockDesc));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tRockWindDesc.vPosition);

	m_pTransformCom->Scaled(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));

	return S_OK;
}

_int CBreakRock::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBreakRock::Tick");
		return -1;
	}

	m_pTransformCom->Go_Vector(m_tRockWindDesc.vDir, dTimeDelta);
	m_pTransformCom->Go_Vector(XMVectorSet(0.f, -1.f, 0.f, 0.f), dTimeDelta * m_fDownPower);

	if (0.5f < m_fTime)
		return DEAD;

	m_fTime += (_float)dTimeDelta;
	m_fDownPower += (_float)dTimeDelta * m_fGravityPower;

	return _int();
}

_int CBreakRock::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBreakRock::LateTick");
		return -1;
	}


	// ÀýµÎÃ¼
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

	return _int();
}




HRESULT CBreakRock::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CBreakRock::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBreakRock::Render");
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

HRESULT CBreakRock::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBreakRock::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBreakRock::SetUp_Components(Shader)");
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
		MSGBOX("__super::Add_Component returned E_FAIL in CBreakRock::SetUp_Components(Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBreakRock::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBreakRock::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBreakRock::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBreakRock::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBreakRock::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CBreakRock * CBreakRock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBreakRock*	pInstance = new CBreakRock(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBreakRock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBreakRock::Clone(void * pArg)
{
	CBreakRock*	pInstance = new CBreakRock(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBreakRock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBreakRock::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
