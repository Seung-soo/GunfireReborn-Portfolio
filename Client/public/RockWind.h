#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CModel;
class CCollider;
END

BEGIN(Client)

class CRockWind final : public CGameObject
{
public:
	typedef struct tagRockWindDesc
	{
		CTransform*	pRockWindTransform = nullptr;
		_vector		vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		_bool*		pDead = nullptr;
	}ROCKWINDDESC;
private:
	CRockWind(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CRockWind(const CRockWind& rhs);
	virtual ~CRockWind() = default;
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
	ROCKWINDDESC		m_tRockWindDesc;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static	CRockWind*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END