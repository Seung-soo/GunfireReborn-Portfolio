#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CMonkSmallEffect final : public CGameObject
{
private:
	CMonkSmallEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonkSmallEffect(const CMonkSmallEffect& rhs);
	virtual ~CMonkSmallEffect() = default;
public:
	void			Set_Disappear();
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
private:
	_int				m_iTexIndex = 0;
	_float				m_fMoveNoise = 0.f;
	_float				m_fMoveUV = 0.f;
	_int				m_iPass = 15;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CMonkSmallEffect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END