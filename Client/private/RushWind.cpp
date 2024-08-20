#include "stdafx.h"
#include "..\public\RushWind.h"
#include "GameInstance.h"
#include "Easing_Function.h"
#include "HierarchyNode.h"
#include "Particle_Effect.h"

CRushWind::CRushWind(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CRushWind::CRushWind(const CRushWind & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRushWind::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRushWind::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(1600.f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CRushWind::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CRushWind::NativeConstruct");
		return E_FAIL;
	}

	if (nullptr != pArg)
		memcpy(&m_tRushWindDesc, pArg, sizeof(RUSHWINDDESC));



	CHierarchyNode* pNode = m_tRushWindDesc.pModel->Find_HierarchyNode(m_tRushWindDesc.pSocketName);
	m_pCombineMatrix = pNode->Get_CombinedTransformationFloat4x4();
	m_pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	m_PivotMatrix = XMLoadFloat4x4(&m_tRushWindDesc.pModel->Get_TransformMatrix());
	m_TransformMatrix = m_tRushWindDesc.pTransform->Get_WorldMatrix();

	_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(m_tRushWindDesc.pTransform->Get_State(CTransform::STATE_LOOK)));
	m_pTransformCom->ArrangeMatrix();

	//m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-5.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 10.f);
	m_pTransformCom->Scaled(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));


	// 물튀기기 파티클
	_float4	vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 4.f + XMVectorSet(0.f, -1.5f, 0.f, 0.f));

	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 9;
	tParticleDesc.fMinSpeed = 10.f;
	tParticleDesc.fMaxSpeed = 20.f;
	tParticleDesc.fOriginScale = 0.3f;
	tParticleDesc.fScaleSpeed = 0.19f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.fMakeRange = 4.f;
	tParticleDesc.fMaxDistance = 20.f;
	tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
	tParticleDesc.pParentPosition = &vPosition;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eEasing = CParticle_Effect::INCUBIC;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead = nullptr;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}
	tParticleDesc.iTexIndex = 10;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_10;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CWave::NativeConstruct");
		return E_FAIL;
	}
	return S_OK;
}

_int CRushWind::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CRushWind::Tick");
		return DEAD;
	}

	if (true == m_bDead)
		return DEAD;

	//if (true == g_pGameInstance->Get_KeyEnter(DIK_RIGHT))
	//{
	//	m_iTexIndex++;
	//	if (m_iTexIndex > 98)
	//		m_iTexIndex = 0;
	//}
	//else if (true == g_pGameInstance->Get_KeyEnter(DIK_LEFT))
	//{
	//	m_iTexIndex--;
	//	if (m_iTexIndex < 0)
	//		m_iTexIndex = 98;
	//}

	//if (true == g_pGameInstance->Get_KeyEnter(DIK_UP))
	//	m_iSpeed++;
	//else if (true == g_pGameInstance->Get_KeyEnter(DIK_DOWN))
	//	m_iSpeed--;

	m_fMoveUV += EaseOutSine(8.f, m_fLiveTime, 1.f);
	if (false == m_bBye)
		m_fMoveUV -= (_int)m_fMoveUV;
	else
	{
		if (1.f <= m_fMoveUV)
			return DEAD;
	}

	m_fScale = EaseOutSine(6.f, m_fLiveTime, 1.f) + 0.5f;
	m_pTransformCom->Go_Backward(TimeDelta * m_fSpeed);
	m_fLiveTime += (_float)TimeDelta;
	m_pTransformCom->Scaled(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));

	if (0.2f < m_fLiveTime)
	{
		m_bBye = true;
		m_iPass = 12;
	}


	return _int();
}

_int CRushWind::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CRushWind::LateTick");
		return DEAD;
	}

	//m_TransformMatrix = m_tRushWindDesc.pTransform->Get_WorldMatrix();


	//_matrix TransformMatrix = XMMatrixTranspose(XMLoadFloat4x4(m_pOffsetMatrix)) * XMLoadFloat4x4(m_pCombineMatrix) * m_PivotMatrix * m_TransformMatrix;
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}




HRESULT CRushWind::Render()
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CRushWind::Render");
		return E_FAIL;
	}


	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CRushWind::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i);
	}


	return S_OK;
}

HRESULT CRushWind::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRushWind::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRushWind::SetUp_Components(Shader");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE3_2, TEXT("Prototype_Component_Model_RushWind"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CRushWind::SetUp_Components(Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRushWind::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CRushWind::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CRushWind::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRushWind::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRushWind::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	_float fX = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &fX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRushWind::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CRushWind::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



CRushWind * CRushWind::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRushWind*	pInstance = new CRushWind(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CRushWind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRushWind::Clone(void * pArg)
{
	CRushWind*	pInstance = new CRushWind(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CRushWind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRushWind::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
