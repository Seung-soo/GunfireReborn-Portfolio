#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CSwordTrail final : public CGameObject
{
public:
	typedef struct tagSwordTrail
	{
		_vector	vAxis;
		_float	fAngle;
		_float3	vTranslation;
	}SWORDTRAIL;
private:
	CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSwordTrail(const CSwordTrail& rhs);
	virtual ~CSwordTrail() = default;

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
	_int				m_iLiveTime = 180;
	_vector				m_vDir;
	CTransform*			m_pCamTransform = nullptr;
	_matrix				m_LocalMatrix = XMMatrixIdentity();
private:
	_float				m_fCurrentSpriteIndexX = 0.f;
	_int				m_iCurrentSpriteIndexY = 0;
	_int				m_iSpriteMaxXIndex = 3;
	_int				m_iSpriteMaxYIndex = 1;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CSwordTrail*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END