#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CBearAttack final : public CGameObject
{
private:
	CBearAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBearAttack(const CBearAttack& rhs);
	virtual ~CBearAttack() = default;
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
	_float				m_fAlpha = 550.f;
	_float				m_fScale = 0.2f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	void	MakeSmoke();
public:
	static CBearAttack*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END