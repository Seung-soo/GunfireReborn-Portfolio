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

class CSwordPower final : public CGameObject
{
public:
	typedef struct tagBulletDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
		_float			fDist = 0.f;
	}BULLETDESC;
private:
	CSwordPower(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSwordPower(const CSwordPower& rhs);
	virtual ~CSwordPower() = default;
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
	CCollider*			m_pColliderCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
private:
	_float				m_fFrustumRadius = 5.f;
	_double				m_dLiveTime = 0.0;
	_bool				m_bCollision = false;

	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;

	_vector				m_vDir;
	BULLETDESC			m_tBulletDesc = {};
	CGameObject*		m_pPoleMornarch = nullptr;

	_bool*				m_pPhase4 = nullptr;
	_float				m_fAddNoiseUV = 0.f;
	_float4				m_vPosition = _float4(0.f, 0.f, 0.f, 1.f);
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static	CSwordPower*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END