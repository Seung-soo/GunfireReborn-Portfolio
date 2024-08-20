#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)

class CBossGun final : public CGameObject
{
private:
	CBossGun(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBossGun(const CBossGun& rhs);
	virtual ~CBossGun() = default;
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
	CTexture*			m_pMaskTextureCom = nullptr;
private:
	_uint				m_iAnimIndex = 0;
	CTransform*			m_pPlayerTransform = nullptr;
	_bool				m_bFire = false;

	_float				m_fTime = 0.f;
	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;
	_float				m_fDissolve = 1.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static	CBossGun*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END