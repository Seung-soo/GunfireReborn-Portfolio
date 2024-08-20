#include "..\public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Texture.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

ID3D11ShaderResourceView * CRenderer::Get_SRV(_tchar * pRenderTargetTag)
{
	return m_pTarget_Manager->Get_SRV(pRenderTargetTag);
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (nullptr == m_pTarget_Manager)
	{
		MSGBOX("nullptr == m_pTarget_Manager in CRenderer::NativeConstruct_Prototype");
		return E_FAIL;
	}
	// 뷰포트를 얻어온다.
	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	// 디퓨즈 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Diffuse)");
		return E_FAIL;
	}
	
	// 노말 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Normal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Normal)");
		return E_FAIL;
	}

	// 깊이 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Depth)");
		return E_FAIL;
	}

	// 셰이드 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shade"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Normal)");
		return E_FAIL;
	}
	// 외곽선 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Outline"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Outline)");
		return E_FAIL;
	}

	// 계산 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Caculation"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Caculation)");
		return E_FAIL;
	}

	// 백버퍼 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_BackBuffer"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_BackBuffer)");
		return E_FAIL;
	}

	// 다운샘플링 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DownSampling"), _uint(Viewport.Width * 0.5f), _uint(Viewport.Height * 0.5f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_DownSampling)");
		return E_FAIL;
	}

	// 업샘플링 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_UpSampling"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_UpSampling)");
		return E_FAIL;
	}

	// 가로블러 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HorizenBlur"), _uint(Viewport.Width * 0.5f), _uint(Viewport.Height * 0.5f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_HorizenBlur)");
		return E_FAIL;
	}
	// 세로블러 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_VerticalBlur"), _uint(Viewport.Width * 0.5f), _uint(Viewport.Height * 0.5f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_VerticalBlur)");
		return E_FAIL;
	}
	// 블러마스크 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_BlurMask"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_BlurMask)");
		return E_FAIL;
	}
	// 논블랜드 연산 결과 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_NonBlendFinal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_NonBlendFinal)");
		return E_FAIL;
	}
	//// 알파 연산 결과 타겟 추가
	//if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_AlphaFinal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	//{
	//	MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_AlphaFinal)");
	//	return E_FAIL;
	//}
	//// 이펙트 타겟 추가
	//if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Effect"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	//{
	//	MSGBOX("m_pTarget_Manager->Add_RenderTarget returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Effect)");
	//	return E_FAIL;
	//}

	// 객체들을 그릴 때 바인드하는 멀티랜더타겟을 추가
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))	// 디퓨즈
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Diffuse)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))	// 노말
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Normal)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))	// 깊이
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Depth)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_BlurMask"))))	// 블러마스크
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Depth)");
		return E_FAIL;
	}

	// 빛을 그릴 때 바인드하는 멀티랜더타겟을 추가
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))	// 명암
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Shade)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Outline")))) // 외곽선
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Outline)");
		return E_FAIL;
	}

	// 계산할 때 바인드하는 멀티랜더타겟을 추가
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Caculation"), TEXT("Target_Caculation"))))	// 계산
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Caculation)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Caculation"), TEXT("Target_BackBuffer"))))	// 그리는 버퍼
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_BackBuffer)");
		return E_FAIL;
	}

	// 다운샘플링할 때 바인드하는 멀티랜더타겟을 추가
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DownSampling"), TEXT("Target_DownSampling"))))	// 다운샘플링
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_DownSampling)");
		return E_FAIL;
	}

	// 업샘플링할 때 바인드하는 멀티랜더타겟을 추가
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_UpSampling"), TEXT("Target_UpSampling"))))	// 업샘플링
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_UpSampling)");
		return E_FAIL;
	}

	// 가로블러할 때 바인드하는 멀티랜더타겟을 추가
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HorizenBlur"), TEXT("Target_HorizenBlur"))))	// 가로블러
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_HorizenBlur)");
		return E_FAIL;
	}

	// 세로블러할 때 바인드하는 멀티랜더타겟을 추가
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_VerticalBlur"), TEXT("Target_VerticalBlur"))))	// 세로블러
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_VerticalBlur)");
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_NonBlendFinal"), TEXT("Target_NonBlendFinal"))))	// 논블랜드 연산 결과
	{
		MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_NonBlendFinal)");
		return E_FAIL;
	}

	//if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_AlphaFinal"), TEXT("Target_AlphaFinal"))))	// 논블랜드 연산 결과
	//{
	//	MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_NonBlendFinal)");
	//	return E_FAIL;
	//}

	//// 이펙트까지 저장할 때 바인드하는 멀티랜더타겟을 추가
	//if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Caculation"))))	// 알파들
	//{
	//	MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Caculation)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_BackBuffer"))))	// 백버퍼
	//{
	//	MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_LastRender)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_BlurMask"))))	// 블러마스크
	//{
	//	MSGBOX("m_pTarget_Manager->Add_MRT returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_LastRender)");
	//	return E_FAIL;
	//}

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pVIBuffer)
	{
		MSGBOX("nullptr == m_pVIBuffer in CRenderer::NativeConstruct_Prototype");
		return E_FAIL;
	}

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pShader in CRenderer::NativeConstruct_Prototype");
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = Viewport.Width;
	m_WorldMatrix._22 = Viewport.Height;
	m_WorldMatrix._33 = 1.f;
	memcpy(&m_WorldMatrix.m[3][0], &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));

	//	랜더할 버퍼의 상태를 세팅해줌
#ifdef _DEBUG
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Diffuse"), 50.f, 50.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Diffuse)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Normal"), 50.f, 150.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Normal)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Depth"), 50.f, 250.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Depth)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Shade"), 150.f, 50.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Shade)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Outline"), 150.f, 150.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Outline)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Caculation"), 150.f, 250.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_BackBuffer)");
		return E_FAIL;
	}
	//if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Effect"), 50.f, 350.f, 100.f, 100.f)))
	//{
	//	MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Effect)");
	//	return E_FAIL;
	//}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_BackBuffer"), 50.f, 450.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Effect)");
		return E_FAIL;
	}
	//if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Effect"), 50.f, 350.f, 100.f, 100.f)))
	//{
	//	MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Effect)");
	//	return E_FAIL;
	//}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_DownSampling"), 150.f, 350.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Effect)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_UpSampling"), 150.f, 450.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Effect)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_HorizenBlur"), 250.f, 350.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Effect)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_VerticalBlur"), 250.f, 450.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Effect)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_BlurMask"), 350.f, 50.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Effect)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_NonBlendFinal"), 350.f, 150.f, 100.f, 100.f)))
	{
		MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_Effect)");
		return E_FAIL;
	}
	//if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_AlphaFinal"), 350.f, 250.f, 100.f, 100.f)))
	//{
	//	MSGBOX("m_pTarget_Manager->Ready_DebugDesc returned E_FAIL in CRenderer::NativeConstruct_Prototype(Target_AlphaFinal)");
	//	return E_FAIL;
	//}
#endif // _DEBUG

	m_pNoiseTexture = CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Texture/Effect/NoiseRect/noise (%d).png"), 99);


	m_fNormalization = (m_fBlurWeight0 + 2.f * (m_fBlurWeight1 + m_fBlurWeight2 + m_fBlurWeight3 + m_fBlurWeight4));

	m_fBlurWeight0 /= m_fNormalization;
	m_fBlurWeight1 /= m_fNormalization;
	m_fBlurWeight2 /= m_fNormalization;
	m_fBlurWeight3 /= m_fNormalization;
	m_fBlurWeight4 /= m_fNormalization;

	return S_OK;
}

HRESULT CRenderer::NativeConstruct(void * pArg)
{


	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
	{
		MSGBOX("eRenderGroup >= RENDER_END in CRenderer::Add_RenderGroup");
		return E_FAIL;
	}

	if (nullptr == pRenderObject)
	{
		MSGBOX("pRenderObject is nullptr in CRenderer::Add_RenderGroup");
		return E_FAIL;
	}

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::Add_Debug(CComponent * pComponent)
{
	m_DebugObject.push_back(pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}
#endif
HRESULT CRenderer::Render()
{
	if (FAILED(Render_Priority()))
	{
		MSGBOX("Render_Priority returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}

	if (FAILED(Render_NonAlpha()))
	{
		MSGBOX("Render_NonAlpha returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}

	if (FAILED(Render_Lights()))
	{
		MSGBOX("Render_Lights returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}

	if (FAILED(Render_Calculation()))
	{
		MSGBOX("Render_Calculation returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}
	if (FAILED(Render_DownSampling()))
	{
		MSGBOX("Render_DownSampling returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}
	if (FAILED(Render_HorizenBlur()))
	{
		MSGBOX("Render_HorizenBlur returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}
	if (FAILED(Render_VerticalBlur()))
	{
		MSGBOX("Render_VerticalBlur returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}
	if (FAILED(Render_UpSampling()))
	{
		MSGBOX("Render_UpSampling returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}
	if (FAILED(Render_NonBlendTexture()))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(Render_Blend()))
	{
		MSGBOX("Render_Blend returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}

	//if (FAILED(Render_Detail()))
	//{
	//	MSGBOX("Render_Detail returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}

	if (FAILED(Render_NonLight()))
	{
		MSGBOX("Render_NonLight returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}

	if (FAILED(Render_Alpha()))
	{
		MSGBOX("Render_Alpha returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}
	//if (FAILED(Render_DownSampling()))
	//{
	//	MSGBOX("Render_DownSampling returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}
	//if (FAILED(Render_HorizenBlur()))
	//{
	//	MSGBOX("Render_HorizenBlur returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}
	//if (FAILED(Render_VerticalBlur()))
	//{
	//	MSGBOX("Render_VerticalBlur returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}
	//if (FAILED(Render_UpSampling()))
	//{
	//	MSGBOX("Render_UpSampling returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}
	//if (FAILED(Render_AlphaTexture()))
	//{
	//	__debugbreak();
	//	return E_FAIL;
	//}
	//if (FAILED(Render_Blend()))
	//{
	//	MSGBOX("Render_Blend returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}
	//if (FAILED(Render_EffectDownSampling()))
	//{
	//	MSGBOX("Render_EffectDownSampling returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}
	//if (FAILED(Render_EffectHorizenBlur()))
	//{
	//	MSGBOX("Render_HorizenBlur returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}
	//if (FAILED(Render_EffectVerticalBlur()))
	//{
	//	MSGBOX("Render_VerticalBlur returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}
	//if (FAILED(Render_UpSampling()))
	//{
	//	MSGBOX("Render_UpSampling returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}
	//if (FAILED(Render_Effect()))
	//{
	//	MSGBOX("Render_Effect returned E_FAIL in CRenderer::Render");
	//	return E_FAIL;
	//}
#ifdef _DEBUG
	if (FAILED(Render_Debug()))
	{
		MSGBOX("Render_Debug returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}
#endif
	if (FAILED(Render_UI()))
	{
		MSGBOX("Render_UI returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}

	if (FAILED(Render_StopUI()))
	{
		MSGBOX("Render_StopUI returned E_FAIL in CRenderer::Render");
		return E_FAIL;
	}

#ifdef _DEBUG


	if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Deferred"))))
	{
		MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_Deferred)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_LightAcc"))))
	{
		MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_LightAcc)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Caculation"))))
	{
		MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_LightAcc)");
		return E_FAIL;
	}
	//if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Effect"))))
	//{
	//	MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_LightAcc)");
	//	return E_FAIL;
	//}
	if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_DownSampling"))))
	{
		MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_LightAcc)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_UpSampling"))))
	{
		MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_LightAcc)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_HorizenBlur"))))
	{
		MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_LightAcc)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_VerticalBlur"))))
	{
		MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_VerticalBlur)");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_NonBlendFinal"))))
	{
		MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_NonBlendFinal)");
		return E_FAIL;
	}
	//if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Effect"))))
	//{
	//	__debugbreak();
	//	MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_Effect)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_AlphaFinal"))))
	//{
	//	__debugbreak();
	//	MSGBOX("m_pTarget_Manager->Render_DebugBuffer returned E_FAIL in CRenderer::Render(MRT_Effect)");
	//	return E_FAIL;
	//}
#endif


	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
		{
			if (FAILED(pRenderObject->Render()))
			{
				MSGBOX("pRenderObject->Render returned E_FAIL in CRenderer::Render_Priority");
				return E_FAIL;
			}
		}
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
	// 디퍼드 멀티랜더타겟을 장치에 바인딩
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_Deferred"))))
	{
		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_NonAlpha");
		return E_FAIL;
	}
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
		{

			if (FAILED(pRenderObject->Render()))
			{
				MSGBOX("pRenderObject->Render returned E_FAIL in CRenderer::Render_NonAlpha");
				return E_FAIL;
			}

		}
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONBLEND].clear();

	// 다시 백버퍼를 바인딩
	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_Deferred"))))
	{
		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_NonAlpha");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (nullptr == m_pTarget_Manager)
	{
		MSGBOX("nullptr == m_pTarget_Manager in CRenderer::Render_Lights");
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_LightAcc"))))
	{
		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_Lights");
		return E_FAIL;
	}

	m_pLight_Manager->Render();

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_LightAcc"))))
	{
		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_Lights");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CRenderer::Render_Calculation()
{
	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_Calculation");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_Caculation"))))
	{
		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_Calculation");
		return E_FAIL;
	}

	// 디퓨즈 타겟을 얻어와서 텍스쳐로 던진다.
	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_Calculation(g_DiffuseTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_Calculation(g_ShadeTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_OutlineTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Outline")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_Calculation(g_OutlineTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Calculation(g_WorldMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Calculation(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Calculation(m_ProjMatrix)");
		return E_FAIL;
	}

	m_pVIBuffer->Render(m_pShader, 4);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_Caculation"))))
	{
		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_Calculation");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_DownSampling()
{
	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_DownSampling");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_DownSampling"))))
	{
		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_DownSampling");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Caculation")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_DownSampling(Target_Caculation)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BlurMask")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_DownSampling(Target_BlurMask)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_DownSampling(g_WorldMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_DownSampling(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_DownSampling(m_ProjMatrix)");
		return E_FAIL;
	}

	m_pVIBuffer->Render(m_pShader, 5);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_DownSampling"))))
	{
		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_DownSampling");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CRenderer::Render_UpSampling()
{
	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_UpSampling");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_UpSampling"))))
	{
		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_UpSampling");
		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_VerticalBlur")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_UpSampling(Target_VerticalBlur)");
		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_UpSampling(g_WorldMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_UpSampling(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_UpSampling(m_ProjMatrix)");
		return E_FAIL;
	}

	m_pVIBuffer->Render(m_pShader, 6);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_UpSampling"))))
	{
		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_DownSampling");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CRenderer::Render_HorizenBlur()
{
	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_HorizenBlur");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HorizenBlur"))))
	{
		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_HorizenBlur");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DownSampling")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_HorizenBlur(Target_UpSampling)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BlurMask")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_HorizenBlur(Target_BlurMask)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_WorldMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(m_ProjMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_fWeight0", &m_fBlurWeight0, sizeof(_float))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_fWeight0)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_fWeight1", &m_fBlurWeight1, sizeof(_float))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_fWeight1)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_fWeight2", &m_fBlurWeight2, sizeof(_float))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_fWeight2)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_fWeight3", &m_fBlurWeight3, sizeof(_float))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_fWeight3)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_fWeight4", &m_fBlurWeight4, sizeof(_float))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_fWeight4)");
		return E_FAIL;
	}


	m_pVIBuffer->Render(m_pShader, 7);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HorizenBlur"))))
	{
		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_HorizenBlur");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_VerticalBlur()
{
	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_VerticalBlur");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_VerticalBlur"))))
	{
		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_VerticalBlur");
		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HorizenBlur")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_VerticalBlur(Target_HorizenBlur)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BlurMask")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_VerticalBlur(Target_BlurMask)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(g_WorldMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(m_ProjMatrix)");
		return E_FAIL;
	}

	m_pVIBuffer->Render(m_pShader, 8);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_VerticalBlur"))))
	{
		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_VerticalBlur");
		return E_FAIL;
	}

	return S_OK;
}

//HRESULT CRenderer::Render_EffectHorizenBlur()
//{
//	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
//	{
//		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_HorizenBlur");
//		return E_FAIL;
//	}
//	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HorizenBlur"))))
//	{
//		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_HorizenBlur");
//		return E_FAIL;
//	}
//
//	Render_Detail();
//
//	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DownSampling")))))
//	{
//		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_HorizenBlur(Target_UpSampling)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BlurMask")))))
//	{
//		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_HorizenBlur(Target_BlurMask)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_WorldMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_ViewMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(m_ProjMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_fWeight0", &m_fBlurWeight0, sizeof(_float))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_fWeight0)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_fWeight1", &m_fBlurWeight1, sizeof(_float))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_fWeight1)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_fWeight2", &m_fBlurWeight2, sizeof(_float))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_fWeight2)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_fWeight3", &m_fBlurWeight3, sizeof(_float))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_fWeight3)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_fWeight4", &m_fBlurWeight4, sizeof(_float))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_HorizenBlur(g_fWeight4)");
//		return E_FAIL;
//	}
//
//
//	m_pVIBuffer->Render(m_pShader, 7);
//
//	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HorizenBlur"))))
//	{
//		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_HorizenBlur");
//		return E_FAIL;
//	}
//
//	return S_OK;
//}
//
//HRESULT CRenderer::Render_EffectVerticalBlur()
//{
//	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
//	{
//		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_VerticalBlur");
//		return E_FAIL;
//	}
//	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_VerticalBlur"))))
//	{
//		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_VerticalBlur");
//		return E_FAIL;
//	}
//
//	Render_Detail();
//
//	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HorizenBlur")))))
//	{
//		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_VerticalBlur(Target_HorizenBlur)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BlurMask")))))
//	{
//		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_VerticalBlur(Target_BlurMask)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(g_WorldMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(g_ViewMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(m_ProjMatrix)");
//		return E_FAIL;
//	}
//
//	m_pVIBuffer->Render(m_pShader, 8);
//
//	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_VerticalBlur"))))
//	{
//		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_VerticalBlur");
//		return E_FAIL;
//	}
//
//	return S_OK;
//}
//
//HRESULT CRenderer::Render_Detail()
//{
//	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Caculation")))))
//	{
//		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_VerticalBlur(Target_Caculation)");
//		return E_FAIL;
//	}
//	// 아웃라인 타겟을 얻어와서 텍스쳐로 던진다.
//	if (FAILED(m_pShader->Set_Texture("g_OutlineTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Outline")))))
//	{
//		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_Blend(Target_Outline)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(g_WorldMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(g_ViewMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(m_ProjMatrix)");
//		return E_FAIL;
//	}
//
//	m_pVIBuffer->Render(m_pShader, 9);
//
//	return S_OK;
//}

HRESULT CRenderer::Render_NonBlendTexture()
{
	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_NonBlendTexture");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_NonBlendFinal"))))
	{
		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_NonBlendTexture");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BackBuffer")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_NonBlendTexture(Target_BackBuffer)");
		return E_FAIL;
	}
	// 업샘플링 타겟을 얻어와서 텍스쳐로 던진다.
	if (FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_UpSampling")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_NonBlendTexture(Target_UpSampling)");
		return E_FAIL;
	}
	// 블러마스크 타겟을 얻어와서 텍스쳐로 던진다.
	if (FAILED(m_pShader->Set_Texture("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BlurMask")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_NonBlendTexture(Target_BlurMask)");
		return E_FAIL;
	}
	// 외곽선 타겟 던짐
	if (FAILED(m_pShader->Set_Texture("g_OutlineTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Outline")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_NonBlendTexture(Target_Outline)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_NonBlendTexture(g_WorldMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_NonBlendTexture(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_NonBlendTexture(m_ProjMatrix)");
		return E_FAIL;
	}

	m_pVIBuffer->Render(m_pShader, 3);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_NonBlendFinal"))))
	{
		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_NonBlendTexture");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CRenderer::Render_AlphaTexture()
{
	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_NonBlendTexture");
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_AlphaFinal"))))
	{
		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_NonBlendTexture");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Caculation")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_NonBlendTexture(Target_BackBuffer)");
		return E_FAIL;
	}
	// 업샘플링 타겟을 얻어와서 텍스쳐로 던진다.
	if (FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_UpSampling")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_NonBlendTexture(Target_UpSampling)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_NonBlendTexture(g_WorldMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_NonBlendTexture(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_NonBlendTexture(m_ProjMatrix)");
		return E_FAIL;
	}

	m_pVIBuffer->Render(m_pShader, 9);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_AlphaFinal"))))
	{
		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_NonBlendTexture");
		return E_FAIL;
	}


	if (FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_AlphaFinal")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_VerticalBlur(Target_NonBlendFinal)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Blend(g_WorldMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Blend(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Blend(m_ProjMatrix)");
		return E_FAIL;
	}

	m_pVIBuffer->Render(m_pShader, 0);

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_Blend");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonBlendFinal")))))
	{
		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_VerticalBlur(Target_NonBlendFinal)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Blend(g_WorldMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Blend(g_ViewMatrix)");
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Blend(m_ProjMatrix)");
		return E_FAIL;
	}

	m_pVIBuffer->Render(m_pShader, 0);

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	//if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_Effect"))))
	//{
	//	MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_NonLight");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonBlendFinal")))))
	//{
	//	MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_VerticalBlur(Target_NonBlendFinal)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	//{
	//	MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Blend(g_WorldMatrix)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	//{
	//	MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Blend(g_ViewMatrix)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	//{
	//	MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Blend(m_ProjMatrix)");
	//	return E_FAIL;
	//}

	//m_pVIBuffer->Render(m_pShader, 10);

	//Render_Detail();

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
		{
			if (FAILED(pRenderObject->Render()))
			{
				MSGBOX("pRenderObject->Render returned E_FAIL in CRenderer::Render_NonLight");
				return E_FAIL;
			}
		}
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Alpha()
{


	m_RenderObjects[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	{
		
		return pSour->Get_AlphaDepth() > pDest->Get_AlphaDepth();
	});

	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pRenderObject)
		{
			if (FAILED(pRenderObject->Render()))
			{
				MSGBOX("pRenderObject->Render returned E_FAIL in CRenderer::Render_Alpha");
				return E_FAIL;
			}
		}
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_BLEND].clear();

	//if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_Effect"))))
	//{
	//	MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_Alpha");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect")))))
	//{
	//	MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_VerticalBlur(Target_Caculation)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	//{
	//	MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(g_WorldMatrix)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	//{
	//	MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(g_ViewMatrix)");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	//{
	//	MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_VerticalBlur(m_ProjMatrix)");
	//	return E_FAIL;
	//}

	//m_pVIBuffer->Render(m_pShader, 6);

	return S_OK;
}
//
//HRESULT CRenderer::Render_EffectDownSampling()
//{
//	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
//	{
//		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_DownSampling");
//		return E_FAIL;
//	}
//	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_DownSampling"))))
//	{
//		MSGBOX("m_pTarget_Manager->Begin returned E_FAIL in CRenderer::Render_DownSampling");
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect")))))
//	{
//		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_DownSampling(Target_Caculation)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_DownSampling(g_WorldMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_DownSampling(g_ViewMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_DownSampling(m_ProjMatrix)");
//		return E_FAIL;
//	}
//
//	m_pVIBuffer->Render(m_pShader, 5);
//
//	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_DownSampling"))))
//	{
//		MSGBOX("m_pTarget_Manager->End returned E_FAIL in CRenderer::Render_DownSampling");
//		return E_FAIL;
//	}
//
//	return S_OK;
//}
//
//HRESULT CRenderer::Render_Effect()
//{
//	if (nullptr == m_pVIBuffer || nullptr == m_pShader)
//	{
//		MSGBOX("nullptr == m_pVIBuffer || nullptr == m_pShader in CRenderer::Render_Effect");
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Effect(g_WorldMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Effect(g_ViewMatrix)");
//		return E_FAIL;
//	}
//	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
//	{
//		MSGBOX("m_pShader->Set_RawValue returned E_FAIL in CRenderer::Render_Effect(m_ProjMatrix)");
//		return E_FAIL;
//	}
//
//	// 디퓨즈 타겟을 얻어와서 텍스쳐로 던진다.
//	if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BackBuffer")))))
//	{
//		MSGBOX("m_pShader->Set_Texture returned E_FAIL in CRenderer::Render_Effect(Target_BackBuffer)");
//		return E_FAIL;
//	}
//
//	m_pVIBuffer->Render(m_pShader, 6);
//
//	return S_OK;
//}

HRESULT CRenderer::Render_UI()
{
	m_RenderObjects[RENDER_UI].sort(DepthSort<CGameObject*>);
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
		{
			if (false == m_bDontRenderUI)
			{
				if (FAILED(pRenderObject->Render()))
				{
					MSGBOX("pRenderObject->Render returned E_FAIL in CRenderer::Render_UI");
					return E_FAIL;
				}
			}
		}
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_StopUI()
{
	m_RenderObjects[RENDER_STOPUI].sort(DepthSort<CGameObject*>);
	for (auto& pRenderObject : m_RenderObjects[RENDER_STOPUI])
	{
		if (nullptr != pRenderObject)
		{
			if (FAILED(pRenderObject->Render()))
			{
				MSGBOX("pRenderObject->Render returned E_FAIL in CRenderer::Render_StopUI");
				return E_FAIL;
			}
		}
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_STOPUI].clear();

	return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& pComponent : m_DebugObject)
	{
		if (nullptr != pComponent)
			pComponent->Render();

		Safe_Release(pComponent);
	}
	m_DebugObject.clear();

	return S_OK;
}
#endif
CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("NativeConstruct_Prototype returned E_FAIL in CRenderer::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pGameObject : RenderObjects)
			Safe_Release(pGameObject);
		RenderObjects.clear();
	}
}
