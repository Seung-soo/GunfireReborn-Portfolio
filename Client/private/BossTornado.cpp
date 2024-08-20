#include "stdafx.h"
#include "..\public\BossTornado.h"
#include "GameInstance.h"
#include "Easing_Function.h"
#include "Camera_Dynamic.h"

CBossTornado::CBossTornado(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CBossTornado::CBossTornado(const CBossTornado & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossTornado::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CBossTornado::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossTornado::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 10.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(360.0f);
	tTransformDesc.fScalingPerSec = 30.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CBossTornado::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CBossTornado::NativeConstruct");
		return E_FAIL;
	}

	CTransform*	pBossTransform = (CTransform*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pBossTransform->Get_State(CTransform::STATE_POSITION));

	g_pGameInstance->StopSound(CSoundMgr::MONSTER4);
	g_pGameInstance->LetsPlaySound(TEXT("Boss2StormStart.mp3"), CSoundMgr::MONSTER4, 0.7f);

	static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Set_HorizonShake(2.f, 1.5f);


	return S_OK;
}

_int CBossTornado::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{

		return DEAD;
	}
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CBossTornado::Tick");
		return E_FAIL;
	}

	m_fAddNoiseUV += _float(TimeDelta);
	m_fMoveUV += _float(TimeDelta) * 5.f;
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);
	if (1.f > m_fAddNoiseUV)
	{
		m_pTransformCom->Scaled(XMVectorSet(EaseOutSine(3.f, m_fAddNoiseUV, 1.f), EaseOutSine(3.f, m_fAddNoiseUV, 1.f), EaseOutSine(3.f, m_fAddNoiseUV, 1.f), 0.f));
	}
	else if (2.f > m_fAddNoiseUV)
	{
		m_pTransformCom->Scaled(XMVectorSet(3.f - EaseOutSine(2.f, m_fAddNoiseUV - 1.f, 1.f), /*3.f - EaseOutSine(1.5f, m_fAddNoiseUV - 1.f, 1.f)*/3.f, 3.f - EaseOutSine(2.f, m_fAddNoiseUV - 1.f, 1.f), 0.f));
		m_pTransformCom->Go_Down(TimeDelta);
	}
	else if (2.5f > m_fAddNoiseUV)
	{
		if (false == m_bSound)
		{
			g_pGameInstance->StopSound(CSoundMgr::MONSTER3);
			g_pGameInstance->LetsPlaySound(TEXT("Boss2StormFinal.mp3"), CSoundMgr::MONSTER3, 1.f);
			static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Set_HorizonShake(0.2f, 2.f);
			m_bSound = true;
		}
		m_pTransformCom->OneScaling(CTransform::STATE_RIGHT, TimeDelta);
		m_pTransformCom->OneScaling(CTransform::STATE_LOOK, TimeDelta);
		m_iPass = 18;
	}
	else
		return DEAD;


	return _int();
}

_int CBossTornado::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CBossTornado::LateTick");
		return E_FAIL;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}




HRESULT CBossTornado::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CBossTornado::Render");
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

HRESULT CBossTornado::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_TigerCannon::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_TigerCannon::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_NoiseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_NoiseTexture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Prototype_Component_Texture_NoiseFire)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE4_2, TEXT("Prototype_Component_Model_BossTornado"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CBullet_Tesla::SetUp_Components(Prototype_Component_Model_BossTornado)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossTornado::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CBossTornado::SetUp_ConstantTable");
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

	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 36)))
	{
		MSGBOX("m_pNoiseTextureCom->SetUp_OnShader returned E_FAIL in CWave::SetUp_ConstantTable(DiffuseTexture)");
		return E_FAIL;
	}
	_float	fMoveUVX = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &m_fMoveUV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("ShaderCom->Set_RawValue returned E_FAIL in CNoiseRect::SetUp_ConstantTable(g_fMoveNoise_X)");
		return E_FAIL;
	}
	_float fAddNoiseUV = m_fAddNoiseUV * 3.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &fAddNoiseUV, sizeof(_float))))
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

CBossTornado * CBossTornado::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossTornado*	pInstance = new CBossTornado(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBossTornado");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossTornado::Clone(void * pArg)
{
	CBossTornado*	pInstance = new CBossTornado(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBossTornado");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossTornado::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseTextureCom);
}
