#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CRevolutionRock final : public CGameObject
{
private:
	CRevolutionRock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CRevolutionRock(const CRevolutionRock& rhs);
	virtual ~CRevolutionRock() = default;
public:
	void			Set_Throw(_bool bThrow) { m_bThrow = bThrow; }
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
private:
	_vector				m_vStartPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector				m_vAxis = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_bool				m_bThrow = false;
	_bool				m_bTargetSetting = false;
	_vector				m_vTargetDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	CTransform*			m_pPlayerTransform = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static	CRevolutionRock*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END