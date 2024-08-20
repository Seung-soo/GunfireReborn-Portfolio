#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	MAKE_SINGLETON(CLight_Manager)
private:
	CLight_Manager() = default;
	virtual ~CLight_Manager() = default;
public:
	const LIGHTDESC*	Get_LightDesc(_uint iIndex) const;
public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT	Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	void	Update();
	HRESULT Render();
	HRESULT	Release_Light();
private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
private: 
	list<class CLight*>			m_Lights;
	typedef list<class CLight*>	LIGHTS;
private:
	class CVIBuffer_Rect*		m_pVIBuffer = nullptr;
	class CShader*				m_pShader = nullptr;
private:
	_float4x4					m_WorldMatrix;
	_float4x4					m_ViewMatrix;
	_float4x4					m_ProjMatrix;
public:
	virtual void	Free() override;
};

END