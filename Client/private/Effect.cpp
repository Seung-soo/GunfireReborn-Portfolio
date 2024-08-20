#include "stdafx.h"
#include "..\public\Effect.h"
#include "GameInstance.h"
#include "Player_Hand.h"

CEffect::CEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CEffect::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CEffect::NativeConstruct");
		return E_FAIL;
	}

	if (nullptr != pArg)
		memcpy(&m_tEffectDesc, pArg, sizeof(EFFECTDESC));

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CEffect::NativeConstruct");
		return E_FAIL;
	}
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_tEffectDesc.MyWorldMatrix * XMLoadFloat4x4(m_tEffectDesc.pParentWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_pTransformCom->Scaled(m_tEffectDesc.vScale);
	m_fScale = XMVectorGetX(m_tEffectDesc.vScale);
	m_fCurrentSpriteIndexX = (_float)m_tEffectDesc.iSpriteStartX;
	m_iCurrentSpriteIndexY = m_tEffectDesc.iSpriteStartY;
	return S_OK;
}

_int CEffect::Tick(_double dTimeDelta)
{
	if ((nullptr != m_tEffectDesc.pDead && true == *m_tEffectDesc.pDead) || true == m_bDead)
		return DEAD;
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CEffect::Tick");
		return DEAD;
	}

	m_fCurrentTime += _float(dTimeDelta);

	if (WAY_SCALEDOWN == m_tEffectDesc.eWay)
	{
		m_fScale -= m_tEffectDesc.fDownAlphaSpeed * (_float)dTimeDelta;
		if (0 >= m_fScale)
		{
			return DEAD;
		}
		else
			m_pTransformCom->Scaled(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));
	}
	else if (WAY_SCALEUP == m_tEffectDesc.eWay)
	{
		m_fScale += m_tEffectDesc.fDownAlphaSpeed * (_float)dTimeDelta;
		if (m_tEffectDesc.fRoofTime <= m_fScale)
		{
			return DEAD;
		}
		else
			m_pTransformCom->Scaled(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));
	}

	return NOPROBLEM;
}

_int CEffect::LateTick(_double dTimeDelta)
{
	if ((nullptr != m_tEffectDesc.pDead && true == *m_tEffectDesc.pDead) || true == m_bDead)
		return DEAD;

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CEffect::LateTick");
		return -1;
	}

	if (MOVE_CHASE == m_tEffectDesc.eMove)
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, m_tEffectDesc.MyWorldMatrix * XMLoadFloat4x4(m_tEffectDesc.pParentWorldMatrix));
		m_pTransformCom->Set_WorldMatrix(WorldMatrix);
		if (WAY_SCALEDOWN != m_tEffectDesc.eWay && WAY_SCALEUP != m_tEffectDesc.eWay)
			m_pTransformCom->Scaled(m_tEffectDesc.vScale);
		else
			m_pTransformCom->Scaled(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));

	}
	else if (MOVE_DIR == m_tEffectDesc.eMove)
	{
		m_pTransformCom->Go_Vector(m_tEffectDesc.vDir, dTimeDelta * 20.f);
	}

	if (TYPE_SPRITE == m_tEffectDesc.eType)
		Go_Sprite(dTimeDelta);

	if (TYPE_MULTITEX == m_tEffectDesc.eType)
		Go_MultiTexture(dTimeDelta);

	m_fAlpha += m_tEffectDesc.fDownAlphaSpeed * (_float)dTimeDelta;
	if (255.f < m_fAlpha)
	{
		m_fAlpha = 255.f;
		if (WAY_ALPHADOWN == m_tEffectDesc.eWay)
			m_bDead = true;
	}

	if (WAY_FINAL == m_tEffectDesc.eWay)
	{
		if (0.f == m_fCurrentSpriteIndexX && m_tEffectDesc.iSpriteNumY == m_iCurrentSpriteIndexY)
			return DEAD;
	}


	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
	{
		if (TYPE_MESH > m_tEffectDesc.eType)
		{
			if (true == m_tEffectDesc.bNotTurnY)
			{
				_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

				ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

				m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSetY(ViewMatrix.r[2], 0.f) * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));
				m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f) * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
				m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
			}
			else if (false == m_tEffectDesc.bNotLookCamera)
			{
				_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

				ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

				m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
				m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
				m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));
			}
		}
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}

	return NOPROBLEM;
}




HRESULT CEffect::Render()
{

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CEffect::Render");
		return E_FAIL;
	}

	if (TYPE_MESH > m_tEffectDesc.eType)
	{
		if (nullptr == m_pVIBuffer)
		{
			MSGBOX("nullptr == m_pVIBuffer in CEffect::Render");
			return E_FAIL;
		}
		if (FAILED(m_pVIBuffer->Render(m_pShaderCom, m_tEffectDesc.iPass)))
		{
			MSGBOX("m_pVIBuffer->Render returned E_FAIL in CEffect::Render");
			return E_FAIL;
		}
	}
	else if (TYPE_MESH == m_tEffectDesc.eType)
	{
		if (nullptr == m_pModelCom)
		{
			MSGBOX("nullptr == m_pModelCom in CEffect::Render");
			return E_FAIL;
		}

		_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; i++)
		{
			m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Render(m_pShaderCom, m_tEffectDesc.iPass, i);
		}
	}


	return S_OK;
}

HRESULT CEffect::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CEffect::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	
	if (TYPE_MESH > m_tEffectDesc.eType)
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CEffect::SetUp_Components(Shader)");
			return E_FAIL;
		}
		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CEffect::SetUp_Components(VIBuffer)");
			return E_FAIL;
		}
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tEffectDesc.pPrototypeTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CEffect::SetUp_Components(Texture)");
			return E_FAIL;
		}
	}
	else if (TYPE_MESH == m_tEffectDesc.eType)
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CEffect::SetUp_Components(Shader)");
			return E_FAIL;
		}

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tEffectDesc.pPrototypeTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CEffect::SetUp_Components(Model)");
			return E_FAIL;
		}
	}





	return S_OK;
}

HRESULT CEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CEffect::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CEffect::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CEffect::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CEffect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}


	if (TYPE_MESH > m_tEffectDesc.eType)
	{
		_uint iTexIndex = (_uint)m_fTexIndex;
		if (TYPE_MULTITEX > m_tEffectDesc.eType)
			iTexIndex = m_tEffectDesc.iNumTexIndex;
		if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", iTexIndex)))
		{
			MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CEffect::SetUp_ConstantTable(DiffuseTexture)");
			return E_FAIL;
		}

		if (TYPE_SPRITE == m_tEffectDesc.eType)
		{
			/*g_iSpriteIndex;
			g_iSpriteYIndex = 0;
			g_iSpriteMaxXIndex;
			g_iSpriteMaxYIndex;*/
			_int iSpriteX = (_int)m_fCurrentSpriteIndexX;

			if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteIndex", &iSpriteX, sizeof(_int))))
			{
				MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CEffect::SetUp_ConstantTable(g_iSpriteIndex)");
				return E_FAIL;
			}
			if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteYIndex", &m_iCurrentSpriteIndexY, sizeof(_int))))
			{
				MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CEffect::SetUp_ConstantTable(g_iSpriteYIndex)");
				return E_FAIL;
			}
			if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteMaxXIndex", &m_tEffectDesc.iSpriteNumX, sizeof(_int))))
			{
				MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CEffect::SetUp_ConstantTable(g_iSpriteMaxXIndex)");
				return E_FAIL;
			}
			if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteMaxYIndex", &m_tEffectDesc.iSpriteNumY, sizeof(_int))))
			{
				MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CEffect::SetUp_ConstantTable(g_iSpriteMaxYIndex)");
				return E_FAIL;
			}
		}
		_int iAlpha = (_int)m_fAlpha;
		if (FAILED(m_pShaderCom->Set_RawValue("g_iHitAlpha", &iAlpha, sizeof(_int))))
		{
			MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CEffect::SetUp_ConstantTable(g_iHitAlpha)");
			return E_FAIL;
		}
	}


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CEffect::Go_Sprite(_double TimeDelta)
{
	m_fCurrentSpriteIndexX += m_tEffectDesc.fSpriteSpeed * (_float)TimeDelta;
	if (m_tEffectDesc.iSpriteNumX <= m_fCurrentSpriteIndexX)
	{
		++m_iCurrentSpriteIndexY;
		m_fCurrentSpriteIndexX = 0.f;
		if (m_tEffectDesc.iSpriteNumY <= m_iCurrentSpriteIndexY)
		{
			if (WAY_FINAL == m_tEffectDesc.eWay)
			{
				m_bDead = true;
			}
			else if (WAY_ROOFTIME == m_tEffectDesc.eWay)
				m_iCurrentSpriteIndexY = 0;
		}
	}
}

void CEffect::Go_MultiTexture(_double TimeDelta)
{
	m_fTexIndex += m_tEffectDesc.fSpriteSpeed * (_float)TimeDelta;
	if (m_tEffectDesc.iNumTexIndex <= m_fTexIndex)
	{
		if (WAY_FINAL == m_tEffectDesc.eWay)
		{
			m_bDead = true;
			m_fTexIndex -= 1.f;
		}
		else if (WAY_ROOFTIME == m_tEffectDesc.eWay)
		{
			if (m_tEffectDesc.fRoofTime <= m_fCurrentTime)
			{
				m_bDead = true;
				m_fTexIndex -= 1.f;
				return;
			}
			else
				m_fTexIndex = 0.f;
		}
	}
}

CEffect * CEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect*	pInstance = new CEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect::Clone(void * pArg)
{
	CEffect*	pInstance = new CEffect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
