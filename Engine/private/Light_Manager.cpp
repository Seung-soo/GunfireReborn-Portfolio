#include "..\public\Light_Manager.h"
#include "Light.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Target_Manager.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CLight_Manager)

const LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex) const
{
	if (iIndex >= m_Lights.size())
	{
		MSGBOX("iIndex >= m_Lights.size in CLight_Manager::Get_LightDesc");
		return nullptr;
	}

	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	m_pVIBuffer = CVIBuffer_Rect::Create(pDevice, pDeviceContext);
	if (nullptr == m_pVIBuffer)
	{
		MSGBOX("nullptr == m_pVIBuffer in CLight_Manager::Initialize");
		return E_FAIL;
	}

	m_pShader = CShader::Create(pDevice, pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pShader in CLight_Manager::Initialize");
		return E_FAIL;
	}
	// 직교 행렬 세팅
	_uint			iNumViewports = 1;
	D3D11_VIEWPORT	Viewport;

	pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = Viewport.Width;
	m_WorldMatrix._22 = Viewport.Height;
	m_WorldMatrix._33 = 1.f;
	memcpy(&m_WorldMatrix.m[3][0], &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(pDevice, pDeviceContext, LightDesc);
	if (nullptr == pLight)
	{
		MSGBOX("pLight is nullptr in CLight_Manager::Add_Light");
		return E_FAIL;
	}

	m_Lights.push_back(pLight);

	return S_OK;
}

void CLight_Manager::Update()
{
	for (auto iter = m_Lights.begin(); iter != m_Lights.end();)
	{
		if (nullptr != (*iter)->Get_LightDesc()->pDead && true == *(*iter)->Get_LightDesc()->pDead)
		{
			Safe_Release((*iter));
			iter = m_Lights.erase(iter);
			continue;
		}
		else if (iter != m_Lights.end())
			++iter;
	}
}

HRESULT CLight_Manager::Render()
{
	if (nullptr == m_pShader || nullptr == m_pVIBuffer)
	{
		MSGBOX("nullptr == m_pShader || nullptr == m_pVIBuffer in CLight_Manager::Render");
		return E_FAIL;
	}

	CTarget_Manager*	pTarget_Manager = GET_INSTANCE(CTarget_Manager);

	// 노말타겟의 쉐이더 리소스 뷰를 가져와서 노말타겟 텍스쳐 생성
	if (FAILED(m_pShader->Set_Texture("g_NormalTexture", pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CLight_Manager::Render(g_NormalTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_DepthTexture", pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CLight_Manager::Render(g_DepthTexture)");
		return E_FAIL;
	}

	// 쉐이더에 행렬 세팅
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CLight_Manager::Render(g_WorldMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CLight_Manager::Render(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CLight_Manager::Render(g_ProjMatrix)");
		return E_FAIL;
	}

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4		ViewMatrixInv, ProjMatrixInv;
	_float4			vCamPosition;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))));
	XMStoreFloat4(&vCamPosition, pPipeLine->Get_CamPosition());

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CLight_Manager::Render(g_ViewMatrixInv)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CLight_Manager::Render(g_ProjMatrixInv)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CLight_Manager::Render(g_vCamPosition)");
		return E_FAIL;
	}


	RELEASE_INSTANCE(CPipeLine);

	CLight*	pDirectionalLight = nullptr;

	auto iter = m_Lights.begin();
	// 라이트의 랜더 호출
	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight)
		{
			if (LIGHTDESC::TYPE_DIRECTTIONAL == pLight->Get_LightDesc()->eLightType)
			{
				pDirectionalLight = pLight;
				continue;
			}
			pLight->Render(m_pShader, m_pVIBuffer);
		}
	}

	if (nullptr != pDirectionalLight)
	{
		pDirectionalLight->Render(m_pShader, m_pVIBuffer);
	}


	RELEASE_INSTANCE(CTarget_Manager);

	return S_OK;
}

HRESULT CLight_Manager::Release_Light()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);
	m_Lights.clear();

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);
	m_Lights.clear();

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
}
