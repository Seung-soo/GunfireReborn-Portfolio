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

class CVictoryShine final : public CGameObject
{
private:
	explicit CVictoryShine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVictoryShine(const CVictoryShine& rhs);
	virtual ~CVictoryShine() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
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
	_double				m_dTime = 0.0;
	_float				m_fSpriteIndex = 0.f;
	_int				m_iSpriteYIndex = 0;
private:
	HRESULT				SetUp_Components();
public:
	static CVictoryShine*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END