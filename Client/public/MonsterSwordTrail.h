#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)

class CMonsterSwordTrail final : public CGameObject
{
public:
	typedef struct tagSwordTrail
	{
		_matrix		RotationMatrix = XMMatrixIdentity();
		_float3		vTranslation = _float3(0.f, 0.f, 0.f);
		CTransform*	pParentTransform = nullptr;
		_float		fTrailSpeed = 0.f;
		_float		fTrailYSpeed = 0.f;
		_vector		vTrailScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		_tchar*		pModelTag = nullptr;
		_tchar*		pTextureTag = nullptr;
		_float		fFirstUV = 0.f;
		_float		fFirstUV_Y = 0.f;
		_bool		bUseNoise = false;
	}SWORDTRAIL;
private:
	CMonsterSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonsterSwordTrail(const CMonsterSwordTrail& rhs);
	virtual ~CMonsterSwordTrail() = default;

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
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
private:
	SWORDTRAIL			m_tSwordTrail;
	_float				m_fMoveUV = 1.f;
	_float				m_fDelayTime = 0.5f;
	_matrix				m_LocalMatrix = XMMatrixIdentity();
	_float				m_fAddNoiseUV = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CMonsterSwordTrail*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END