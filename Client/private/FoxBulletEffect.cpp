#include "stdafx.h"
#include "..\public\FoxBulletEffect.h"
#include "GameInstance.h"

CFoxBulletEffect::CFoxBulletEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CFoxBulletEffect::CFoxBulletEffect(const CFoxBulletEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFoxBulletEffect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFoxBulletEffect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 1.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(1600.f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CFoxBulletEffect::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CFoxBulletEffect::NativeConstruct");
		return E_FAIL;
	}
	m_pBullet = (CGameObject*)pArg;
	m_pBulletTransform = static_cast<CTransform*>(m_pBullet->Get_Component(TEXT("Com_Transform")));
	m_vLocalPos = _float3(0.f, 0.f, 1.f);
	_float4x4	WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranslation(m_vLocalPos.x, m_vLocalPos.y, m_vLocalPos.z) * m_pBulletTransform->Get_WorldMatrix());

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	m_pTransformCom->Scaled(XMVectorSet(0.3f, 0.3f, 0.8f, 0.f));
	m_iTexIndex = 15;

	return S_OK;
}

_int CFoxBulletEffect::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CFoxBulletEffect::Tick");
		return DEAD;
	}

	if (true == m_pBullet->Get_Dead() || true == m_bDead)
		return DEAD;

	//if (true == g_pGameInstance->Get_KeyEnter(DIK_RIGHT))
	//{
	//	m_iTexIndex++;
	//	if (m_iTexIndex > 98)
	//		m_iTexIndex = 0;
	//}
	//else if (true == g_pGameInstance->Get_KeyEnter(DIK_LEFT))
	//{
	//	m_iTexIndex--;
	//	if (m_iTexIndex < 0)
	//		m_iTexIndex = 98;
	//}

	//if (true == g_pGameInstance->Get_KeyEnter(DIK_UP))
	//	m_iSpeed++;
	//else if (true == g_pGameInstance->Get_KeyEnter(DIK_DOWN))
	//	m_iSpeed--;

	m_fAddNoiseUV += (_float)TimeDelta * (_float)m_iSpeed;


	return _int();
}

_int CFoxBulletEffect::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CFoxBulletEffect::LateTick");
		return DEAD;
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pBulletTransform->Get_State(CTransform::STATE_POSITION) + m_pBulletTransform->Get_State(CTransform::STATE_LOOK) * m_vLocalPos.z);

	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}




HRESULT CFoxBulletEffect::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CFoxBulletEffect::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CFoxBulletEffect::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 11, i);
	}


	return S_OK;
}

HRESULT CFoxBulletEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFoxBulletEffect::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFoxBulletEffect::SetUp_Components(Shader");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_FoxBulletEffect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFoxBulletEffect::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CFoxBulletEffect::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFoxBulletEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CFoxBulletEffect::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CFoxBulletEffect::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CFoxBulletEffect::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CFoxBulletEffect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CFoxBulletEffect::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CFoxBulletEffect::SetUp_ConstantTable(NoiseUV)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CFoxBulletEffect * CFoxBulletEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CFoxBulletEffect*	pInstance = new CFoxBulletEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CFoxBulletEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFoxBulletEffect::Clone(void * pArg)
{
	CFoxBulletEffect*	pInstance = new CFoxBulletEffect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CFoxBulletEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFoxBulletEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
