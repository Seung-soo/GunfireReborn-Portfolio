#include "stdafx.h"
#include "..\public\TeslaHit.h"
#include "GameInstance.h"

CTeslaHit::CTeslaHit(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CTeslaHit::CTeslaHit(const CTeslaHit & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTeslaHit::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CTeslaHit::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTeslaHit::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 40.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(1000.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CTeslaHit::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CTeslaHit::NativeConstruct");
		return E_FAIL;
	}
	CTransform*	pTransform = (CTransform*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTransform->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(_float(rand() % 360)));
	m_pTransformCom->Scaled(XMVectorSet(4.f + _float(rand() % 3), 2.f, 4.f + _float(rand() % 3), 0.f));

	return S_OK;
}

_int CTeslaHit::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CTeslaHit::Tick");
		return DEAD;
	}

	if (m_fMoveUV <= -1.f)
	{
		//if (0.f > m_fDelayTime)
			m_bDead = true;
		//m_fDelayTime -= _float(TimeDelta) * 5.f;
	}
	else
		m_fMoveUV -= _float(TimeDelta) * 8.f;

	return NOPROBLEM;
}

_int CTeslaHit::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CTeslaHit::LateTick");
		return DEAD;
	}

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}


	return _int();
}

HRESULT CTeslaHit::Render()
{

	if (2 < m_iRender++ % 6)
		return S_OK;

	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CTeslaHit::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 7, i);
	}

	return S_OK;
}

HRESULT CTeslaHit::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTeslaHit::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTeslaHit::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_TeslaHit"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTeslaHit::SetUp_Components(Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTeslaHit::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CTeslaHit::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CTeslaHit::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTeslaHit::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTeslaHit::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	_float	fMoveUVY = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTeslaHit::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &fMoveUVY, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTeslaHit::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CTeslaHit * CTeslaHit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTeslaHit*	pInstance = new CTeslaHit(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTeslaHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTeslaHit::Clone(void * pArg)
{
	CTeslaHit*	pInstance = new CTeslaHit(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTeslaHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTeslaHit::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
