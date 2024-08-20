#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CHitWave final : public CGameObject
{
private:
	CHitWave(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CHitWave(const CHitWave& rhs);
	virtual ~CHitWave() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom[2] = { nullptr, };
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	_float				m_fAddNoiseUV = 0;
	_int				m_iTexIndex = 0;
	_float4x4			m_ProjMatrix;
	_float				m_fDistortionPower = 0.05f;
	_float				m_fDistortionTime = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static	CHitWave*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END