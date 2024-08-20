#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CBreakRock final : public CGameObject
{
public:
	typedef struct tagRockDesc
	{
		_vector		vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		_vector		vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	}ROCKDESC;
private:
	CBreakRock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBreakRock(const CBreakRock& rhs);
	virtual ~CBreakRock() = default;
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
private:
	tagRockDesc		m_tRockWindDesc;
	_float				m_fGravityPower = 4.f;
	_float				m_fDownPower = 0.f;
	_float				m_fTime = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static	CBreakRock*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END