#include "stdafx.h"
#include "..\public\BossBlinkSmoke.h"
#include "GameInstance.h"

CBossBlinkSmoke::CBossBlinkSmoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBossBlinkSmoke::CBossBlinkSmoke(const CBossBlinkSmoke & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossBlinkSmoke::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBossBlinkSmoke::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossBlinkSmoke::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBossBlinkSmoke::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBossBlinkSmoke::NativeConstruct");
		return E_FAIL;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4((_float4*)pArg));

	m_pTransformCom->Scaled(XMVectorSet(6.f, 6.f, 6.f, 0.f));

	return S_OK;
}

_int CBossBlinkSmoke::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBossBlinkSmoke::Tick");
		return E_FAIL;
	}

	m_fSprite += (_float)TimeDelta * 32.f;
	if (4.f <= m_fSprite)
	{
		m_fSprite = 0.f;
		m_iSpriteY++;
		if (4 <= m_iSpriteY)
			return DEAD;
	}
	m_fAlpha -= (_float)TimeDelta * 510.f;
	if (0.f > m_fAlpha)
		m_fAlpha = 0.f;
	return _int();
}

_int CBossBlinkSmoke::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBossBlinkSmoke::LateTick");
		return E_FAIL;
	}

	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}

	return _int();
}




HRESULT CBossBlinkSmoke::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBossBlinkSmoke::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 22)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CBossBlinkSmoke::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossBlinkSmoke::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossBlinkSmoke::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossBlinkSmoke::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossBlinkSmoke::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Texture_BossBlinkSmoke"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBossBlinkSmoke::SetUp_Components(Prototype_Component_Texture_BossBlinkSmoke)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBossBlinkSmoke::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBossBlinkSmoke::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBossBlinkSmoke::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossBlinkSmoke::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossBlinkSmoke::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBossBlinkSmoke::SetUp_ConstantTable");
		return E_FAIL;
	}

	_int iSprite = (_int)m_fSprite;


	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteIndex", &iSprite, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossBlinkSmoke::SetUp_ConstantTable(g_iSpriteIndex)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteYIndex", &m_iSpriteY, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossBlinkSmoke::SetUp_ConstantTable(g_iSpriteYIndex)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteMaxXIndex", &m_iMaxSpriteX, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossBlinkSmoke::SetUp_ConstantTable(g_iSpriteMaxXIndex)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteMaxYIndex", &m_iMaxSpriteY, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossBlinkSmoke::SetUp_ConstantTable(g_iSpriteMaxYIndex)");
		return E_FAIL;
	}
	_int	iAlpha = (_int)m_fAlpha;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iHitAlpha", &iAlpha, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBossBlinkSmoke::SetUp_ConstantTable(g_iHitAlpha)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CBossBlinkSmoke * CBossBlinkSmoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossBlinkSmoke*	pInstance = new CBossBlinkSmoke(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBossBlinkSmoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossBlinkSmoke::Clone(void * pArg)
{
	CBossBlinkSmoke*	pInstance = new CBossBlinkSmoke(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBossBlinkSmoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossBlinkSmoke::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
