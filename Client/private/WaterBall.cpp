#include "stdafx.h"
#include "..\public\WaterBall.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Hand.h"
#include "Effect.h"
#include "Camera_Dynamic.h"

CWaterBall::CWaterBall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CWaterBall::CWaterBall(const CWaterBall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWaterBall::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CWaterBall::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWaterBall::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 30.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CWaterBall::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CWaterBall::NativeConstruct");
		return E_FAIL;
	}
	if (nullptr != pArg)
		memcpy(&m_tBulletDesc, pArg, sizeof(BULLETDESC));

	m_pCamera = static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")));

	CHierarchyNode* pNode = m_tBulletDesc.pModel->Find_HierarchyNode(m_tBulletDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tBulletDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tBulletDesc.pTransform->Get_WorldMatrix();


	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);
	m_vDir = XMVector4Normalize(XMVectorSetY(static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION), 3.f) - TransformMatrix.r[3]);

	m_pTransformCom->Scaled(XMVectorSet(2.f, 2.f, 2.f, 0.f));

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
	tEffectDesc.pDead = nullptr;
	tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
	tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_Water");
	tEffectDesc.vScale = XMVectorSet(5.f, 5.f, 5.f, 0.f);
	for (_uint i = 0; i < 20; i++)
	{
		tEffectDesc.iNumTexIndex = 0;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) / 25.f, _float(rand() % 101 - 50) / 25.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWaterBall::Tick");
			return DEAD;
		}
		tEffectDesc.iNumTexIndex = 1;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) / 25.f, _float(rand() % 101 - 50) / 25.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWaterBall::Tick");
			return DEAD;
		}
		tEffectDesc.iNumTexIndex = 2;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(_float(rand() % 101 - 50) / 25.f, _float(rand() % 101 - 50) / 25.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWaterBall::Tick");
			return DEAD;
		}
	}

	return S_OK;
}

_int CWaterBall::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		g_pGameInstance->StopSound(CSoundMgr::EFFECT13);
		g_pGameInstance->LetsPlaySound(TEXT("Boss1WaterBallBomb.mp3"), CSoundMgr::EFFECT13, 0.3f);
		_float4 vPos = {};
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_WaterBallBomb"), TEXT("Prototype_GameObject_WaterBallBomb"), &vPos);

		CEffect::EFFECTDESC tEffectDesc;
		tEffectDesc.bNotLookCamera = false;
		tEffectDesc.eMove = CEffect::MOVE_STAND;
		tEffectDesc.eType = CEffect::TYPE_2D;
		tEffectDesc.eWay = CEffect::WAY_SCALEDOWN;
		tEffectDesc.fDownAlphaSpeed = 100.f;
		tEffectDesc.iPass = 24;
		tEffectDesc.MyWorldMatrix = XMMatrixIdentity();
		tEffectDesc.pDead = nullptr;
		tEffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4_Pointer();
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_Boss1BoomGlow");
		tEffectDesc.vScale = XMVectorSet(20.f, 20.f, 1.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWaterBall::Tick");
			return DEAD;
		}
		//tEffectDesc.fDownAlphaSpeed = 90.f;
		//tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, 0.0001f);
		//tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_LobstarBoomLight");
		//if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		//{
		//	MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstarBullet::Tick");
		//	return DEAD;
		//}

		tEffectDesc.eType = CEffect::TYPE_SPRITE;
		tEffectDesc.eWay = CEffect::WAY_FINAL;
		tEffectDesc.iSpriteNumX = 3;
		tEffectDesc.iSpriteNumY = 3;
		tEffectDesc.iSpriteStartX = 0;
		tEffectDesc.iSpriteStartY = 0;
		tEffectDesc.fSpriteSpeed = 25.f;
		tEffectDesc.iPass = 20;
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_Boss1BoomRing");
		tEffectDesc.vScale = XMVectorSet(15.f, 15.f, 1.f, 0.f);
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, 0.0002f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWaterBall::Tick");
			return DEAD;
		}

		m_pCamera->Set_VerticalShake(0.4f, 1.5f);
		
		return DEAD;
	}
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CWaterBall::Tick");
		return E_FAIL;
	}

	if (0 > --m_iLiveTime)
		m_bDead = true;

	m_pTransformCom->Go_Vector(m_vDir, TimeDelta);
	m_dSpeed += TimeDelta;
	m_pTransformCom->Set_Speed(30.f + (_float)m_dSpeed * 20.f);

	_matrix TransformMatrix = m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pColliderCom->Update_Transform(TransformMatrix);

	m_fWaterEffectTime -= (_float)TimeDelta;
	if (0.f > m_fWaterEffectTime)
	{
		m_fWaterEffectTime = 0.02f;

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
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_Water");
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

_int CWaterBall::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CWaterBall::LateTick");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider* pPlayerCollider = static_cast<CCollider*>(pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));



	if (true == pGameInstance->Collision_Sphere(m_pColliderCom, pPlayerCollider))
	{
		static_cast<CPlayer_Hand*>(pPlayerCollider->Get_Parent())->Hit(100.f);
	}


	if (3.f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_bDead = true;

	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}




HRESULT CWaterBall::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CWaterBall::Render");
		return E_FAIL;
	}



	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pModelShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pModelShaderCom, 19, i);
	}

	return S_OK;
}

HRESULT CWaterBall::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_TigerCannon::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_TigerCannon::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_ModelShader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_ModelShader"), (CComponent**)&m_pModelShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_TigerCannon::SetUp_Components(ModelShader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_TigerCannon::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Texture_WaterBall"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Prototype_Component_Texture_WaterBall)");
		return E_FAIL;
	}
	/* For.Com_NoiseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_NoiseTexture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Prototype_Component_Texture_NoiseFire)");
		return E_FAIL;
	}
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_WaterBall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Prototype_Component_Model_WaterBall)");
		return E_FAIL;
	}

	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Collider)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CWaterBall::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CWaterBall::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTransformCom->Bind_OnShader(m_pModelShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CBullet_TigerCannon::SetUp_ConstantTable");
		return E_FAIL;
	}
	

	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 6)))
	{
		MSGBOX("m_pNoiseTextureCom->SetUp_OnShader returned E_FAIL in CWave::SetUp_ConstantTable(DiffuseTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pModelShaderCom, "g_SourTexture", 6)))
	{
		MSGBOX("m_pNoiseTextureCom->SetUp_OnShader returned E_FAIL in CWave::SetUp_ConstantTable(DiffuseTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_AddNoiseUV", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_AddNoiseUV)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_DistortionPower", &m_fDistortionPower, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_DistortionPower)");
		return E_FAIL;
	}

	_float	fMoveUVX = 0.f;
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_fMoveUV_X", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_fMoveUV_Y", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_fMoveNoise_X", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveNoise_X)");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_fMoveNoise_Y", &m_fAddNoiseUV, sizeof(_float))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveNoise_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_Texture("g_WindowTexture", m_pRendererCom->Get_SRV(TEXT("Target_Caculation")))))
	{
		MSGBOX("m_pShaderCom->Set_Texture returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_WindowTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_Texture("g_WindowTexture", m_pRendererCom->Get_SRV(TEXT("Target_Caculation")))))
	{
		MSGBOX("m_pModelShaderCom->Set_Texture returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_WindowTexture)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CWaterBall * CWaterBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWaterBall*	pInstance = new CWaterBall(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CWaterBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWaterBall::Clone(void * pArg)
{
	CWaterBall*	pInstance = new CWaterBall(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CWaterBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWaterBall::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseTextureCom);
}
