#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CEnergyOrb final : public CGameObject
{
private:
	CEnergyOrb(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEnergyOrb(const CEnergyOrb& rhs);
	virtual ~CEnergyOrb() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
private:
	_int				m_iLiveTime = 180;
	_vector				m_vDir;
	_float				m_fLightningTime = 0.1f;
	CEffect::EFFECTDESC m_tEffectDesc = {};
	_float				m_fFreezeTime = 2.f;
	_bool				m_bExplotion = false;
	_float				m_fScale = 1.f;
	_bool				m_bCollision = false;
	_bool				m_bBigCollider = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
private:
	HRESULT Explotion();
public:
	static CEnergyOrb*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END