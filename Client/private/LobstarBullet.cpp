#include "stdafx.h"
#include "..\public\LobstarBullet.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Player_Hand.h"
#include "Monster.h"
#include "Effect.h"
#include "MonsterWave.h"
#include "Particle_Effect.h"

CLobstarBullet::CLobstarBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CLobstarBullet::CLobstarBullet(const CLobstarBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLobstarBullet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobstarBullet::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 1.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tLobstarBulletDesc = *(LOBSTARBULLETDESC*)pArg;

	m_vCenterPoint = (m_tLobstarBulletDesc.vStartPoint * 0.7f + m_tLobstarBulletDesc.vTargetPoint * 0.3f);
	_float fDist = XMVectorGetX(XMVector4Length(m_tLobstarBulletDesc.vTargetPoint - m_tLobstarBulletDesc.vStartPoint));
	m_vCenterPoint = XMVectorSetY(m_vCenterPoint, XMVectorGetY(m_vCenterPoint) + fDist * 0.4f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tLobstarBulletDesc.vStartPoint);
	m_pTransformCom->Scaled(XMVectorSet(2.f, 2.f, 2.f, 0.f));


	// 물튀기기 파티클
	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 32;
	tParticleDesc.fMinSpeed = 0.01f;
	tParticleDesc.fMaxSpeed = 0.02f;
	tParticleDesc.fOriginScale = 0.36f;
	tParticleDesc.fScaleSpeed = 0.19f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 100.f;
	tParticleDesc.fMakeRange = 1.5f;
	tParticleDesc.fMaxDistance = 5.4f;
	tParticleDesc.eDir = CParticle_Effect::DIR_ONE;
	tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	tParticleDesc.pParentPosition = &m_vPosition;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_50;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.fTrailInterval = 0.02f;
	tParticleDesc.pDead = &m_bDead;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CYachaBullet::NativeConstruct");
		return E_FAIL;
	}

	return S_OK;
}

_int CLobstarBullet::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		g_pGameInstance->StopSound(CSoundMgr::MONSTER4);
		g_pGameInstance->LetsPlaySound(TEXT("TigerCannonShot.mp3"), CSoundMgr::MONSTER4, 0.5f);

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
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_LobstarBoomGlow");
		tEffectDesc.vScale = XMVectorSet(20.f, 20.f, 1.f, 0.f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstarBullet::Tick");
			return DEAD;
		}
		tEffectDesc.fDownAlphaSpeed = 90.f;
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, 0.0001f);
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_LobstarBoomLight");
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstarBullet::Tick");
			return DEAD;
		}

		tEffectDesc.eType = CEffect::TYPE_SPRITE;
		tEffectDesc.eWay = CEffect::WAY_FINAL;
		tEffectDesc.iSpriteNumX = 3;
		tEffectDesc.iSpriteNumY = 3;
		tEffectDesc.iSpriteStartX = 0;
		tEffectDesc.iSpriteStartY = 0;
		tEffectDesc.fSpriteSpeed = 25.f;
		tEffectDesc.iPass = 20;
		tEffectDesc.pPrototypeTag = TEXT("Prototype_Component_Texture_LobstarBoomRing");
		tEffectDesc.vScale = XMVectorSet(15.f, 15.f, 1.f, 0.f);
		tEffectDesc.MyWorldMatrix = XMMatrixTranslation(0.f, 0.f, 0.0002f);
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstarBullet::Tick");
			return DEAD;
		}
		_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector	vPlayerPosition = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
		if (5.f > DISTANCE(vPlayerPosition - vPosition))
			static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Hit(40.f);


		// 터지는 파티클
		CParticle_Effect::PARTICLEDESC tParticleDesc;
		tParticleDesc.iTexIndex = 1;
		tParticleDesc.fMinSpeed = 25.f;
		tParticleDesc.fMaxSpeed = 35.f;
		tParticleDesc.fOriginScale = 0.09f;
		tParticleDesc.fScaleSpeed = 0.13f;
		tParticleDesc.fGravityPower = 0.f;
		tParticleDesc.fLiveTime = 0.35f;
		tParticleDesc.fMakeRange = 0.f;
		tParticleDesc.fMaxDistance = 7.5f;
		tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
		tParticleDesc.vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
		tParticleDesc.pParentPosition = &m_vPosition;
		tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
		tParticleDesc.bUseEasing = true;
		tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
		tParticleDesc.bChase = false;
		tParticleDesc.bRoof = false;
		tParticleDesc.bTrail = false;
		tParticleDesc.fTrailInterval = 0.f;
		tParticleDesc.pDead = nullptr;
		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstarBullet::Tick");
			return DEAD;
		}

		return DEAD;
	}
	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (0 > --m_iLiveTime)
		m_bDead = true;
	_float fDist = XMVectorGetX(XMVector4Length(m_tLobstarBulletDesc.vTargetPoint - m_tLobstarBulletDesc.vStartPoint));

	m_dLerpTime += TimeDelta * (12 /fDist);

	_vector vPos = XMVectorLerp(XMVectorLerp(m_tLobstarBulletDesc.vStartPoint, m_vCenterPoint, (_float)m_dLerpTime), XMVectorLerp(m_vCenterPoint, m_tLobstarBulletDesc.vTargetPoint, (_float)m_dLerpTime), (_float)m_dLerpTime);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	m_fRotNoise += (_float)TimeDelta * 900.f;
	m_fMoveUV += (_float)TimeDelta * 2.f;
	m_fFireEffectTime -= (_float)TimeDelta;

	if (0.f > m_fFireEffectTime)
	{
		m_fFireEffectTime = 0.01f;

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
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CLobstarBullet::Tick");
			return DEAD;
		}
	}

	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return _int();
}

_int CLobstarBullet::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	if (false == m_bManipulation)
	{
		if (g_pGameInstance->Collision_Sphere(m_pColliderCom, (CCollider*)g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider"))))
		{
			static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Hit(70.f);
			return DEAD;
		}
	}
	else
	{
		auto ColliderList = g_pGameInstance->Get_ColliderList(TEXT("Body"), CCollision_Manager::TARGET_ENEMY);
		if (nullptr != ColliderList)
		{
			for (auto pCollider : *ColliderList)
			{
				if (static_cast<CCollider*>(m_pTarget->Get_Component(TEXT("Com_Collider_Body"))) == pCollider)
					continue;

				if (true == g_pGameInstance->Collision_Sphere(m_pColliderCom, pCollider))
				{
					static_cast<CMonster*>(pCollider->Get_Parent())->Hit(70.f, 1.f);
					return DEAD;
				}
			}
		}
	}

	if (FAILED(Collision_Terrain()))
	{
		MSGBOX("Collision_Terrain returned E_FAIL in CLobstarBullet::LateTick");
		return E_FAIL;
	}


	_matrix		ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return _int();
}




HRESULT CLobstarBullet::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CLobstarBullet::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 30)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CLobstarBullet::Render");
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

HRESULT CLobstarBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarBullet::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarBullet::SetUp_Components(Shader");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_ModelShader"), (CComponent**)&m_pModelShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarBullet::SetUp_Components(Shader");
		return E_FAIL;
	}
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarBullet::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_1, TEXT("Prototype_Component_Texture_LobstarBullet"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarBullet::SetUp_Components(Texture");
		return E_FAIL;
	}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_Texture_Sour"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarBullet::SetUp_Components(Texture");
		return E_FAIL;
	}
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_1, TEXT("Prototype_Component_Model_LobstarFireRing"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarBullet::SetUp_Components(Model)");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CLobstarBullet::SetUp_Components(Collider)");
		return E_FAIL;
	}
	m_pColliderCom->Set_Parent(this);

	return S_OK;
}

HRESULT CLobstarBullet::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CLobstarBullet::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	//{
	//	MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CLobstarBullet::SetUp_ConstantTable");
	//	return E_FAIL;
	//}
	_matrix	matWorld = m_pTransformCom->Get_WorldMatrix();
	//_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	//_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	//_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_matrix	rotMat = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_fRotNoise * 0.5f));
	/*_matrix	rotMat = XMMatrixRotationAxis(XMVector4Normalize(vLook), XMConvertToRadians(m_fRotNoise * 0.5f));
	matWorld.r[0] = XMVector3TransformCoord(XMVector4Normalize(vRight), rotMat) * XMVectorGetX(m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	matWorld.r[1] = XMVector3TransformCoord(XMVector4Normalize(vUp), rotMat) * XMVectorGetX(m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	matWorld.r[2] = XMVector3TransformCoord(XMVector4Normalize(vLook), rotMat) * XMVectorGetX(m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));*/
	_float4x4	WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(rotMat * matWorld));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CLobstarBullet::SetUp_ConstantTable(World)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CLobstarBullet::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CLobstarBullet::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTransformCom->Bind_OnShader(m_pModelShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CLobstarBullet::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in CLobstarBullet::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pModelShaderCom->Set_RawValue returned E_FAIL in m_pModelShaderCom::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CLobstarBullet::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CLobstarBullet::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_AddNoiseUV", &m_fRotNoise, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CLobstarBullet::SetUp_ConstantTable(m_fRotNoise)");
		return E_FAIL;
	}
	if (FAILED(m_pModelShaderCom->Set_RawValue("g_fMoveUV_X", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CLobstarBullet::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLobstarBullet::Collision_Terrain()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CBullet_TigerCannon::Collision_Terrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CBullet_TigerCannon::Collision_Terrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	// 로컬 포지션 구하기
	_matrix	WorldMatrixInverse = pTerrainTransform->Get_WorldMatrixInverse();

	_vector	vLocalPos = XMVector3TransformCoord(vPosition, WorldMatrixInverse);
	_float3 vLocalPosFloat3;
	XMStoreFloat3(&vLocalPosFloat3, vLocalPos);

	vPosition = XMVector3TransformCoord(vLocalPos, pTerrainTransform->Get_WorldMatrix());


	_float fTerrainY = pVIBuffer_Terrain->Compute_Y(vLocalPosFloat3);

	if (fTerrainY > vLocalPosFloat3.y)
		m_bDead = true;

	return S_OK;
}



CLobstarBullet * CLobstarBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLobstarBullet*	pInstance = new CLobstarBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CLobstarBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLobstarBullet::Clone(void * pArg)
{
	CLobstarBullet*	pInstance = new CLobstarBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CLobstarBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLobstarBullet::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelShaderCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
