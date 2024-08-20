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
class CBearAttackSmoke final : public CGameObject
{
public:
	typedef struct tagSmokeDesc
	{
		CTransform*		pTransform = nullptr;
		_vector			vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	}SMOKEDESC;
private:
	CBearAttackSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBearAttackSmoke(const CBearAttackSmoke& rhs);
	virtual ~CBearAttackSmoke() = default;

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
private:
	SMOKEDESC			m_tSmokeDesc;

	_float				m_fSprite = 0.f;
	_int				m_iSpriteY = 0;
	_int				m_iMaxSpriteX = 4;
	_int				m_iMaxSpriteY = 4;
	_float				m_fAlpha = 255.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CBearAttackSmoke*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END