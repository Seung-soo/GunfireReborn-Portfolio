#include "stdafx.h"
#include "..\public\MiniBullet.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Player_Hand.h"
#include "Effect.h"

CMiniBullet::CMiniBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CMiniBullet::CMiniBullet(const CMiniBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniBullet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniBullet::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_matrix pYachaMatrix = *(_matrix*)pArg;
	_float4x4 pFireMatrix;
	XMStoreFloat4x4(&pFireMatrix, pYachaMatrix);

	m_pTransformCom->Set_WorldMatrix(pFireMatrix);
	m_vDir = XMVector4Normalize(static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pTransformCom->Scaled(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));


	return S_OK;
}

_int CMiniBullet::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return DEAD;
	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (0 > --m_iLiveTime)
		m_bDead = true;

	m_pTransformCom->Go_Vector(m_vDir, TimeDelta);
	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	m_fWaterEffectTime -= (_float)TimeDelta;
	if (0.f > m_fWaterEffectTime)
	{
		m_fWaterEffectTime = 0.03f;

		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.bNotLookCamera = false;
		tEffectDesc.eMove = CEffect::MOVE_STAND;
		tEffectDesc.eType = CEffect::TYPE_SPRITE;
		tEffectDesc.eWay = CEffect::WAY_FINAL;
		tEffectDesc.fSpriteSpeed = 75.f;
		tEffectDesc.iPass = 20;
		tEffectDesc.iSpriteNumX = 3;
		tEffectDesc.iSpriteNumY = 3;
		tEffectDesc.iSpriteStartX = 0;
		tEffectDesc.iSpriteStartY = 0;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) / 150.f, _float(rand() % 101 - 50) / 150.f, -0.3f);
		tEffectDesc.pDead = nullptr;
		tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_MiniWater");
		tEffectDesc.vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		tEffectDesc.iNumTexIndex = 0;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWaterBall::Tick");
			return DEAD;
		}
		tEffectDesc.iNumTexIndex = 1;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) / 150.f, _float(rand() % 101 - 50) / 150.f, -0.3f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWaterBall::Tick");
			return DEAD;
		}
		tEffectDesc.iNumTexIndex = 2;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) / 150.f, _float(rand() % 101 - 50) / 150.f, -0.3f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWaterBall::Tick");
			return DEAD;
		}
	}

	m_fAddNoiseUV += _float(TimeDelta);
	m_fMoveUV -= _float(TimeDelta) * 2.f;

	return _int();
}

_int CMiniBullet::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;


	if (g_pGameInstance->Collision_Sphere(m_pColliderCom, (CCollider*)g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider"))))
	{
		static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->HitAndSlow(30.f, 0.5f);
		return DEAD;
	}


	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return _int();
}




HRESULT CMiniBullet::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CMiniBullet::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 19, i);
	}
	return S_OK;
}

HRESULT CMiniBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFOBullet::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFOBullet::SetUp_Components(Shader");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_MiniBullet"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFOBullet::SetUp_Components(Model)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFOBullet::SetUp_Components(Texture");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CUFOBullet::SetUp_Components(Collider)");
		return E_FAIL;
	}
	m_pColliderCom->Set_Parent(this);

	return S_OK;
}

HRESULT CMiniBullet::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CMiniBullet::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CMiniBullet::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMiniBullet::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CMiniBullet::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 6)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CMiniBullet::SetUp_ConstantTable");
		return E_FAIL;
	}
	_float	fMoveUVX = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveNoise_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveNoise_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_Texture("g_WindowTexture", m_pRendererCom->Get_SRV(TEXT("Target_Caculation")))))
	{
		MSGBOX("m_pShaderCom->Set_Texture returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_WindowTexture)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CMiniBullet * CMiniBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMiniBullet*	pInstance = new CMiniBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMiniBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMiniBullet::Clone(void * pArg)
{
	CMiniBullet*	pInstance = new CMiniBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMiniBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMiniBullet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
