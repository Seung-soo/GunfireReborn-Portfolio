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

class CElementIcon final : public CGameObject
{
public:
	enum ELEMENT_TYPE { TYPE_BURNING, TYPE_SHOCK, TYPE_DECAY, TYPE_COMBUSTION, TYPE_MANIPULATION, TYPE_MIASMA, TYPE_END };

	typedef struct tagElementIconDesc
	{
		CTransform*		pHpBackTransform = nullptr;
		ELEMENT_TYPE	eType = TYPE_END;
	}ELEMENTICONDESC;
private:
	CElementIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CElementIcon(const CElementIcon& rhs);
	virtual ~CElementIcon() = default;
public:
	void			Set_Render(_bool bUIRender) { m_bUIRender = bUIRender; }
	void			Set_Position(_float3 vPosition) { m_vPosition = vPosition; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	_bool				m_bUIRender = false;
	_float3				m_vPosition = _float3(0.f, 0.f, 0.f);
	ELEMENTICONDESC		m_tElementIconDesc;
	_float				m_fTime = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static	CElementIcon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END