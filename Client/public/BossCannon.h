#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CBossCannon final : public CGameObject
{
private:
	CBossCannon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBossCannon(const CBossCannon& rhs);
	virtual ~CBossCannon() = default;
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
	_uint				m_iAnimIndex = 0;
	CTransform*			m_pBossTransform = nullptr;
	_bool				m_bFire = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static	CBossCannon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END