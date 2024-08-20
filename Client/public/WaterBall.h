#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
class CModel;
END

BEGIN(Client)

class CWaterBall final : public CGameObject
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
	CWaterBall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWaterBall(const CWaterBall& rhs);
	virtual ~CWaterBall() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
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
	_int				m_iLiveTime = 180;
	_vector				m_vDir;
	BULLETDESC			m_tBulletDesc = {};

	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;

	_double				m_dSpeed = 0.0;
	class CCamera_Dynamic* m_pCamera = nullptr;
private:
	_float				m_fAddNoiseUV = 0.f;
	_float				m_fMoveUV = 0.f;
	_int				m_iTexIndex = 0;
	_float				m_fDistortionPower = 0.01f;
	_float				m_fWaterEffectTime = 0.02f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CWaterBall*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END