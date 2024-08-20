#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	MAKE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATETYPE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;
public:
	HRESULT			Set_Transform(TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix);

	_matrix			Get_TransformMatrix(TRANSFORMSTATETYPE eStateType);

	_float4x4		Get_TransformFloat4x4(TRANSFORMSTATETYPE eStateType);

	_float4x4		Get_TransformFloat4x4_TP(TRANSFORMSTATETYPE eStateType);

	_vector			Get_CamPosition() const;

	ContainmentType	Frustum(_vector vPosition, _float fRadius);

public:
	void			Update();
private:
	_float4x4			m_PipeLineMatrix[2];

	_float4				m_vCamPosition;
public:
	virtual void	Free() override;
};

END