#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_RectXZ;
END

BEGIN(Client)

class CRushWarning final : public CGameObject
{
private:
	CRushWarning(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CRushWarning(const CRushWarning& rhs);
	virtual ~CRushWarning() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pMaskTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_RectXZ*		m_pVIBufferCom = nullptr;
private:
	_float				m_fMoveUV = 0.f;
	_float				m_fScaleX = 1.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CRushWarning*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END