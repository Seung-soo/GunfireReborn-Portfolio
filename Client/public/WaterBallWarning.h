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

class CWaterBallWarning final : public CGameObject
{
private:
	CWaterBallWarning(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWaterBallWarning(const CWaterBallWarning& rhs);
	virtual ~CWaterBallWarning() = default;

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
	CVIBuffer_RectXZ*		m_pVIBufferCom = nullptr;
private:
	_float				m_fScale = 1.f;
	_float				m_fTime = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CWaterBallWarning*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END