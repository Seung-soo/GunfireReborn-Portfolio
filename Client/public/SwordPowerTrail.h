#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CModel;
class CCollider;
END

BEGIN(Client)

class CSwordPowerTrail final : public CGameObject
{
private:
	CSwordPowerTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSwordPowerTrail(const CSwordPowerTrail& rhs);
	virtual ~CSwordPowerTrail() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
private:
	_float				m_fFrustumRadius = 5.f;
	_float				m_fAddNoiseUV = 0.f;
	CGameObject*		m_pSwordPower = nullptr;
	CTransform*			m_pSwordPowerTransform = nullptr;
	_float				m_fScale = 0.1f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static	CSwordPowerTrail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END