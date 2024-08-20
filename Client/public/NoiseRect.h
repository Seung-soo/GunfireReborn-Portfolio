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

class CNoiseRect final : public CGameObject
{
private:
	CNoiseRect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNoiseRect(const CNoiseRect& rhs);
	virtual ~CNoiseRect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CShader*			m_pModelShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTexture*			m_pTextureCom[4] = { nullptr, };
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	_float				m_fAddNoiseUV = 0;
	_int				m_iTexIndex = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static	CNoiseRect*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END