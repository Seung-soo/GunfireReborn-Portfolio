#include "stdafx.h"
#include "..\public\Bullet_ThunderClapGloves.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CBullet_ThunderClapGloves::CBullet_ThunderClapGloves(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBullet_ThunderClapGloves::CBullet_ThunderClapGloves(const CBullet_ThunderClapGloves & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBullet_ThunderClapGloves::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBullet_ThunderClapGloves::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBullet_ThunderClapGloves::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 20.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBullet_ThunderClapGloves::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBullet_ThunderClapGloves::NativeConstruct");
		return E_FAIL;
	}
	if (nullptr != pArg)
		memcpy(&m_tBulletDesc, pArg, sizeof(BULLETDESC));

	// Ä«¸Þ¶ó ·è ¹Þ¾Æ¿È
	m_vDir = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK);

	CHierarchyNode* pNode = m_tBulletDesc.pModel->Find_HierarchyNode(m_tBulletDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tBulletDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tBulletDesc.pTransform->Get_WorldMatrix();
	_vector vCamPos = g_pGameInstance->Get_CamPosition();

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(vCamPos + m_vDir * *m_tBulletDesc.pDist - TransformMatrix.r[3]));
	m_vDir = vCamPos + m_vDir * *m_tBulletDesc.pDist - TransformMatrix.r[3];
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3] + m_vDir * 0.5f);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - g_pGameInstance->Get_CamPosition()))));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pTransformCom->Get_State(CTransform::STATE_RIGHT))));
	m_pTransformCom->Scaled(XMVectorSet(0.25f, 1.f, XMVectorGetX(XMVector4Length(m_vDir)), 0.f));

	return S_OK;
}

_int CBullet_ThunderClapGloves::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBullet_ThunderClapGloves::Tick");
		return E_FAIL;
	}
	m_fAddNoiseUV += (_float)TimeDelta;

	if (g_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
		return DEAD;

	return _int();
}

_int CBullet_ThunderClapGloves::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBullet_ThunderClapGloves::LateTick");
		return E_FAIL;
	}
	CHierarchyNode* pNode = m_tBulletDesc.pModel->Find_HierarchyNode(m_tBulletDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tBulletDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tBulletDesc.pTransform->Get_WorldMatrix();
	_vector vCamPos = g_pGameInstance->Get_CamPosition();

	m_vDir = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK);

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(vCamPos + m_vDir * *m_tBulletDesc.pDist - TransformMatrix.r[3]));
	m_vDir = vCamPos + m_vDir * *m_tBulletDesc.pDist - TransformMatrix.r[3];
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3] + m_vDir * 0.5f);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - g_pGameInstance->Get_CamPosition()))));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector4Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pTransformCom->Get_State(CTransform::STATE_RIGHT))));
	m_pTransformCom->Scaled(XMVectorSet(0.25f, 1.f, XMVectorGetX(XMVector4Length(m_vDir)), 0.f));

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}

	return _int();
}




HRESULT CBullet_ThunderClapGloves::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBullet_ThunderClapGloves::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 25)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CBullet_ThunderClapGloves::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBullet_ThunderClapGloves::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_ThunderClapGloves::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_ThunderClapGloves::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectXZ"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_ThunderClapGloves::SetUp_Components(VIBuffer_RectXZ)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bullet_ThunderClapGloves"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom[0])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_ThunderClapGloves::SetUp_Components(Texture_Bullet_ThunderClapGloves)");
		return E_FAIL;
	}
	/* For.Com_Texture1 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_SourceTexture"), (CComponent**)&m_pTextureCom[1])))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_ThunderClapGloves::SetUp_Components(Texture");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBullet_ThunderClapGloves::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBullet_ThunderClapGloves::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBullet_ThunderClapGloves::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_ThunderClapGloves::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_ThunderClapGloves::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom[0]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBullet_ThunderClapGloves::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom[1]->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 37)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBullet_ThunderClapGloves::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_AddNoiseUV", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_ThunderClapGloves::SetUp_ConstantTable(g_AddNoiseUV)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CBullet_ThunderClapGloves * CBullet_ThunderClapGloves::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBullet_ThunderClapGloves*	pInstance = new CBullet_ThunderClapGloves(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBullet_ThunderClapGloves");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBullet_ThunderClapGloves::Clone(void * pArg)
{
	CBullet_ThunderClapGloves*	pInstance = new CBullet_ThunderClapGloves(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBullet_ThunderClapGloves");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBullet_ThunderClapGloves::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom[0]);
	Safe_Release(m_pTextureCom[1]);
	Safe_Release(m_pShaderCom);
}
