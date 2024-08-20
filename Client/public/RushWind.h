#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CRushWind final : public CGameObject
{
public:
	typedef struct tagRushWindDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
	}RUSHWINDDESC;
private:
	CRushWind(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CRushWind(const CRushWind& rhs);
	virtual ~CRushWind() = default;
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
	_float				m_fMoveUV = 0.f;
	_float				m_fSpeed = 15.f;
	_float				m_fScale = 0.1f;
	_float				m_fLiveTime = 0.f;
	_bool				m_bBye = false;
	_int				m_iPass = 14;

	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;

	RUSHWINDDESC		m_tRushWindDesc;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CRushWind*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END