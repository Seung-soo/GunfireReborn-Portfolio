#include "..\public\Light.h"
#include "GameInstance.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CLight::NativeConstruct(const LIGHTDESC & LightDesc)
{
	memcpy(&m_LightDesc, &LightDesc, sizeof(LIGHTDESC));

	return S_OK;
}

HRESULT CLight::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	
	if (nullptr != m_LightDesc.pPosition)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (DISJOINT == pGameInstance->Frustum(XMLoadFloat4(m_LightDesc.pPosition), 3.f))
		{
			RELEASE_INSTANCE(CGameInstance);
			return S_OK;
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	
	if (nullptr == pShader)
	{
		MSGBOX("nullptr == pShader in CLight::Render");
		return E_FAIL;
	}

	_uint		iPassIndex = 0;

	if (LIGHTDESC::TYPE_DIRECTTIONAL == m_LightDesc.eLightType)
	{
		if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
		{
			MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightDir)");
			return E_FAIL;
		}
		iPassIndex = 1;
	}
	else
	{
		if (LIGHTDESC::TYPE_POINTCHASE == m_LightDesc.eLightType)
		{
			if (FAILED(pShader->Set_RawValue("g_vLightPos", m_LightDesc.pPosition, sizeof(_float4))))
			{
				MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightPos)");
				return E_FAIL;
			}
		}
		else if (FAILED(pShader->Set_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
		{
			MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightPos)");
			return E_FAIL;
		}
		if (FAILED(pShader->Set_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
		{
			MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_fLightRange)");
			return E_FAIL;
		}
		iPassIndex = 2;
	}

	if (FAILED(pShader->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightDiffuse)");
		return E_FAIL;
	}
	if (FAILED(pShader->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightAmbient)");
		return E_FAIL;
	}
	//if (FAILED(pShader->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
	//{
	//	MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightSpecular)");
	//	return E_FAIL;
	//}

	return pVIBuffer->Render(pShader, iPassIndex);
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*	pInstance = new CLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(LightDesc)))
	{
		MSGBOX("Failed to Created CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
