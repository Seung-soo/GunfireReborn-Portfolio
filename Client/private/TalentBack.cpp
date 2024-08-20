#include "stdafx.h"
#include "..\public\TalentBack.h"
#include "GameInstance.h"
#include "Player_Hand.h"
#include "Talent.h"
#include "Cursor.h"

CTalentBack::CTalentBack(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTalentBack::CTalentBack(const CTalentBack & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTalentBack::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTalentBack::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fX = 640.f;
	m_fY = 360.f;
	m_fSizeX = 960.f;
	m_fSizeY = 960.f;
	m_fDepth = 4.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));
	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	m_pTalents = static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_Talents();
	m_pRendererCom->Set_DontRenderUI(true);

	m_pCursor = CCursor::Create(m_pDevice, m_pDeviceContext);
	g_bCameraAction = true;
	return S_OK;
}

_int CTalentBack::Tick(_double dTimeDelta)
{
	if (nullptr != m_pCursor)
		m_pCursor->Tick(dTimeDelta);

	if (true == m_bDead)
	{
		if (nullptr != m_pTalentObject[0] && true == m_pTalentObject[0]->Get_Pick())
		{
			if (nullptr != m_pTalentObject[1])
			{
				m_pTalents[m_pTalentObject[1]->Get_TexIndex()] = false;
				m_pTalentObject[1]->Set_Dead(true);
				m_pTalentObject[1] = nullptr;
			}
			m_pTalents[m_pTalentObject[0]->Get_TexIndex()] = true;
		}
		else if (nullptr != m_pTalentObject[1] && true == m_pTalentObject[1]->Get_Pick())
		{
			if (nullptr != m_pTalentObject[0])
			{
				m_pTalents[m_pTalentObject[0]->Get_TexIndex()] = false;
				m_pTalentObject[0]->Set_Dead(true);
				m_pTalentObject[0] = nullptr;
			}
			m_pTalents[m_pTalentObject[1]->Get_TexIndex()] = true;
		}
		Safe_Release(m_pCursor);
		return DEAD;
	}

	if (0 > __super::Tick(dTimeDelta))
		return -1;

	for (_uint i = 0; i < 6; i++)
	{
		if (false == m_pTalents[i])
			break;
		if (5 == i)
			return NOPROBLEM;
	}

	if (true == m_bStart && 4 <= m_iTexIndex)
	{
		_uint iTalentIndex = 0;
		while (true == m_pTalents[iTalentIndex])
		{
			iTalentIndex += 2;
			if (5 < iTalentIndex)
			{
				if (iTalentIndex % 2 == 0)
					iTalentIndex = 1;
				else
					iTalentIndex = 0;
			}
		}

		m_pTalents[iTalentIndex] = true;

		INFO_UI tInfoUI;
		tInfoUI.fPositionX = 490.f;
		tInfoUI.fPositionY = 360.f;
		tInfoUI.iTextureIndex = iTalentIndex;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Talent"), TEXT("Prototype_GameObject_Talent"), &tInfoUI)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTalentBack::Tick");
			return E_FAIL;
		}
		m_pTalentObject[0] = static_cast<CTalent*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Talent")));


		for (_uint i = 0; i < 6; i++)
		{
			if (false == m_pTalents[i])
				break;
			if (5 == i)
				return NOPROBLEM;
		}

		iTalentIndex = 5;
		while (true == m_pTalents[iTalentIndex])
		{
			iTalentIndex -= 2;
			if (0 > iTalentIndex)
			{
				if (iTalentIndex % 2 == 0)
					iTalentIndex = 5;
				else
					iTalentIndex = 4;
			}
		}

		m_pTalents[iTalentIndex] = true;


		tInfoUI.fPositionX = 790.f;
		tInfoUI.fPositionY = 360.f;
		tInfoUI.iTextureIndex = iTalentIndex;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Talent"), TEXT("Prototype_GameObject_Talent"), &tInfoUI)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CTalentBack::Tick");
			return E_FAIL;
		}
		m_pTalentObject[1] = static_cast<CTalent*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Talent")));
		m_bStart = false;
	}

	return _int();
}

_int CTalentBack::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	if (4.0 > m_dTime)
		m_dTime += 20.0 * dTimeDelta;
	m_iTexIndex = (_uint)m_dTime;

	if (m_iTexIndex > 4)
		m_iTexIndex = 4;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STOPUI, this);


	return _int();
}

HRESULT CTalentBack::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;
	if (nullptr != m_pCursor)
		m_pCursor->Render();

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CTalentBack::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTalentBack::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CTalentBack::Render(g_ProjMatrix)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CTalentBack::Render");
		return E_FAIL;
	}


	m_pVIBufferCom->Render(m_pShaderCom, 16);



	return S_OK;
}

HRESULT CTalentBack::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalentBack::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalentBack::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalentBack::SetUp_Components(ViBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TalentBack"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CTalentBack::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CTalentBack * CTalentBack::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTalentBack*	pInstance = new CTalentBack(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTalentBack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTalentBack::Clone(void * pArg)
{
	CTalentBack*	pInstance = new CTalentBack(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTalentBack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalentBack::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
