#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CCoolTime final : public CGameObject
{
public:
	enum SKILL_TYPE { TYPE_ENERGE_ORB, TYPE_DASH, TYPE_END };
	
	typedef struct tagCoolTimeDesc
	{
		INFO_UI		tInfoUI = {};
		SKILL_TYPE	eSkill_Type = TYPE_END;
		_float*		pCurrentCoolTime = nullptr;
		_float*		pMaxCoolTime = nullptr;
	}COOLTIMEDESC;
private:
	explicit CCoolTime(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCoolTime(const CCoolTime& rhs);
	virtual ~CCoolTime() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
private:
	_float4x4			m_ProjMatrix;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
private:
	COOLTIMEDESC		m_tCoolTimeDesc;
	CGameObject*		m_pNumberUI = nullptr;
	_int				m_iCurrentTime = 0;
private:
	HRESULT				SetUp_Components();
public:
	static CCoolTime*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END