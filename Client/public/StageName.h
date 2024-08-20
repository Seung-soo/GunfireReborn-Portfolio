#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CStageName final : public CGameObject
{
public:
	typedef struct tagStageNameDesc
	{
		_float4x4	WorldMatrix;
		_int		iTexIndex;
	}STAGENAMEDESC;
private:
	explicit CStageName(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStageName(const CStageName& rhs);
	virtual ~CStageName() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(_bool bStage);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
private:
	_float4x4			m_ProjMatrix;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
private:
	_uint				m_iTexIndex = 0;
	_bool				m_bStart = true;
	_bool				m_bStage = false;
	INFO_UI				m_tInfoUI = {};
	_float				m_fAlpha = 255.f;
private:
	HRESULT				SetUp_Components();
public:
	static CStageName*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _bool bStage);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END