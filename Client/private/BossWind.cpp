#include "stdafx.h"
#include "..\public\BossWind.h"
#include "GameInstance.h"
#include "Easing_Function.h"
#include "Camera_Dynamic.h"

CBossWind::CBossWind(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBossWind::CBossWind(const CBossWind & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossWind::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBossWind::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossWind::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 40.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(100.0f);
	tTransformDesc.fScalingPerSec = 50.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBossWind::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBossWind::NativeConstruct");
		return E_FAIL;
	}
	_float4	vPosition = *(_float4*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
	m_pTransformCom->Scaled(XMVectorSet(2.f, 2.f, 2.f, 0.f));

	g_pGameInstance->StopSound(CSoundMgr::EFFECT13);
	g_pGameInstance->LetsPlaySound(TEXT("Boss2StormFinal.mp3"), CSoundMgr::EFFECT13, 0.6f);

	static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Set_HorizonShake(0.2f, 1.5f);

	return S_OK;
}

_int CBossWind::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBossWind::Tick");
		return DEAD;
	}

	m_fMoveUV += _float(TimeDelta) * 5.f;

	//m_pTransformCom->Scaling(TimeDelta);
	m_pTransformCom->OneScaling(CTransform::STATE_RIGHT, TimeDelta);
	m_pTransformCom->OneScaling(CTransform::STATE_LOOK, TimeDelta);
	m_fTime += _float(TimeDelta);
	if (0.3f < m_fTime)
		m_bDead = true;
	
	return NOPROBLEM;
}

_int CBossWind::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBossWind::LateTick");
		return DEAD;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}

HRESULT CBossWind::Render()
{


	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBossWind::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 24, i);
	}

	return S_OK;
}

HRESULT CBossWind::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossWind::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossWind::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_BossWind"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossWind::SetUp_Components(Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossWind::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBossWind::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBossWind::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossWind::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossWind::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossWind::SetUp_ConstantTable(g_fMoveUV)");
		return E_FAIL;
	}
	_float fMoveUVY = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &fMoveUVY, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossWind::SetUp_ConstantTable(g_fMoveUV)");
		return E_FAIL;
	}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &fMoveUVY, sizeof(_float))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossWind::SetUp_ConstantTable(g_fMoveNoise_X)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &m_fAddNoiseUV, sizeof(_float))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossWind::SetUp_ConstantTable(g_fMoveNoise_Y)");
	//	return E_FAIL;
	//}


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CBossWind * CBossWind::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossWind*	pInstance = new CBossWind(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBossWind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossWind::Clone(void * pArg)
{
	CBossWind*	pInstance = new CBossWind(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBossWind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossWind::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
