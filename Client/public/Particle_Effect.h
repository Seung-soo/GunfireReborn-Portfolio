#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_Effect final : public CGameObject
{
public:
	enum DIR { DIR_ONE, DIR_RANDOM, DIR_END };
	enum PATICLENUM { NUM_10 = 10, NUM_30 = 30, NUM_50 = 50, NUM_100 = 100, NUM_END };
	enum EASINGFUNCTION {
		INSINE, OUTSINE, INOUTSINE, INQUAD, OUTQUAD, INOUTQUAD,
		INCUBIC, OUTCUBIC, INOUTCUBIC, INQUART, OUTQUART, INOUTQUART,
		INQUINT, OUTQUINT, INOUTQUINT, INEXPO, OUTEXPO, INOUTEXPO,
		INCIRC, OUTCIRC, INOUTCIRC, INBACK, OUTBACK, INOUTBACK,
		INELASTIC, OUTELASTIC, INOUTELASTIC, INBOUNCE, OUTBOUNCE, INOUTBOUNCE,
		EASING_END
	};
public:
	typedef struct tagParticleDesc
	{
		_int			iTexIndex = 0;
		_float			fMinSpeed = 0.f;
		_float			fMaxSpeed = 0.f;
		_float			fOriginScale = 1.f;
		_float			fScaleSpeed = 0.f;
		_float			fGravityPower = 0.f;
		_float			fLiveTime = 0.f;
		_float			fMakeRange = 0.f;
		_float			fMaxDistance = 0.f;
		DIR				eDir = DIR_ONE;
		_vector			vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		PATICLENUM		eParticleNumber = NUM_10;
		EASINGFUNCTION	eEasing = INSINE;
		_bool			bRoof = false;
		_bool			bUseEasing = false;
		_bool			bChase = false;
		_bool			bTrail = false;
		_float			fTrailInterval = 0.f;
		_float4*		pParentPosition = nullptr;
		_bool*			pDead = nullptr;
	}PARTICLEDESC;
private:
	CParticle_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CParticle_Effect(const CParticle_Effect& rhs);
	virtual ~CParticle_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Point_Instance*	m_pVIBufferCom = nullptr;
private:
	vector<_float4x4>			m_TransformMatrix;
	_vector						m_vStartPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_float						m_fCurrentTime = 0.f;
	PARTICLEDESC				m_tParticleDesc;
	vector<_float>				m_Speeds;
	vector<_float>				m_CurrentSpeeds;
	vector<_vector>				m_Directions;
	vector<_float3>				m_FirstPositions;
	vector<_float>				m_CurrentScale;
	_int						m_iParticleNum = 0;
	_int						m_iTrailNum = 0;
	_float						m_fTrailTime = 0.f;
	_bool						m_bParticleEnd = false;
	_int						m_iEndCycle = -1;
	_bool						m_bEndStart = false;
private:
	_float(*m_pEasingFunction)(_float, _float, _float);
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
private:
	void	UseEasingFuntion();
public:
	static CParticle_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END