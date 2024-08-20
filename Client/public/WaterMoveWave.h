#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CWaterMoveWave final : public CGameObject
{
public:
	typedef struct tagMoveWaveDesc
	{
		_bool		bIsRight = true;
		CTransform*	pBossTransform = nullptr;
	}MOVEWAVEDESC;
private:
	CWaterMoveWave(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWaterMoveWave(const CWaterMoveWave& rhs);
	virtual ~CWaterMoveWave() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
private:
	_float				m_fAddNoiseUV = 0.f;
	_float				m_fMoveUV = 0.f;
	_float				m_fTime = 0.f;
	_float				m_fScale = 1.f;
	_float				m_fAlpha = 0.f;
	_int				m_iPass = 22;
	_float				m_fScaleY = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CWaterMoveWave*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END