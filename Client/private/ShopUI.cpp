#include "stdafx.h"
#include "..\public\ShopUI.h"
#include "GameInstance.h"
#include "Product.h"
#include "Cursor.h"
#include "Player_Hand.h"
#include "NumberUI.h"

CShopUI::CShopUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CShopUI::CShopUI(const CShopUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CShopUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopUI::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pBuy = (_bool*)pArg;

	m_fSizeX = 1000.f;
	m_fSizeY = 1000.f;
	m_fX = 640.f;
	m_fY = 360.f;
	m_fDepth = 4.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	m_pCoin = static_cast<CPlayer_Hand*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Player_Hand")))->Get_Money();

	CProduct::PRODUCTDESC tProductDesc;
	tProductDesc.fPositionX = 315.f;
	tProductDesc.fPositionY = 250.f;
	tProductDesc.iTexIndex = 0;
	tProductDesc.pCoin = m_pCoin;
	tProductDesc.pBuy = &m_pBuy[0];
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Product"), TEXT("Prototype_GameObject_Product"), &tProductDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CShopUI::NativeConstruct");
		return E_FAIL;
	}

	tProductDesc.fPositionX = 465.f;
	tProductDesc.iTexIndex = 1;
	tProductDesc.pBuy = &m_pBuy[1];
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Product"), TEXT("Prototype_GameObject_Product"), &tProductDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CShopUI::NativeConstruct");
		return E_FAIL;
	}

	tProductDesc.fPositionX = 615.f;
	tProductDesc.iTexIndex = 2;
	tProductDesc.pBuy = &m_pBuy[2];
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Product"), TEXT("Prototype_GameObject_Product"), &tProductDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CShopUI::NativeConstruct");
		return E_FAIL;
	}

	CNumberUI::NUMBERDESC tNumberDesc;
	tNumberDesc.eNumberType = CNumberUI::TYPE_COIN;
	tNumberDesc.fX = 1001.f;
	tNumberDesc.fY = 127.f;
	tNumberDesc.pCurrent = m_pCoin;
	tNumberDesc.bStopUI = true;
	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Shop_CoinNumber"), TEXT("Prototype_GameObject_NumberUI"), &tNumberDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CShopUI::NativeConstruct(CoinNumber)");
		return E_FAIL;
	}
	m_pNumber = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Shop_CoinNumber"));
	m_pRendererCom->Set_DontRenderUI(true);

	m_pCursor = CCursor::Create(m_pDevice, m_pDeviceContext);
	g_bCameraAction = true;
	// 커서 뜨게하고 상품들 배치하고 마우스 피킹 잘되는지 확인부터
	return S_OK;
}

_int CShopUI::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
		return -1;

	if (nullptr != m_pCursor)
		m_pCursor->Tick(dTimeDelta);

	if (true == m_bDead)
	{
		if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Product"))))
		{
			MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CShopUI::Tick(Product)");
			return DEAD;
		}
		if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Product_Info"))))
		{
			MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CShopUI::Tick(Product_Info)");
			return DEAD;
		}
		/*if (FAILED(g_pGameInstance->Release_LayerObject(g_eCurrentLevel, TEXT("Layer_Shop_CoinNumber"))))
		{
			MSGBOX("g_pGameInstance->Release_LayerObject returned E_FAIL in CShopUI::Tick(CoinNumber)");
			return DEAD;
		}*/
		m_pNumber->Set_Dead(true);

		return DEAD;
	}

	if (true == g_pGameInstance->Get_KeyUp(DIK_B))
	{
		m_bDead = true;
		m_pRendererCom->Set_DontRenderUI(false);
		g_bCameraAction = false;
	}

	return _int();
}

_int CShopUI::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STOPUI, this);

	return _int();
}

HRESULT CShopUI::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (nullptr != m_pCursor)
		m_pCursor->Render();

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CShopUI::Render");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CShopUI::Render(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CShopUI::Render(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CShopUI::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 3);

	return S_OK;
}

HRESULT CShopUI::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CShopUI::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CShopUI::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CShopUI::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Shop"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CShopUI::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

CShopUI * CShopUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CShopUI*	pInstance = new CShopUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CShopUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CShopUI::Clone(void * pArg)
{
	CShopUI*	pInstance = new CShopUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CShopUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShopUI::Free()
{
	__super::Free();

	Safe_Release(m_pCursor);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
