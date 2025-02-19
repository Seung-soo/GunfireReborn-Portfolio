#include "stdafx.h"
#include "..\public\Item_Coin.h"
#include "GameInstance.h"
#include "Player_Hand.h"
#include "HierarchyNode.h"

CItem_Coin::CItem_Coin(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CItem_Coin::CItem_Coin(const CItem_Coin & rhs)
	: CGameObject(rhs)
{
}

HRESULT CItem_Coin::Set_Navigation(const _tchar * pPrototypeTag)
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

HRESULT CItem_Coin::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CItem_Coin::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItem_Coin::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 2.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CItem_Coin::NativeConstruct");
		return E_FAIL;
	}

	if (nullptr != pArg)
		memcpy(&m_tItemCoinDesc, pArg, sizeof(ITEMCOINDESC));

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CItem_Coin::NativeConstruct");
		return E_FAIL;
	}

	m_tItemCoinDesc.vTargetPos = XMVectorSetY(m_tItemCoinDesc.vTargetPos, TerrainY(m_tItemCoinDesc.vTargetPos));

	m_vCenterPoint = (m_tItemCoinDesc.vPosition * 0.7f + m_tItemCoinDesc.vTargetPos * 0.3f);
	XMVectorSetW(m_vCenterPoint, 1.f);
	_float fDist = XMVectorGetX(XMVector4Length(m_tItemCoinDesc.vTargetPos - m_tItemCoinDesc.vPosition));
	m_vCenterPoint = XMVectorSetY(m_vCenterPoint, XMVectorGetY(m_vCenterPoint) + fDist);

	m_pPlayerCollider = static_cast<CCollider*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Collider")));
	m_pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")));
	XMStoreFloat4(&m_vPosition, m_tItemCoinDesc.vPosition + XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPosition));

	return S_OK;
}

_int CItem_Coin::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CItem_Coin::Tick");
		return -1;
	}

	if (true == m_bDead)
		return DEAD;
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = (m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, -0.8f, 0.f, 0.f)) - vMyPos;
	_float fDistance = XMVectorGetX(XMVector4Length(vDir));

	if (1.0 > m_dLerpTime)
	{
		_float fDist = XMVectorGetX(XMVector4Length(m_tItemCoinDesc.vTargetPos - m_tItemCoinDesc.vPosition));

		m_fJumpTime += (_float)dTimeDelta * 2.f;
		m_dLerpTime += dTimeDelta * m_fJumpTime;

		if (1.0 <= m_dLerpTime)
		{
			m_dLerpTime = 1.0;
			m_bJump = false;
			m_fJumpTime = 0.f;
		}

		_vector vPos = XMVectorLerp(XMVectorLerp(m_tItemCoinDesc.vPosition, m_vCenterPoint, (_float)m_dLerpTime), XMVectorLerp(m_vCenterPoint, m_tItemCoinDesc.vTargetPos, (_float)m_dLerpTime), (_float)m_dLerpTime);

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
	else if (6.f > fDistance)
	{
		m_pTransformCom->Go_Vector(vDir, dTimeDelta * (6.f - fDistance) * 2.f, m_pNavigationCom);
		m_bJump = true;
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
		if (1.f > fDistance)
		{
			g_pGameInstance->StopSound(CSoundMgr::KILL_MONSTER);
			g_pGameInstance->LetsPlaySound(TEXT("EatItem.mp3"), CSoundMgr::KILL_MONSTER, 0.5f);
			static_cast<CPlayer_Hand*>(m_pPlayerCollider->Get_Parent())->Set_Money(m_tItemCoinDesc.iMoney);
			m_bDead = true;
		}
	}

	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

	return _int();
}

_int CItem_Coin::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CItem_Coin::LateTick");
		return -1;
	}
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	if (false == m_bJump && CCell::TYPE_ROAD == m_pNavigationCom->Get_Type())
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos = XMVectorSetY(vPos, m_pNavigationCom->Compute_Y(vPos));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (FAILED(SetUp_OnTerrain()))
	{
		MSGBOX("__super::SetUp_OnTerrain returned E_FAIL in CItem_Coin::LateTick");
		return DEAD;
	}

	// 절두체
	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return _int();
}

HRESULT CItem_Coin::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CItem_Coin::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CItem_Coin::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 28, i);
	}

	return S_OK;
}

HRESULT CItem_Coin::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CItem_Coin::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CItem_Coin::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Coin"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CItem_Coin::SetUp_Components(Model)");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(0.3f, 0.3f, 0.3f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.15f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CItem_Coin::Set_Collider(Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItem_Coin::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CItem_Coin::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CItem_Coin::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Coin::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Coin::SetUp_ConstantTable(Proj)");
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
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Coin::SetUp_ConstantTable(g_vLightDir)");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Coin::SetUp_ConstantTable(g_vLightDiffuse)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Coin::SetUp_ConstantTable(g_LightAmbient)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Coin::SetUp_ConstantTable(g_vLightSpecular)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//{
	//	MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CItem_Coin::SetUp_ConstantTable(g_vCamPosition)");
	//	return E_FAIL;
	//}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CItem_Coin::SetUp_OnTerrain()
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

	if (0.4f < m_fJumpTime && fTerrainY > vLocalPosFloat3.y)
	{
		m_bJump = false;
		m_fJumpTime = 0.f;
	}
	else if (false == m_bJump && fTerrainY + 0.05f < vLocalPosFloat3.y)
	{
		m_bJump = true;
		m_fJumpTime = 0.4f;
	}
	/////////////////////////////////////////////////////////////////////

	if (false == m_bJump)
	{
		vPosition = XMVectorSetY(vPosition, fTerrainY);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_float CItem_Coin::TerrainY(_fvector vPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CItem_Coin::TerrainY");
		RELEASE_INSTANCE(CGameInstance);
		return -5.f;
	}

	_vector	vPosition = vPos;

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CItem_Coin::TerrainY");
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

	return fTerrainY;
}

CItem_Coin * CItem_Coin::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CItem_Coin*	pInstance = new CItem_Coin(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CItem_Coin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CItem_Coin::Clone(void * pArg)
{
	CItem_Coin*	pInstance = new CItem_Coin(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CItem_Coin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_Coin::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
