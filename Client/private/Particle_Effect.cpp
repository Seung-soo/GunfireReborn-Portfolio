#include "stdafx.h"
#include "..\public\Particle_Effect.h"
#include "GameInstance.h"
#include "Easing_Function.h"

CParticle_Effect::CParticle_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CParticle_Effect::CParticle_Effect(const CParticle_Effect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticle_Effect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CParticle_Effect::NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CParticle_Effect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;
	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CParticle_Effect::NativeConstruct");
		return E_FAIL;
	}

	memcpy(&m_tParticleDesc, pArg, sizeof(PARTICLEDESC));

	m_vStartPosition = XMLoadFloat4(m_tParticleDesc.pParentPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vStartPosition);

	m_TransformMatrix.reserve(m_tParticleDesc.eParticleNumber);
	m_Speeds.reserve(m_tParticleDesc.eParticleNumber);
	m_Directions.reserve(m_tParticleDesc.eParticleNumber);
	m_CurrentScale.reserve(m_tParticleDesc.eParticleNumber);

	if (false == m_tParticleDesc.bTrail)
	{
		for (_uint i = 0; i < (_uint)m_tParticleDesc.eParticleNumber; i++)
		{
			_float4x4 TransformMatrix;
			_float3 vPosition = _float3(_float(_float(rand() % (_int(m_tParticleDesc.fMakeRange * 100.f + 1.f))) * 0.01f - m_tParticleDesc.fMakeRange * 0.5f)
				, _float(_float(rand() % (_int(m_tParticleDesc.fMakeRange * 100.f + 1.f))) * 0.01f - m_tParticleDesc.fMakeRange * 0.5f)
				, _float(_float(rand() % (_int(m_tParticleDesc.fMakeRange * 100.f + 1.f))) * 0.01f - m_tParticleDesc.fMakeRange * 0.5f));
			m_FirstPositions.push_back(vPosition);

			XMStoreFloat4x4(&TransformMatrix, XMMatrixScaling(m_tParticleDesc.fOriginScale, m_tParticleDesc.fOriginScale, 1.f) * XMMatrixTranslation(vPosition.x + XMVectorGetX(m_vStartPosition), vPosition.y + XMVectorGetY(m_vStartPosition), vPosition.z + XMVectorGetZ(m_vStartPosition)));
			m_TransformMatrix.push_back(TransformMatrix);

			_float	fSpeed = rand() % _int(m_tParticleDesc.fMaxSpeed - m_tParticleDesc.fMinSpeed + 1.f) + m_tParticleDesc.fMinSpeed;
			m_Speeds.push_back(fSpeed);
			m_CurrentSpeeds.push_back(fSpeed);

			_vector	vDir;
			if (DIR_RANDOM == m_tParticleDesc.eDir)
				vDir = XMVector4Normalize(XMVectorSet(_float(rand() % 360 - 180), _float(rand() % 360 - 180), _float(rand() % 360 - 180), 0.f));
			else
				vDir = m_tParticleDesc.vDir;
			m_Directions.push_back(vDir);

			m_CurrentScale.push_back(m_tParticleDesc.fOriginScale);
		}
	}
	else
	{
		for (_uint i = 0; i < (_uint)m_tParticleDesc.eParticleNumber; i++)
		{
			_float4x4 TransformMatrix;
			_float3 vPosition = _float3(_float(_float(rand() % (_int(m_tParticleDesc.fMakeRange * 100.f + 1.f))) * 0.01f - m_tParticleDesc.fMakeRange * 0.5f)
				, _float(_float(rand() % (_int(m_tParticleDesc.fMakeRange * 100.f + 1.f))) * 0.01f - m_tParticleDesc.fMakeRange * 0.5f)
				, _float(_float(rand() % (_int(m_tParticleDesc.fMakeRange * 100.f + 1.f))) * 0.01f - m_tParticleDesc.fMakeRange * 0.5f));
			m_FirstPositions.push_back(vPosition);

			XMStoreFloat4x4(&TransformMatrix, XMMatrixScaling(m_tParticleDesc.fOriginScale, m_tParticleDesc.fOriginScale, 1.f) * XMMatrixTranslation(vPosition.x + 1000.f, vPosition.y + 1000.f, vPosition.z + 1000.f));
			m_TransformMatrix.push_back(TransformMatrix);

			_float	fSpeed = rand() % _int(m_tParticleDesc.fMaxSpeed - m_tParticleDesc.fMinSpeed + 1.f) + m_tParticleDesc.fMinSpeed;
			m_Speeds.push_back(fSpeed);
			m_CurrentSpeeds.push_back(fSpeed);

			_vector	vDir;
			if (DIR_RANDOM == m_tParticleDesc.eDir)
				vDir = XMVector4Normalize(XMVectorSet(_float(rand() % 360 - 180), _float(rand() % 360 - 180), _float(rand() % 360 - 180), 0.f));
			else
				vDir = m_tParticleDesc.vDir;
			m_Directions.push_back(vDir);

			m_CurrentScale.push_back(m_tParticleDesc.fOriginScale);
		}
	}
	

	switch (m_tParticleDesc.eEasing)
	{
	case INSINE:
		m_pEasingFunction = EaseInSine;
		break;
	case OUTSINE:
		m_pEasingFunction = EaseOutSine;
		break;
	case INOUTSINE:
		m_pEasingFunction = EaseInOutSine;
		break;
	case INQUAD:
		m_pEasingFunction = EaseInQuad;
		break;
	case OUTQUAD:
		m_pEasingFunction = EaseOutQuad;
		break;
	case INOUTQUAD:
		m_pEasingFunction = EaseInOutQuad;
		break;
	case INCUBIC:
		m_pEasingFunction = EaseInCubic;
		break;
	case OUTCUBIC:
		m_pEasingFunction = EaseOutCubic;
		break;
	case INOUTCUBIC:
		m_pEasingFunction = EaseInOutCubic;
		break;
	case INQUART:
		m_pEasingFunction = EaseInQuart;
		break;
	case OUTQUART:
		m_pEasingFunction = EaseOutQuart;
		break;
	case INOUTQUART:
		m_pEasingFunction = EaseInOutQuart;
		break;
	case INQUINT:
		m_pEasingFunction = EaseInQuint;
		break;
	case OUTQUINT:
		m_pEasingFunction = EaseOutQuint;
		break;
	case INOUTQUINT:
		m_pEasingFunction = EaseInOutQuint;
		break;
	case INEXPO:
		m_pEasingFunction = EaseInExpo;
		break;
	case OUTEXPO:
		m_pEasingFunction = EaseOutExpo;
		break;
	case INOUTEXPO:
		m_pEasingFunction = EaseInOutExpo;
		break;
	case INCIRC:
		m_pEasingFunction = EaseInCirc;
		break;
	case OUTCIRC:
		m_pEasingFunction = EaseOutCirc;
		break;
	case INOUTCIRC:
		m_pEasingFunction = EaseInOutCirc;
		break;
	case INBACK:
		m_pEasingFunction = EaseInBack;
		break;
	case OUTBACK:
		m_pEasingFunction = EaseOutBack;
		break;
	case INOUTBACK:
		m_pEasingFunction = EaseInOutBack;
		break;
	case INELASTIC:
		m_pEasingFunction = EaseInElastic;
		break; 
	case OUTELASTIC:
		m_pEasingFunction = EaseOutElastic;
		break;
	case INOUTELASTIC:
		m_pEasingFunction = EaseInOutElastic;
		break;
	case INBOUNCE:
		m_pEasingFunction = EaseInBounce;
		break;
	case OUTBOUNCE:
		m_pEasingFunction = EaseOutBounce;
		break;
	case INOUTBOUNCE:
		m_pEasingFunction = EaseInOutBounce;
		break;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CParticle_Effect::NativeConstruct");
		return E_FAIL;
	}
	return S_OK;
}

_int CParticle_Effect::Tick(_double TimeDelta)
{
	if (false == m_bParticleEnd && nullptr != m_tParticleDesc.pDead && true == *m_tParticleDesc.pDead)
		m_bParticleEnd = true;

	if (true == m_bDead)
		return DEAD;

	if (m_tParticleDesc.fLiveTime < m_fCurrentTime)
		return DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CParticle_Effect::Tick");
		return DEAD;
	}
	if (true == m_tParticleDesc.bUseEasing)
		UseEasingFuntion();


	if (false == m_tParticleDesc.bTrail)
	{
		for (_uint i = 0; i < (_uint)m_tParticleDesc.eParticleNumber; i++)
		{
			m_CurrentScale[i] -= m_tParticleDesc.fScaleSpeed * (_float)TimeDelta;

			if (0.f > m_CurrentScale[i])
				m_CurrentScale[i] = 0.f;

			_float4 vPosition = _float4(m_TransformMatrix[i]._41, m_TransformMatrix[i]._42, m_TransformMatrix[i]._43, 1.f);
			XMStoreFloat4(&vPosition, XMLoadFloat4(&vPosition) + (m_Directions[i] * m_CurrentSpeeds[i] + XMVectorSet(0.f, -1.f, 0.f, 0.f) * m_fCurrentTime * m_tParticleDesc.fGravityPower) * (_float)TimeDelta);

			if (true == m_tParticleDesc.bRoof)
			{
				if (true == m_tParticleDesc.bChase)
				{
					if (m_tParticleDesc.fMaxDistance < DISTANCE(XMLoadFloat4(&vPosition) - XMLoadFloat4(m_tParticleDesc.pParentPosition)) || 0.f == m_CurrentScale[i])
					{
						XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixScaling(m_tParticleDesc.fOriginScale, m_tParticleDesc.fOriginScale, 1.f) * XMMatrixTranslation(m_FirstPositions[i].x + (*m_tParticleDesc.pParentPosition).x, m_FirstPositions[i].y + (*m_tParticleDesc.pParentPosition).y, m_FirstPositions[i].z + (*m_tParticleDesc.pParentPosition).z));
						m_CurrentScale[i] = m_tParticleDesc.fOriginScale;
					}
					else
						XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixScaling(m_CurrentScale[i], m_CurrentScale[i], 1.f) * XMMatrixTranslation(vPosition.x, vPosition.y, vPosition.z));
				}
				else
				{
					if (m_tParticleDesc.fMaxDistance < DISTANCE(XMLoadFloat4(&vPosition) - m_vStartPosition) || 0.f == m_CurrentScale[i])
					{
						XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixScaling(m_tParticleDesc.fOriginScale, m_tParticleDesc.fOriginScale, 1.f) * XMMatrixTranslation(m_FirstPositions[i].x + m_vStartPosition.m128_f32[0], m_FirstPositions[i].y + m_vStartPosition.m128_f32[1], m_FirstPositions[i].z + m_vStartPosition.m128_f32[2]));
						m_CurrentScale[i] = m_tParticleDesc.fOriginScale;
					}
					else
						XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixScaling(m_CurrentScale[i], m_CurrentScale[i], 1.f) * XMMatrixTranslation(vPosition.x, vPosition.y, vPosition.z));
				}
			}
			else
			{
				if (m_tParticleDesc.fMaxDistance < DISTANCE(XMLoadFloat4(&vPosition) - m_vStartPosition) || 0.f == m_CurrentScale[i])
				{
					XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixTranslation(1000.f, 1000.f, 1000.f));
				}
				else
					XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixScaling(m_CurrentScale[i], m_CurrentScale[i], 1.f) * XMMatrixTranslation(vPosition.x, vPosition.y, vPosition.z));
			}

		}
	}

	return NOPROBLEM;
}

_int CParticle_Effect::LateTick(_double TimeDelta)
{
	if (false == m_bParticleEnd && nullptr != m_tParticleDesc.pDead && true == *m_tParticleDesc.pDead)
		m_bParticleEnd = true;

	if (true == m_bDead)
		return DEAD;

	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CParticle_Effect::LateTick");
		return DEAD;
	}

	if (true == m_tParticleDesc.bTrail)
	{
		if (m_tParticleDesc.fTrailInterval < m_fTrailTime)
		{
			_float4 vPosition;
			if (true == m_bParticleEnd)
			{
				if (m_iTrailNum == m_iEndCycle)
					m_bDead = true;
				if (false == m_bEndStart)
				{
					XMStoreFloat4(&vPosition, XMVectorSet(1000.f, 1000.f, 1000.f, 1.f));
					m_iEndCycle = m_iTrailNum;
					m_bEndStart = true;
				}

			}
			else
				XMStoreFloat4(&vPosition, XMLoadFloat4(m_tParticleDesc.pParentPosition) + XMLoadFloat3(&m_FirstPositions[m_iTrailNum]));
			m_CurrentScale[m_iTrailNum] = m_tParticleDesc.fOriginScale;
			m_CurrentSpeeds[m_iTrailNum] = m_Speeds[m_iTrailNum];
			XMStoreFloat4x4(&m_TransformMatrix[m_iTrailNum], XMMatrixScaling(m_CurrentScale[m_iTrailNum], m_CurrentScale[m_iTrailNum], 1.f) * XMMatrixTranslation(vPosition.x, vPosition.y, vPosition.z));

			m_fTrailTime = 0.f;
			m_iTrailNum++;
			if (m_tParticleDesc.eParticleNumber <= m_iTrailNum)
				m_iTrailNum = 0;


		}
		
		for (_uint i = 0; i < (_uint)m_tParticleDesc.eParticleNumber; i++)
		{
			m_CurrentScale[i] -= m_tParticleDesc.fScaleSpeed * (_float)TimeDelta;

			if (0.f > m_CurrentScale[i])
				m_CurrentScale[i] = 0.f;

			_float4 vPosition = _float4(m_TransformMatrix[i]._41, m_TransformMatrix[i]._42, m_TransformMatrix[i]._43, 1.f);
			XMStoreFloat4(&vPosition, XMLoadFloat4(&vPosition) + (m_Directions[i] * m_CurrentSpeeds[i] + XMVectorSet(0.f, -1.f, 0.f, 0.f) * m_fCurrentTime * m_tParticleDesc.fGravityPower) * (_float)TimeDelta);

			XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixScaling(m_CurrentScale[i], m_CurrentScale[i], 1.f) * XMMatrixTranslation(vPosition.x, vPosition.y, vPosition.z));
		}
	}

	m_pVIBufferCom->UpdateParticle(m_TransformMatrix, TimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	m_fCurrentTime += (_float)TimeDelta;
	m_fTrailTime += (_float)TimeDelta;

	return _int();
}

HRESULT CParticle_Effect::Render()
{
	if (nullptr == m_pVIBufferCom)
	{
		MSGBOX("nullptr == m_pVIBufferCom in CParticle_Effect::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CParticle_Effect::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 1)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CParticle_Effect::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CParticle_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CParticle_Effect::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_PointInstance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CParticle_Effect::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/*For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Particle_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CParticle_Effect::SetUp_Components(Texture)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	switch (m_tParticleDesc.eParticleNumber)
	{
	case NUM_10:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_10"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, nullptr)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CParticle_Effect::SetUp_Components(VIBuffer)");
			return E_FAIL;
		}
		break;
	case NUM_30:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_30"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, nullptr)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CParticle_Effect::SetUp_Components(VIBuffer)");
			return E_FAIL;
		}
		break;
	case NUM_50:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_50"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, nullptr)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CParticle_Effect::SetUp_Components(VIBuffer)");
			return E_FAIL;
		}
		break;
	case NUM_100:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_100"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, nullptr)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CParticle_Effect::SetUp_Components(VIBuffer)");
			return E_FAIL;
		}
		break;
	}




	return S_OK;
}

HRESULT CParticle_Effect::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(View)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_tParticleDesc.iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
		return E_FAIL;
	}

	_float4		vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_vCamPosition)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CParticle_Effect::UseEasingFuntion()
{
	for (_uint i = 0; i < (_uint)m_tParticleDesc.eParticleNumber; i++)
	{
		m_CurrentSpeeds[i] = m_Speeds[i] - m_pEasingFunction(m_Speeds[i], m_fCurrentTime, m_tParticleDesc.fLiveTime);
	}
}

CParticle_Effect * CParticle_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CParticle_Effect*	pInstance = new CParticle_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CParticle_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CParticle_Effect::Clone(void * pArg)
{
	CParticle_Effect*	pInstance = new CParticle_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CParticle_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticle_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
