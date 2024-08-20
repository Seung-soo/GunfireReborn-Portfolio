#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTarget() = default;
public:
	ID3D11RenderTargetView*		Get_RTV() { return m_pRTV; }
	ID3D11ShaderResourceView*	Get_SRV() { return m_pSRV; }
public:
	HRESULT	NativeConstruct(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor);
	// 버퍼 색상을 클리어
	HRESULT Clear();
#ifdef _DEBUG
public:
	// 디버깅 용도로 화면에 출력할 버퍼의 정보를 셋팅하는 함수
	HRESULT Ready_DebugDesc(_float fX, _float fY, _float fSizeX, _float fSizeY);
	// 디버깅 용도로 화면에 버퍼를 출력
	HRESULT Render_DebugBuffer(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	ID3D11Texture2D*			m_pTexture2D = nullptr;
	ID3D11RenderTargetView*		m_pRTV = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;

private:
	_float4						m_vClearColor;
	_uint						m_iPass = 0;
#ifdef _DEBUG
private:
	_float4x4					m_WorldMatrix;
	_float4x4					m_ViewMatrix;
	_float4x4					m_ProjMatrix;
#endif
public:
	static CRenderTarget*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor);
	virtual void			Free() override;
};

END