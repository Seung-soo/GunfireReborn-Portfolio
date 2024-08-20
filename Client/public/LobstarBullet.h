#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CLobstarBullet final : public CGameObject
{
public:
	typedef struct tagLobstarBulletDesc
	{
		_vector vStartPoint;
		_vector vTargetPoint;
	}LOBSTARBULLETDESC;
private:
	CLobstarBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLobstarBullet(const CLobstarBullet& rhs);
	virtual ~CLobstarBullet() = default;
public:
	void			Set_Manipulation(CGameObject* pTarget) { m_bManipulation = true; m_pTarget = pTarget; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CShader*			m_pModelShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
private:
	_int				m_iLiveTime = 300;
	_vector				m_vDir;
	LOBSTARBULLETDESC	m_tLobstarBulletDesc = {};
	_vector				m_vCenterPoint = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_double				m_dLerpTime = 0.0;
	_bool				m_bManipulation = false;
	CGameObject*		m_pTarget = nullptr;
	_int				m_iTexIndex = 0;
	_float				m_fRotNoise = 0.f;
	_float				m_fMoveUV = 0.f;
	_float				m_fFireEffectTime = 0.1f;
	_float4				m_vPosition;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT	Collision_Terrain();
public:
	static CLobstarBullet*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END