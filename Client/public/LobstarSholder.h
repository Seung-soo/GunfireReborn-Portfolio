#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CLobstarSholder final : public CGameObject
{
public:
	typedef struct tagShoulderDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
		CGameObject*	pParent = nullptr;
	}SHOULDERDESC;
private:
	CLobstarSholder(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLobstarSholder(const CLobstarSholder& rhs);
	virtual ~CLobstarSholder() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CShader*			m_pModelShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CModel*				m_pModelCom = nullptr;
private:
	_int				m_iTexIndex = 0;
	_float				m_fRotNoise = 0.f;
	_float				m_fMoveUV = 0.f;
	_float				m_fFireEffectTime = 0.1f;
	SHOULDERDESC		m_tShoulderDesc;
	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CLobstarSholder*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END