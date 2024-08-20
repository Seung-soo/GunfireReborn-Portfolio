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

class CLobstarFireBoom final : public CGameObject
{
private:
	CLobstarFireBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLobstarFireBoom(const CLobstarFireBoom& rhs);
	virtual ~CLobstarFireBoom() = default;

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
	_int				m_iTexIndex = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static	CLobstarFireBoom*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END