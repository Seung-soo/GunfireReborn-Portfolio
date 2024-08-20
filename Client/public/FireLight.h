#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CFireLight final : public CGameObject
{
private:
	CFireLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CFireLight(const CFireLight& rhs);
	virtual ~CFireLight() = default;
public:
	void			Set_RealTexIndex(_int iTexIndex) { m_iRealTexIndex = iTexIndex; }
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
	_int				m_iRealTexIndex = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static	CFireLight*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END