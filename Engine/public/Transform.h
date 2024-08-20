#pragma once

#include "Component.h"

BEGIN(Engine)
class CNavigation;
class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = 0.f;
		_float		fRotationPerSec = 0.f;
		_float		fScalingPerSec = 0.f;
	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_Scale(STATE eState) const;
	_vector Get_AllScale() const;
	_vector Get_State(STATE eState) const;

	void	Set_WorldMatrix(_float4x4 WorldMatrix) { m_WorldMatrix = WorldMatrix; }
	_matrix Get_WorldMatrix() const;
	_matrix Get_WorldMatrixInverse() const;
	_float4x4 Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}
	_float4x4* Get_WorldFloat4x4_Pointer() {
		return &m_WorldMatrix;
	}

	_matrix Get_OldWorldMatrix() const;
	_matrix Get_OldWorldMatrixInverse() const;
	_float4x4 Get_OldWorldFloat4x4() const {
		return m_OldWorldMatrix;
	}

	void	Set_TransformDesc(TRANSFORMDESC tTransformDesc) { m_TransformDesc = tTransformDesc; }
	void	Set_Speed(_float fSpeed) { m_TransformDesc.fSpeedPerSec = fSpeed; }
	_float	Get_Speed() { return m_TransformDesc.fSpeedPerSec; }
public:
	void Set_State(STATE eState, _fvector vState);

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	HRESULT Bind_OnShader(class CShader* pShader, const char* pValueName);
	HRESULT Bind_OnOldShader(class CShader* pShader, const char* pValueName);

public:
	HRESULT Go_Straight(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Right(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Left(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Backward(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_FrontLeft(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_FrontRight(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_BackLeft(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_BackRight(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Up(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Down(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Vector(_fvector vDirection, _double dTimeDelta, CNavigation* pNavigation = nullptr);

	HRESULT Turn(_fvector vAxis, _double TimeDelta);
	HRESULT CameraTurn(_fvector vAxis, _double TimeDelta);
	HRESULT Rotation(_fvector vAxis, _float fRadian);
	HRESULT NowRotation(_fvector vAxis, _float fRadian);
	HRESULT AllRotation(_fvector vRotationNum);
	HRESULT Revolution(_fvector vAxis, _fvector vTarget, _double TimeDelta);

	HRESULT Scaled(_fvector vScale);
	HRESULT OneScaled(STATE eState, _float fScale);
	HRESULT Scaling(_double TimeDelta);
	HRESULT OneScaling(STATE eState, _double TimeDelta);

	HRESULT Chase(_fvector vTargetPos, _double TimeDelta);
	HRESULT LookAt(_fvector vTargetPos);
	HRESULT LookAtXZ(_fvector vTargetPos);
	HRESULT MatchIt_XZ(_fvector vTargetPos);
	HRESULT LookTurn(_fvector vAxis, _fvector vTarget, _double TimeDelta);


	void	ArrangeMatrix();
private:
	TRANSFORMDESC			m_TransformDesc;
	_float4x4				m_WorldMatrix;
	_float4x4				m_OldWorldMatrix;
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END