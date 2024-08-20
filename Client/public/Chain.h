#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CChain final : public CGameObject
{
public:
	enum CHAIN_TYPE { TYPE_CIRCLE, TYPE_LINE, TYPE_PIECE_0, TYPE_PIECE_1, TYPE_PIECE_2, TYPE_PIECE_3, TYPE_PIECE_4, TYPE_END };
	typedef struct tagChainDesc
	{
		_matrix		WorldMatrix;
		_matrix		MyWorldMatrix;
		CHAIN_TYPE	eType;
		_bool*		pChain;
	}CHAINDESC;
private:
	CChain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CChain(const CChain& rhs);
	virtual ~CChain() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
private:
	CHAINDESC			m_tChainDesc = {};
	_vector				m_vDir;
	_float				m_fSpeed;
	_vector				m_vAxis;
	_float				m_fLiveTime = 0.5f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static	CChain*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END