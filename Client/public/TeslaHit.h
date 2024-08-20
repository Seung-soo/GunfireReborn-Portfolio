#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CTeslaHit final : public CGameObject
{
private:
	CTeslaHit(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTeslaHit(const CTeslaHit& rhs);
	virtual ~CTeslaHit() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
private:
	_float				m_fMoveUV = 1.f;
	_float				m_fDelayTime = 0.5f;
	_int				m_iRender = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CTeslaHit*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END