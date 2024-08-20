#include "stdafx.h"
#include "..\public\Item_Weapon.h"
#include "GameInstance.h"
#include "Player_Hand.h"
#include "HierarchyNode.h"

CItem_Weapon::CItem_Weapon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CItem_Weapon::CItem_Weapon(const CItem_Weapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CItem_Weapon::Set_Navigation(const _tchar * pPrototypeTag)
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pNavigationCom);
	m_Components.erase(TEXT("Com_Navigation"));
	/* For.Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(g_eCurrentLevel, pPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CItem_Coin::Set_Navigation(Navigation)");
		return E_FAIL;
	}
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

HRESULT CItem_Weapon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CItem_Weapon::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItem_Weapon::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CItem_Weapon::NativeConstruct");
		return E_FAIL;
	}

	if (nullptr != pArg)
		memcpy(&m_tItemWeaponDesc, pArg, sizeof(ITEMWEAPONDESC));

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CItem_Weapon::NativeConstruct");
		return E_FAIL;
	}


	m_tItemWeaponDesc.vTargetPos = XMVectorSetY(m_tItemWeaponDesc.vTargetPos, TerrainY(m_tItemWeaponDesc.vTargetPos));

	switch (m_tItemWeaponDesc.eType)
	{
	case TYPE_1211:
		m_iBullet = 10;
		break;
	case TYPE_1401:
		m_iBullet = 1;
		break;
	case TYPE_1410:
		m_iBullet = 80;
		break;
	case TYPE_1412:
		m_iBullet = 60;
		break;
	case TYPE_1501:
		m_iBullet = 5;
		break;
	case TYPE_1603:
		m_iBullet = 10;
		break;
	}

	if (TYPE_1412 == m_tItemWeaponDesc.eType)
		m_pModelCom->SetUp_AnimIndex(0);

	m_vCenterPoint = (m_tItemWeaponDesc.vPosition * 0.7f + m_tItemWeaponDesc.vTargetPos * 0.3f);
	XMVectorSetW(m_vCenterPoint, 1.f);
	_float fDist = XMVectorGetX(XMVector4Length(m_tItemWeaponDesc.vTargetPos - m_tItemWeaponDesc.vPosition));
	m_vCenterPoint = XMVectorSetY(m_vCenterPoint, XMVectorGetY(m_vCenterPoint) + fDist);

	m_pCameraTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tItemWeaponDesc.vPosition);

	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f));


	return S_OK;
}

_int CItem_Weapon::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CItem_Weapon::Tick");
		return -1;
	}
	if (true == m_bDead)
		return DEAD;
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (1.0 > m_dLerpTime)
	{
		_float fDist = XMVectorGetX(XMVector4Length(m_tItemWeaponDesc.vTargetPos - m_tItemWeaponDesc.vPosition));

		m_fJumpTime += (_float)dTimeDelta * 2.f;
		m_dLerpTime += dTimeDelta * m_fJumpTime;

		if (1.0 <= m_dLerpTime)
		{
			m_dLerpTime = 1.0;
			m_bJump = false;
			m_fJumpTime = 0.f;
		}

		_vector vPos = XMVectorLerp(XMVectorLerp(m_tItemWeaponDesc.vPosition, m_vCenterPoint, (_float)m_dLerpTime), XMVectorLerp(m_vCenterPoint, m_tItemWeaponDesc.vTargetPos, (_float)m_dLerpTime), (_float)m_dLerpTime);
		_float3* pNotUse = nullptr;

		if (true == m_pNavigationCom->Move_OnNavigation(vPos, vMyPos, &pNotUse))
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		else
		{
			m_dLerpTime = 1.0;
			m_bJump = true;
			m_fJumpTime = 0.f;
		}

	}
	else if (CCell::TYPE_ROAD == m_pNavigationCom->Get_Type() && XMVectorGetY(vMyPos) > m_pNavigationCom->Compute_Y(vMyPos))
	{
		m_fJumpTime += (_float)dTimeDelta * 9.8f;
		m_pTransformCom->Go_Down(dTimeDelta * m_fJumpTime, m_pNavigationCom);
		if (XMVectorGetY(vMyPos) <= m_pNavigationCom->Compute_Y(vMyPos))
		{
			m_bJump = false;
			m_fJumpTime = 0.f;
		}
	}
	else if (XMVectorGetY(vMyPos) > TerrainY(vMyPos))
	{
		m_fJumpTime += (_float)dTimeDelta * 9.8f;
		m_pTransformCom->Go_Down(dTimeDelta * m_fJumpTime, m_pNavigationCom);
		if (XMVectorGetY(vMyPos) <= TerrainY(vMyPos))
		{
			m_bJump = false;
			m_fJumpTime = 0.f;
		}
	}

	if (nullptr != m_pColliderCom)
	{
		_float fDist = 0.f;
		if (true == g_pGameInstance->Collision_RayToSphere(m_pCameraTransform->Get_State(CTransform::STATE_POSITION), m_pCameraTransform->Get_State(CTransform::STATE_LOOK), m_pColliderCom, &fDist))
		{
			if (2.f > fDist)
			{
				if (true == m_bStart)
				{
					INFO_UI tInfo_UI = {};
					tInfo_UI.iTextureIndex = m_tItemWeaponDesc.eType;
					if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Weapon_Info"), TEXT("Prototype_GameObject_Weapon_Info"), &tInfo_UI)))
					{
						MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CItem_Weapon::Tick");
						return DEAD;
					}
					m_bStart = false;
				}
				if (g_pGameInstance->Get_KeyEnter(DIK_F))
				{
					CPlayer_Hand::WEAPON tWeapon;
					switch (m_tItemWeaponDesc.eType)
					{
					case TYPE_1211:
						tWeapon = CPlayer_Hand::WEAPON_TESLA;
						break;
					case TYPE_1401:
						tWeapon = CPlayer_Hand::WEAPON_TIGERCANNON;
						break;
					case TYPE_1410:
						tWeapon = CPlayer_Hand::WEAPON_THUNDERCLAPGLOVES;
						break;
					case TYPE_1412:
						tWeapon = CPlayer_Hand::WEAPON_FIREDRAGON;
						break;
					case TYPE_1501:
						tWeapon = CPlayer_Hand::WEAPON_PIERCINGFRAME;
						break;
					case TYPE_1603:
						tWeapon = CPlayer_Hand::WEAPON_SWORD;
						break;
					}
					static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Set_Weapon(tWeapon, m_iBullet);
					g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Weapon_Info"))->Set_Dead(true);
					m_bDead = true;
					m_bStart = true;
				}
			}
			else if (false == m_bStart)
			{
				g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Weapon_Info"))->Set_Dead(true);
				m_bStart = true;
			}
		}
		else if (false == m_bStart)
		{
			g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Weapon_Info"))->Set_Dead(true);
			m_bStart = true;
		}
	}

	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f));


	return NOPROBLEM;
}

_int CItem_Weapon::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CItem_Weapon::LateTick");
		return -1;
	}

	if (TYPE_1412 == m_tItemWeaponDesc.eType)
	{
		m_pModelCom->Update_CombinedTransformationMatrices(ANIM_ROOF, dTimeDelta);
		m_pModelCom->Set_ModelType(CModel::TYPE_ANIM);
	}
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	if (CCell::TYPE_ROAD == m_pNavigationCom->Get_Type())
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos = XMVectorSetY(vPos, m_pNavigationCom->Compute_Y(vPos));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (FAILED(SetUp_OnTerrain()))
	{
		MSGBOX("SetUp_OnTerrain returned E_FAIL in CItem_Weapon::LateTick");
		return DEAD;
	}

	// 절두체
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return NOPROBLEM;
}




HRESULT CItem_Weapon::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CItem_Weapon::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CItem_Weapon::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		if (TYPE_1412 == m_tItemWeaponDesc.eType)
			m_pModelCom->Render(m_pShaderCom, 6, i, "g_BoneMatrices");
		else
			m_pModelCom->Render(m_pShaderCom, 28, i);
	}

	return S_OK;
}

HRESULT CItem_Weapon::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CItem_Weapon::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	_tchar wszShader[MAX_PATH] = TEXT("");
	if (TYPE_1412 == m_tItemWeaponDesc.eType)
		lstrcpy(wszShader, TEXT("Prototype_Component_Shader_AnimModel"));
	else
		lstrcpy(wszShader, TEXT("Prototype_Component_Shader_NonAnimModel"));
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, wszShader, TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CItem_Weapon::SetUp_Components(Shader)");
		return E_FAIL;
	}

	_tchar wszModel[MAX_PATH] = TEXT("");
	switch (m_tItemWeaponDesc.eType)
	{
	case TYPE_1211:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_1211"));
		break;
	case TYPE_1401:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_1401"));
		break;
	case TYPE_1410:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_1410"));
		break;
	case TYPE_1412:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_1412"));
		break;
	case TYPE_1501:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_1501"));
		break;
	case TYPE_1603:
		lstrcpy(wszModel, TEXT("Prototype_Component_Model_1603"));
		break;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, wszModel, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CItem_Weapon::SetUp_Components(Model)");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(0.7f, 0.7f, 0.7f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.5f, 0.f, 1.f);
	if (TYPE_1412 == m_tItemWeaponDesc.eType)
	{
		tColliderDesc.vTranslation = _float4(0.f, 0.2f, 0.f, 1.f);
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CItem_Weapon::Set_Collider(Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItem_Weapon::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CItem_Weapon::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CItem_Weapon::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Weapon::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Weapon::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//{
	//	MSGBOX("pLightDesc is nullptr in CItem_Coin::SetUp_ConstantTable");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Weapon::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Weapon::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Weapon::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Weapon::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Weapon::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CItem_Weapon::SetUp_OnTerrain()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CItem_Coin::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CItem_Coin::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	// 로컬 포지션 구하기
	_matrix	WorldMatrixInverse = pTerrainTransform->Get_WorldMatrixInverse();

	_vector	vLocalPos = XMVector3TransformCoord(vPosition, WorldMatrixInverse);
	_float3 vLocalPosFloat3;
	XMStoreFloat3(&vLocalPosFloat3, vLocalPos);

	vPosition = XMVector3TransformCoord(vLocalPos, pTerrainTransform->Get_WorldMatrix());

	////////////////////////// 점프 멈춰!!!!!!!!////////////////////////
	_float fTerrainY = pVIBuffer_Terrain->Compute_Y(vLocalPosFloat3);

	if (0.4f < m_fJumpTime && fTerrainY + 0.5f > vLocalPosFloat3.y)
	{
		m_bJump = false;
		m_fJumpTime = 0.f;
	}
	else if (false == m_bJump && fTerrainY + 0.5f + 0.05f < vLocalPosFloat3.y)
	{
		m_bJump = true;
		m_fJumpTime = 0.4f;
	}
	/////////////////////////////////////////////////////////////////////

	if (false == m_bJump)
	{
		vPosition = XMVectorSetY(vPosition, fTerrainY + 0.5f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_float CItem_Weapon::TerrainY(_fvector vPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CItem_Weapon::TerrainY");
		RELEASE_INSTANCE(CGameInstance);
		return -5.f;
	}

	_vector	vPosition = vPos;

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CItem_Weapon::TerrainY");
		RELEASE_INSTANCE(CGameInstance);
		return -5.f;
	}

	// 로컬 포지션 구하기
	_matrix	WorldMatrixInverse = pTerrainTransform->Get_WorldMatrixInverse();

	_vector	vLocalPos = XMVector3TransformCoord(vPosition, WorldMatrixInverse);
	_float3 vLocalPosFloat3;
	XMStoreFloat3(&vLocalPosFloat3, vLocalPos);

	vPosition = XMVector3TransformCoord(vLocalPos, pTerrainTransform->Get_WorldMatrix());

	_float fTerrainY = pVIBuffer_Terrain->Compute_Y(vLocalPosFloat3);

	RELEASE_INSTANCE(CGameInstance);

	return fTerrainY + 0.5f;
}

CItem_Weapon * CItem_Weapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CItem_Weapon*	pInstance = new CItem_Weapon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CItem_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CItem_Weapon::Clone(void * pArg)
{
	CItem_Weapon*	pInstance = new CItem_Weapon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CItem_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
