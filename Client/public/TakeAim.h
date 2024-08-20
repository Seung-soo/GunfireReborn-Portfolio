#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CTakeAim final : public CGameObject
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
	CTakeAim(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTakeAim(const CTakeAim& rhs);
	virtual ~CTakeAim() = default;
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
	_float				m_fAddNoiseUV = 1.f;
	AIMDESC				m_tAimDesc;
	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CTakeAim*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END