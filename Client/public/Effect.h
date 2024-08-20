#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CModel;
END

BEGIN(Client)

class CEffect final : public CGameObject
{
public:
	enum EFFECT_TYPE { TYPE_2D, TYPE_SPRITE, TYPE_MULTITEX, TYPE_MESH, TYPE_END };
	enum DELETE_WAY { WAY_FINAL, WAY_ROOFTIME, WAY_ALPHADOWN, WAY_SCALEDOWN, WAY_SCALEUP, WAY_END };
	enum MOVE { MOVE_STAND, MOVE_CHASE, MOVE_DIR, MOVE_END };
	typedef struct tagEffectDesc
	{
		EFFECT_TYPE	eType = TYPE_END;
		DELETE_WAY	eWay = WAY_END;
		MOVE		eMove = MOVE_END;
		_bool*		pDead = nullptr;
		_float		fRoofTime = 0.f;
		_matrix		MyWorldMatrix = XMMatrixIdentity();
		_float4x4*	pParentWorldMatrix = nullptr;
		_vector		vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		_tchar*		pPrototypeTag = nullptr;
		_int		iPass = 0;
		_int		iSpriteNumX = 0;
		_int		iSpriteNumY = 0;
		_int		iSpriteStartX = 1;
		_int		iSpriteStartY = 1;
		_float		fSpriteSpeed = 0.f;
		_int		iNumTexIndex = 0;
		_vector		vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		_float		fDownAlphaSpeed = 1.f;
		_bool		bNotLookCamera = false;
		_bool		bNotTurnY = false;
	}EFFECTDESC;
private:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CVIBuffer_Rect*		m_pVIBuffer = nullptr;
private:
	EFFECTDESC			m_tEffectDesc = {};
	_float				m_fCurrentSpriteIndexX = 0.f;
	_int				m_iCurrentSpriteIndexY = 0;
	_float				m_fTexIndex = 0.f;
	_float				m_fCurrentTime = 0.f;
	_float				m_fAlpha = 0.f;
	_float				m_fScale = 1.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	void	Go_Sprite(_double TimeDelta);
	void	Go_MultiTexture(_double TimeDelta);
public:
	static CEffect*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END