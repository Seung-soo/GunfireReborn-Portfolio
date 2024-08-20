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

class CBossCannonBullet final : public CGameObject
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
	CBossCannonBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBossCannonBullet(const CBossCannonBullet& rhs);
	virtual ~CBossCannonBullet() = default;

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
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
private:
	_int				m_iLiveTime = 200;
	_vector				m_vDir;
	BULLETDESC			m_tBulletDesc = {};

	_float				m_fSprite = 0.f;
	_int				m_iSpriteY = 0;
	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;

	CCollider*			m_pPlayerCollider = nullptr;
	CGameObject*		m_pPoleMornarch = nullptr;
	_float				m_fLightningTime = 0.5f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CBossCannonBullet*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END