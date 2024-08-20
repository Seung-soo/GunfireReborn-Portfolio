#include "stdafx.h"
#include "..\public\LobstarSholder.h"
#include "GameInstance.h"
#include "Effect.h"
#include "HierarchyNode.h"

CLobstarSholder::CLobstarSholder(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CLobstarSholder::CLobstarSholder(const CLobstarSholder & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLobstarSholder::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobstarSholder::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 1.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_tShoulderDesc, pArg, sizeof(SHOULDERDESC));

	CHierarchyNode* pNode = m_tShoulderDesc.pModel->Find_HierarchyNode(m_tShoulderDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tShoulderDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tShoulderDesc.pTransform->Get_WorldMatrix();

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	TransformMatrix.r[3] = XMVectorSetW(TransformMatrix.r[3], 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);

	m_pTransformCom->Scaled(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));

	return S_OK;
}

_int CLobstarSholder::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		return DEAD;
	}
	if (0 > __super::Tick(TimeDelta))
		return -1;

	

	m_fRotNoise += (_float)TimeDelta * 900.f;
	m_fMoveUV += (_float)TimeDelta * 2.f;
	m_fFireEffectTime -= (_float)TimeDelta;

	if (0.f > m_fFireEffectTime)
	{
		m_fFireEffectTime = 0.5f;

		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.bNotLookCamera = false;
		tEffectDesc.eMove = CEffect::MOVE_STAND;
		tEffectDesc.eType = CEffect::TYPE_SPRITE;
		tEffectDesc.eWay = CEffect::WAY_FINAL;
		tEffectDesc.fSpriteSpeed = 25.f;
		tEffectDesc.iPass = 20;
		tEffectDesc.iSpriteNumX = 3;
		tEffectDesc.iSpriteNumY = 3;
		tEffectDesc.iSpriteStartX = 0;
		tEffectDesc.iSpriteStartY = 0;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) / 150.f, _float(rand() % 101 - 50) / 150.f, -0.3f);
		tEffectDesc.pDead = nullptr;
		tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_LobstarBulletFire");
		tEffectDesc.vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstarSholder::Tick");
			return DEAD;
		}
	}

	m_TransformMatrix = m_tShoulderDesc.pTransform->Get_WorldMatrix();

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	TransformMatrix.r[3] = XMVectorSetW(TransformMatrix.r[3], 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);

	return _int();
}

_int CLobstarSholder::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;
	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}

	return _int();
}




HRESULT CLobstarSholder::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CLobstarSholder::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 30)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CLobstarSholder::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pModelShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pModelShaderCom, 14, i);
	}

	return S_OK;
}

HRESULT CLobstarSholder::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarSholder::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarSholder::SetUp_Components(Shader");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_ModelShader"), (CComponent**)&m_pModelShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarSholder::SetUp_Components(Shader");
		return E_FAIL;
	}
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarSholder::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_1, TEXT("Prototype_Component_Texture_LobstarBullet"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarSholder::SetUp_Components(Texture");
		return E_FAIL;
	}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture_Sour"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarSholder::SetUp_Components(Texture");
		return E_FAIL;
	}
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_1, TEXT("Prototype_Component_Model_LobstarFireRing"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarSholder::SetUp_Components(Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLobstarSholder::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CLobstarSholder::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix	matWorld = m_pTransformCom->Get_WorldMatrix();

	_matrix	rotMat = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_fRotNoise * 0.5f));

	_float4x4	WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(rotMat * matWorld));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CLobstarSholder::SetUp_ConstantTable(World)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CLobstarSholder::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CLobstarSholder::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTransformCom->Bind_OnShader(m_pModelShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CLobstarSholder::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CLobstarSholder::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CLobstarSholder::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CLobstarSholder::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CLobstarSholder::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_AddNoiseUV", &m_fRotNoise, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CLobstarSholder::SetUp_ConstantTable(m_fRotNoise)");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_fMoveUV_X", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CLobstarSholder::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLobstarSholder * CLobstarSholder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLobstarSholder*	pInstance = new CLobstarSholder(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CLobstarSholder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLobstarSholder::Clone(void * pArg)
{
	CLobstarSholder*	pInstance = new CLobstarSholder(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CLobstarSholder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLobstarSholder::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelShaderCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
