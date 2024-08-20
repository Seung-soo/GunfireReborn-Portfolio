#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)
class CMonster;
class CTigerCannon_Explosion final : public CGameObject
{
public:
	typedef struct tagBulletDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
		_float			fDist = 30.f;
	}BULLETDESC;
private:
	CTigerCannon_Explosion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTigerCannon_Explosion(const CTigerCannon_Explosion& rhs);
	virtual ~CTigerCannon_Explosion() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
private:
	BULLETDESC			m_tBulletDesc = {};

	_float				m_fSprite = 0.f;
	_int				m_iSpriteY = 0;
	_int				m_iCollCount = 0;
private:
	vector<CMonster*>		m_vCollisionMonster;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CTigerCannon_Explosion*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END