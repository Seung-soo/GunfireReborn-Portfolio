#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CGhostBlinkEffect final : public CGameObject
{
private:
	CGhostBlinkEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGhostBlinkEffect(const CGhostBlinkEffect& rhs);
	virtual ~CGhostBlinkEffect() = default;
public:
	void			Set_EffectDown() { m_bEffectDown = true; }
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
	_float				m_fMoveUV_X = 0.f;
	_float				m_fMoveUV_Y = -1.f;
	_float4x4*			m_pGhostMatrix = nullptr;
	_bool				m_bEffectDown = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CGhostBlinkEffect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END