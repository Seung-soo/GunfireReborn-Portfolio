#include "stdafx.h"
#include "..\public\BulletGlow_Tesla.h"
#include "GameInstance.h"

CBulletGlow_Tesla::CBulletGlow_Tesla(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBulletGlow_Tesla::CBulletGlow_Tesla(const CBulletGlow_Tesla & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBulletGlow_Tesla::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBulletGlow_Tesla::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBulletGlow_Tesla::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 40.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 20.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBulletGlow_Tesla::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBulletGlow_Tesla::NativeConstruct");
		return E_FAIL;
	}
	m_pParent = (CGameObject*)pArg;
	m_pParentWorldMatrix = static_cast<CTransform*>(m_pParent->Get_Component(TEXT("Com_Transform")))->Get_WorldFloat4x4_Pointer();
	m_pTransformCom->Set_WorldMatrix(*m_pParentWorldMatrix);

	return S_OK;
}

_int CBulletGlow_Tesla::Tick(_double TimeDelta)
{
	if (true == m_pParent->Get_Dead())
		return DEAD;
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBulletGlow_Tesla::Tick");
		return E_FAIL;
	}




	return _int();
}

_int CBulletGlow_Tesla::LateTick(_double TimeDelta)
{
	if (true == m_pParent->Get_Dead())
		return DEAD;

	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBulletGlow_Tesla::LateTick");
		return E_FAIL;
	}

	m_pTransformCom->Set_WorldMatrix(*m_pParentWorldMatrix);
	m_pTransformCom->Go_Backward(TimeDelta);
	_vector vScale = m_pTransformCom->Get_AllScale();
	m_pTransformCom->Scaled(m_pTransformCom->Get_AllScale() + XMVectorSet(XMVectorGetX(vScale), 0.f, XMVectorGetZ(vScale), 0.f));
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}

	return _int();
}




HRESULT CBulletGlow_Tesla::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBulletGlow_Tesla::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 24)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CBulletGlow_Tesla::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBulletGlow_Tesla::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBulletGlow_Tesla::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBulletGlow_Tesla::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectXZ"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBulletGlow_Tesla::SetUp_Components(VIBuffer_RectXZ)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BulletGlow_Tesla"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBulletGlow_Tesla::SetUp_Components(Texture_Bullet_Tesla)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBulletGlow_Tesla::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBulletGlow_Tesla::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBulletGlow_Tesla::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBulletGlow_Tesla::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBulletGlow_Tesla::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBulletGlow_Tesla::SetUp_ConstantTable");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CBulletGlow_Tesla * CBulletGlow_Tesla::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBulletGlow_Tesla*	pInstance = new CBulletGlow_Tesla(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBulletGlow_Tesla");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBulletGlow_Tesla::Clone(void * pArg)
{
	CBulletGlow_Tesla*	pInstance = new CBulletGlow_Tesla(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBulletGlow_Tesla");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBulletGlow_Tesla::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
