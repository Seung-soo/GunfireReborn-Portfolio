#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_RectXZ;
class CCollider;
END

BEGIN(Client)

class CBullet_Tesla final : public CGameObject
{
public:
	typedef struct tagBulletDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
		_float			fDist = 30.f;
	}BULLETDESC;
private:
	CBullet_Tesla(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBullet_Tesla(const CBullet_Tesla& rhs);
	virtual ~CBullet_Tesla() = default;

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
	CVIBuffer_RectXZ*	m_pVIBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
private:
	_int				m_iLiveTime = 180;
	_vector				m_vDir;
	BULLETDESC			m_tBulletDesc = {};
	_float				m_fZscale = 1.f;

	_float				m_fSprite = 0.f;
	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;
	_float4				m_vMyPosition = _float4(0.f, 0.f, 0.f, 1.f);
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CBullet_Tesla*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END