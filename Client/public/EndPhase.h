#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CEndPhase final : public CGameObject
{
public:
	typedef struct tagEndPhaseDesc
	{
		_vector	vPosition;
		_float4	vColor;
	}ENDPHASEDESC;
private:
	CEndPhase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEndPhase(const CEndPhase& rhs);
	virtual ~CEndPhase() = default;
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
	_float				m_fMoveUV = 1.f;
	ENDPHASEDESC		m_tEndPhaseDesc;
	_float				m_fScale = 1.f;
	_float				m_fTime = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static	CEndPhase*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END