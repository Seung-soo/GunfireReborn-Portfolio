#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CReadyWaterBall final : public CGameObject
{
public:
	typedef struct tagAimDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
		CGameObject*	pParent = nullptr;
	}AIMDESC;
private:
	CReadyWaterBall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CReadyWaterBall(const CReadyWaterBall& rhs);
	virtual ~CReadyWaterBall() = default;
public:
	void			Set_MoveUV(_float fMoveUV) { m_fMoveUV = fMoveUV; }
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
	CTexture*			m_pNoiseTextureCom = nullptr;
private:
	AIMDESC				m_tAimDesc;
	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;

private:
	_float				m_fAddNoiseUV = 0.f;
	_float				m_fMoveUV = 1.f;
	_int				m_iTexIndex = 0;
	_float				m_fDistortionPower = 0.01f;
	_int				m_iCount = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CReadyWaterBall*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END