#include "stdafx.h"
#include "..\public\BearAttack.h"
#include "GameInstance.h"
#include "Particle_Effect.h"
#include "BearAttackSmoke.h"

CBearAttack::CBearAttack(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBearAttack::CBearAttack(const CBearAttack & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBearAttack::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBearAttack::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBearAttack::NativeConstruct");
		return E_FAIL;
	}

	CTransform*	pParentTransform = (CTransform*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pParentTransform->Get_State(CTransform::STATE_POSITION) + pParentTransform->Get_State(CTransform::STATE_LOOK) * 5.f);
	m_pTransformCom->Scaled(XMVectorSet(2.f, m_fScale, 2.f, 0.f));

	_float4 vPosition;

	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	// 공격 파티클
	CParticle_Effect::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex = 24;
	tParticleDesc.fMinSpeed = 20.f;
	tParticleDesc.fMaxSpeed = 30.f;
	tParticleDesc.fOriginScale = 0.4f;
	tParticleDesc.fScaleSpeed = 0.f;
	tParticleDesc.fGravityPower = 80.f;
	tParticleDesc.fLiveTime = 0.33f;
	tParticleDesc.fMakeRange = 0.f;
	tParticleDesc.fMaxDistance = 5.48f;
	tParticleDesc.eDir = CParticle_Effect::DIR_RANDOM;
	tParticleDesc.pParentPosition = &vPosition;
	tParticleDesc.eParticleNumber = CParticle_Effect::NUM_100;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CParticle_Effect::OUTEXPO;
	tParticleDesc.bChase = false;
	tParticleDesc.bRoof = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.pDead = nullptr;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Particle_Effect"), TEXT("Prototype_GameObject_Particle_Effect"), &tParticleDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CBearAttack::NativeConstruct");
		return E_FAIL;
	}

	MakeSmoke();

	return S_OK;
}

_int CBearAttack::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return DEAD;

	m_fAlpha -= 800.f * (_float)TimeDelta;

	m_fScale += (_float)TimeDelta * 20.f;
	if (2.f < m_fScale)
		m_fScale = 2.f;
	m_pTransformCom->Scaled(XMVectorSet(2.f, m_fScale, 2.f, 0.f));

	if (0.f > m_fAlpha)
		return DEAD;

	return _int();
}

_int CBearAttack::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return DEAD;

	if (DISJOINT != g_pGameInstance->Frustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}
	return NOPROBLEM;
}




HRESULT CBearAttack::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBearAttack::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 10, i);
	}

	return S_OK;
}

HRESULT CBearAttack::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBearAttack::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBearAttack::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_1, TEXT("Prototype_Component_Model_BearAttack"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBearAttack::SetUp_Components(Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBearAttack::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBearAttack::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CBearAttack::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBearAttack::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBearAttack::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	_float fAlpha = m_fAlpha;
	if (255.f < fAlpha)
		fAlpha = 255.f;
	fAlpha /= 255.f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &fAlpha, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CBearAttack::SetUp_ConstantTable(Alpha)");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CBearAttack::MakeSmoke()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	CBearAttackSmoke::SMOKEDESC tSmokeDesc;
	tSmokeDesc.pTransform = m_pTransformCom;
	tSmokeDesc.vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BearAttackSmoke"), &tSmokeDesc)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBearAttack::MakeSmoke");
		return;
	}

	tSmokeDesc.vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
	if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BearAttackSmoke"), &tSmokeDesc)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBearAttack::MakeSmoke");
		return;
	}

	tSmokeDesc.vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BearAttackSmoke"), &tSmokeDesc)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBearAttack::MakeSmoke");
		return;
	}

	tSmokeDesc.vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BearAttackSmoke"), &tSmokeDesc)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBearAttack::MakeSmoke");
		return;
	}

	tSmokeDesc.vDir = XMVectorSet(1.f, 0.f, 1.f, 0.f);
	if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BearAttackSmoke"), &tSmokeDesc)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBearAttack::MakeSmoke");
		return;
	}

	tSmokeDesc.vDir = XMVectorSet(-1.f, 0.f, -1.f, 0.f);
	if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BearAttackSmoke"), &tSmokeDesc)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBearAttack::MakeSmoke");
		return;
	}

	tSmokeDesc.vDir = XMVectorSet(1.f, 0.f, -1.f, 0.f);
	if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BearAttackSmoke"), &tSmokeDesc)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBearAttack::MakeSmoke");
		return;
	}

	tSmokeDesc.vDir = XMVectorSet(-1.f, 0.f, 1.f, 0.f);
	if (FAILED(pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BearAttackSmoke"), &tSmokeDesc)))
	{
		MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CBearAttack::MakeSmoke");
		return;
	}

	RELEASE_INSTANCE(CGameInstance);
}



CBearAttack * CBearAttack::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBearAttack*	pInstance = new CBearAttack(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBearAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBearAttack::Clone(void * pArg)
{
	CBearAttack*	pInstance = new CBearAttack(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBearAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBearAttack::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
