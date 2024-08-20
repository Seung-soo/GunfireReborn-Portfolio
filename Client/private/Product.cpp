#include "stdafx.h"
#include "..\public\Product.h"
#include "GameInstance.h"
#include "ProductInfo.h"
#include "Player_Hand.h"

CProduct::CProduct(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CProduct::CProduct(const CProduct & rhs)
	: CGameObject(rhs)
{
}

HRESULT CProduct::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProduct::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_tProductDesc, pArg, sizeof(PRODUCTDESC));

	m_fSizeX = 200.f;
	m_fSizeY = 200.f;
	m_fX = m_tProductDesc.fPositionX;
	m_fY = m_tProductDesc.fPositionY;
	m_fDepth = 3.f;
	m_iTexIndex = m_tProductDesc.iTexIndex;

	if (0 == m_iTexIndex)
		m_iPrice = 80;
	else if (1 == m_iTexIndex || 2 == m_iTexIndex)
		m_iPrice = 90;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	return S_OK;
}

_int CProduct::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	// 상품의 상태에 따라 다른 텍스쳐 인덱스
	if (true == *m_tProductDesc.pBuy)
		m_iTexIndex = m_tProductDesc.iTexIndex + 3;

	if (g_pGameInstance->Get_KeyEnter(DIK_G))
	{
		*m_tProductDesc.pCoin = 9999;
	}
	
	// 상품 피킹
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (m_fX - m_fSizeX * 0.2f <= ptMouse.x && m_fX + m_fSizeX * 0.2f >= ptMouse.x && m_fY - m_fSizeY * 0.3f <= ptMouse.y && m_fY + m_fSizeY * 0.3f >= ptMouse.y)
	{
		m_fSizeX = 230.f;
		m_fSizeY = 230.f;
		if (true == m_bStart)
		{
			// 상품 설명 생성
			CProductInfo::PRODUCTINFODESC tProductInfoDesc;
			tProductInfoDesc.pBuy = m_tProductDesc.pBuy;
			tProductInfoDesc.pCoin = m_tProductDesc.pCoin;
			tProductInfoDesc.pPrice = &m_iPrice;
			tProductInfoDesc.pTexIndex = &m_iTexIndex;

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Product_Info"), TEXT("Prototype_GameObject_Product_Info"), &tProductInfoDesc)))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CProduct::Tick");
				return DEAD;
			}
			m_pProductInfo = static_cast<CProductInfo*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Product_Info")));
			m_bStart = false;
		}

		if (true == g_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
		{
			m_fSizeX = 230.f;
			m_fSizeY = 230.f;

			if (m_iPrice <= *m_tProductDesc.pCoin && false == *m_tProductDesc.pBuy)
			{
				*m_tProductDesc.pBuy = true;
				*m_tProductDesc.pCoin -= m_iPrice;

				if (0 == m_iTexIndex)
					static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->HealHp();
				else if (1 == m_iTexIndex)
					static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->FullBullet();
				else if (2 == m_iTexIndex)
					static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->FullSmoke();
			}

		}
		else if (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
		{
			m_fSizeX = 180.f;
			m_fSizeY = 180.f;
		}
	}
	else if (false == m_bStart)
	{
		if (nullptr != m_pProductInfo)
			m_pProductInfo->Set_Dead(true);

		m_fSizeX = 200.f;
		m_fSizeY = 200.f;
		// 상품 설명 싹다 릴리즈
		m_bStart = true;
	}

	return _int();
}

_int CProduct::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;
	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STOPUI, this);

	return _int();
}

HRESULT CProduct::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CProduct::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CProduct::Render(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CProduct::Render(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CProduct::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 3);

	return S_OK;
}

HRESULT CProduct::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CProduct::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CProduct::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CProduct::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Product"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CProduct::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CProduct * CProduct::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CProduct*	pInstance = new CProduct(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CProduct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CProduct::Clone(void * pArg)
{
	CProduct*	pInstance = new CProduct(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CProduct");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProduct::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
