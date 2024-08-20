#include "stdafx.h"
#include "..\public\MonsterHpBar.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CMonsterHpBar::CMonsterHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CMonsterHpBar::CMonsterHpBar(const CMonsterHpBar & rhs)
	: CGameObject(rhs)
	, m_eBarType(rhs.m_eBarType)
{
}

HRESULT CMonsterHpBar::NativeConstruct_Prototype(BarType eBarType)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eBarType = eBarType;

	return S_OK;
}

HRESULT CMonsterHpBar::NativeConstruct(void * pArg)
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
		memcpy(&m_tHpBarDesc, pArg, sizeof(HPBARDESC));

	CHierarchyNode* pNode = m_tHpBarDesc.pModel->Find_HierarchyNode(m_tHpBarDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tHpBarDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tHpBarDesc.pTransform->Get_WorldMatrix();

	_matrix TransformMatrix;
	if (!strcmp(m_tHpBarDesc.pSocketName, "All_Root"))
	{
		_matrix MyMatrix = XMMatrixIdentity();
		MyMatrix.r[3] = XMVectorSet(3.5f, 0.f, 0.f, 0.f);
		TransformMatrix = MyMatrix * XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	}
	else
		TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);

	m_pTransformCom->Scaled(XMVectorSet(2.7f, 3.f, 3.f, 0.f));

	m_fDepth = 9.f;

	m_pMonsterStatus = static_cast<CMonster*>(m_tHpBarDesc.pParent)->Get_Status();

	return S_OK;
}

_int CMonsterHpBar::Tick(_double TimeDelta)
{
	if (true == m_tHpBarDesc.pParent->Get_Dead())
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
		return -1;

	m_TransformMatrix = m_tHpBarDesc.pTransform->Get_WorldMatrix();

	_matrix TransformMatrix;
	if (!strcmp(m_tHpBarDesc.pSocketName, "All_Root"))
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

_int CMonsterHpBar::LateTick(_double TimeDelta)
{
	if (true == m_tHpBarDesc.pParent->Get_Dead())
		return DEAD;

	if (0 > __super::LateTick(TimeDelta))
		return -1;

	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (true == m_bUIRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	m_bUIRender = false;
	return _int();
}




HRESULT CMonsterHpBar::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CMonsterHpBar::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 8)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CMonsterHpBar::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterHpBar::SetUp_Components()
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
	if (HP == m_eBarType)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterHpBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (SHIELD == m_eBarType)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterShieldBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (GUARD == m_eBarType)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterGuardBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterHpBar::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CMonsterHpBar::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CMonsterHpBar::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonsterHpBar::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonsterHpBar::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CMonsterHpBar::SetUp_ConstantTable");
		return E_FAIL;
	}
	_float fCurrentHp_UV = 0.f;
	if (HP == m_eBarType)
		fCurrentHp_UV = m_pMonsterStatus->fCurrentHp / m_pMonsterStatus->fMaxHp;
	else
		fCurrentHp_UV = m_pMonsterStatus->fCurrentShield / m_pMonsterStatus->fMaxShield;

	if (FAILED(m_pShaderCom->Set_RawValue("g_CurrentHP_UV", &fCurrentHp_UV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMonsterHpBar::SetUp_ConstantTable");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CMonsterHpBar * CMonsterHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, BarType eBarType)
{
	CMonsterHpBar*	pInstance = new CMonsterHpBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eBarType)))
	{
		MSGBOX("Failed to Created CMonsterHpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonsterHpBar::Clone(void * pArg)
{
	CMonsterHpBar*	pInstance = new CMonsterHpBar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMonsterHpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterHpBar::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
