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

class CDamageNumber final : public CGameObject
{
public:
	typedef struct tagNumberDesc
	{
		_float3	vColor;
		_int	iTexIndex;
		_float	fX, fY;
	}NUMBERDESC;
private:
	CDamageNumber(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDamageNumber(const CDamageNumber& rhs);
	virtual ~CDamageNumber() = default;
public:
	void			Set_UIPosition(_float fX, _float fY, _float fZ) { m_fX = fX, m_fY = fY, m_fZ = fZ; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	_float				m_fAlpha = 255.f;
	NUMBERDESC			m_tNumberDesc = {};
private:
	_float4x4			m_ProjMatrix;
	_float				m_fX, m_fY, m_fZ, m_fSizeX, m_fSizeY;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CDamageNumber*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END