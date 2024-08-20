#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CModel;
END

BEGIN(Client)

class CUFOBuff final : public CGameObject
{
private:
	CUFOBuff(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUFOBuff(const CUFOBuff& rhs);
	virtual ~CUFOBuff() = default;

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
	CTransform*			m_pBuffTransform = nullptr;
	_double				m_BuffTime = 10.0;
	class CMonster*		m_pMonster = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CUFOBuff*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END