#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CGhostBulletEffect final : public CGameObject
{
private:
	CGhostBulletEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGhostBulletEffect(const CGhostBulletEffect& rhs);
	virtual ~CGhostBulletEffect() = default;
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
	CModel*				m_pModelCom = nullptr;
private:
	_int				m_iTexIndex = 0;
	_float				m_fAddNoiseUV = 0.f;
	CGameObject*		m_pBullet = nullptr;
	CTransform*			m_pBulletTransform = nullptr;
	_float3				m_vLocalPos = {};
	_int				m_iSpeed = 10;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CGhostBulletEffect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END