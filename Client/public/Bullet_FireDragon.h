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

class CBullet_FireDragon final : public CGameObject
{
public:
	typedef struct tagBulletDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
		_float*			pDist = nullptr;
	}BULLETDESC;
private:
	CBullet_FireDragon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBullet_FireDragon(const CBullet_FireDragon& rhs);
	virtual ~CBullet_FireDragon() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom[2] = { nullptr, };
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_RectXZ*	m_pVIBufferCom = nullptr;
private:
	_int				m_iLiveTime = 180;
	_vector				m_vDir;
	BULLETDESC			m_tBulletDesc = {};

	_float				m_fSprite = 0.f;
	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;
private:
	_float				m_fAddNoiseUV = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CBullet_FireDragon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END