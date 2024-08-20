#pragma once

#include "Component.h"


BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_STOPUI, RENDER_END };
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;
public:
	void						Set_DontRenderUI(_bool bDontRenderUI) { m_bDontRenderUI = bDontRenderUI; }
	ID3D11ShaderResourceView*	Get_SRV(_tchar* pRenderTargetTag);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
public:
	HRESULT			Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject);
#ifdef _DEBUG
	HRESULT			Add_Debug(class CComponent* pComponent);
#endif
	virtual HRESULT	Render();
private:
	list<CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list <CGameObject>	RENDEROBJECTS;
private:
	list<class CComponent*>			m_DebugObject;
	typedef list<class CComponent*>	DEBUGOBJECT;
private:
	class CTarget_Manager*		m_pTarget_Manager = nullptr;
	class CLight_Manager*		m_pLight_Manager = nullptr;
private:
	class CVIBuffer_Rect*		m_pVIBuffer = nullptr;
	class CShader*				m_pShader = nullptr;
	class CTexture*				m_pNoiseTexture = nullptr;
private:
	_float4x4					m_WorldMatrix;
	_float4x4					m_ViewMatrix;
	_float4x4					m_ProjMatrix;
private:
	_bool						m_bDontRenderUI = false;
	_int						m_iNoiseTexIndex = 0;
	_float						m_fBlurWeight0 = 1.f;
	_float						m_fBlurWeight1 = 0.9f;
	_float						m_fBlurWeight2 = 0.55f;
	_float						m_fBlurWeight3 = 0.18f;
	_float						m_fBlurWeight4 = 0.1f;
	_float						m_fNormalization = 1.f;

	_bool						m_bAlphaEffect = false;
private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlpha();
	// 명암 연산 랜더
	HRESULT Render_Lights();
	// 타겟들을 합친 결과를 랜더
	HRESULT Render_Calculation();
	HRESULT Render_DownSampling();
	HRESULT	Render_UpSampling();
	HRESULT	Render_HorizenBlur();
	HRESULT	Render_VerticalBlur();
	//HRESULT	Render_EffectHorizenBlur();
	//HRESULT	Render_EffectVerticalBlur();
	HRESULT Render_NonBlendTexture();
	HRESULT Render_AlphaTexture();	
	HRESULT Render_Blend();
	//HRESULT	Render_Detail();
	HRESULT Render_NonLight();
	HRESULT Render_Alpha();
	//HRESULT Render_EffectDownSampling();
	//HRESULT Render_Effect();
	HRESULT Render_UI();
	HRESULT Render_StopUI();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif
public:
	static CRenderer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*	Clone(void* pArg) override;
	virtual void		Free() override;
};

END