#include "stdafx.h"
#include "..\public\HpBack.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "ElementIcon.h"

CHpBack::CHpBack(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CHpBack::CHpBack(const CHpBack & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHpBack::Add_ElementIcon(_uint iElementType)
{
	CElementIcon::ELEMENTICONDESC tElementIconDesc;
	tElementIconDesc.eType = (CElementIcon::ELEMENT_TYPE)iElementType;
	tElementIconDesc.pHpBackTransform = m_pTransformCom;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_ElementIcon"), TEXT("Prototype_GameObject_ElementIcon"), &tElementIconDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CHpBack::Add_ElementIcon");
		return E_FAIL;
	}

	m_ElementIcons.push_back(static_cast<CElementIcon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_ElementIcon"))));

	return S_OK;
}

HRESULT CHpBack::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpBack::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_tHpBackDesc, pArg, sizeof(HPBACKDESC));

	CHierarchyNode* pNode = m_tHpBackDesc.pModel->Find_HierarchyNode(m_tHpBackDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tHpBackDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tHpBackDesc.pTransform->Get_WorldMatrix();



	_matrix TransformMatrix;
	if (!strcmp(m_tHpBackDesc.pSocketName, "All_Root"))
	{
		_matrix MyMatrix = XMMatrixIdentity();
		MyMatrix.r[3] = XMVectorSet(3.5f, 0.f, 0.f, 0.f);
		TransformMatrix = MyMatrix * XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	}
	else 
		TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);
	m_pTransformCom->Scaled(XMVectorSet(3.f, 3.f, 3.f, 3.f));

	m_fDepth = 8.f;

	return S_OK;
}

_int CHpBack::Tick(_double TimeDelta)
{
	if (true == m_tHpBackDesc.pParent->Get_Dead())
	{
		for (auto pElementIcon : m_ElementIcons)
			pElementIcon->Set_Dead(true);
		return DEAD;
	}
	if (0 > __super::Tick(TimeDelta))
		return -1;

	m_TransformMatrix = m_tHpBackDesc.pTransform->Get_WorldMatrix();

	_matrix TransformMatrix;
	if (!strcmp(m_tHpBackDesc.pSocketName, "All_Root"))
	{
		_matrix MyMatrix = XMMatrixIdentity();
		MyMatrix.r[3] = XMVectorSet(3.5f, 0.f, 0.f, 1.f);
		TransformMatrix = MyMatrix * XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	}
	else
		TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);

	return _int();
}

_int CHpBack::LateTick(_double TimeDelta)
{
	if (true == m_tHpBackDesc.pParent->Get_Dead())
		return DEAD;

	if (0 > __super::LateTick(TimeDelta))
		return -1;

	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));


	for (size_t i = 0; i < m_ElementIcons.size(); i++)
	{
		if (nullptr == m_ElementIcons.front())
		{
			MSGBOX("nullptr == m_ElementIcons.front() in CHpBack::LateTick");
			return E_FAIL;
		}
		if (true == m_ElementIcons.front()->Get_Dead())
		{
			m_ElementIcons.erase(m_ElementIcons.begin());
			i--;
		}
	}


	if (true == m_bUIRender)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

		if (0 < m_ElementIcons.size())
			if (FAILED(ElementIcon_Arrange()))
			{
				MSGBOX("ElementIcon_Arrange returned E_FAIL in CHpBack::LateTick");
				return DEAD;
			}
		if (0 < m_ElementIcons.size())
			for (auto pElementIcon : m_ElementIcons)
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, pElementIcon);
	}
	m_bUIRender = false;
	return _int();
}




HRESULT CHpBack::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CHpBack::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 0)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CHpBack::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHpBack::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBack"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpBack::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CHpBack::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CHpBack::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CHpBack::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CHpBack::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CHpBack::SetUp_ConstantTable");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CHpBack::ElementIcon_Arrange()
{
	if (6 == m_ElementIcons.size())
	{
		m_ElementIcons[0]->Set_Position(_float3(-0.5f, 0.15f, 0.f));
		m_ElementIcons[1]->Set_Position(_float3(-0.3f, 0.15f, 0.f));
		m_ElementIcons[2]->Set_Position(_float3(-0.1f, 0.15f, 0.f));
		m_ElementIcons[3]->Set_Position(_float3(0.1f, 0.15f, 0.f));
		m_ElementIcons[4]->Set_Position(_float3(0.3f, 0.15f, 0.f));
		m_ElementIcons[5]->Set_Position(_float3(0.5f, 0.15f, 0.f));
	}
	else if (5 == m_ElementIcons.size())
	{
		m_ElementIcons[0]->Set_Position(_float3(-0.4f, 0.15f, 0.f));
		m_ElementIcons[1]->Set_Position(_float3(-0.2f, 0.15f, 0.f));
		m_ElementIcons[2]->Set_Position(_float3(0.f, 0.15f, 0.f));
		m_ElementIcons[3]->Set_Position(_float3(0.2f, 0.15f, 0.f));
		m_ElementIcons[4]->Set_Position(_float3(0.4f, 0.15f, 0.f));
	}
	else if (4 == m_ElementIcons.size())
	{
		m_ElementIcons[0]->Set_Position(_float3(-0.3f, 0.15f, 0.f));
		m_ElementIcons[1]->Set_Position(_float3(-0.1f, 0.15f, 0.f));
		m_ElementIcons[2]->Set_Position(_float3(0.1f, 0.15f, 0.f));
		m_ElementIcons[3]->Set_Position(_float3(0.3f, 0.15f, 0.f));
	}
	else if (3 == m_ElementIcons.size())
	{
		m_ElementIcons[0]->Set_Position(_float3(-0.2f, 0.15f, 0.f));
		m_ElementIcons[1]->Set_Position(_float3(0.f, 0.15f, 0.f));
		m_ElementIcons[2]->Set_Position(_float3(0.2f, 0.15f, 0.f));
	}
	else if (2 == m_ElementIcons.size())
	{
		m_ElementIcons[0]->Set_Position(_float3(-0.1f, 0.15f, 0.f));
		m_ElementIcons[1]->Set_Position(_float3(0.1f, 0.15f, 0.f));
	}
	else if (1 == m_ElementIcons.size())
	{
		m_ElementIcons[0]->Set_Position(_float3(0.f, 0.15f, 0.f));
	}
	return S_OK;
}



CHpBack * CHpBack::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHpBack*	pInstance = new CHpBack(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CHpBack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHpBack::Clone(void * pArg)
{
	CHpBack*	pInstance = new CHpBack(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CHpBack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHpBack::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
