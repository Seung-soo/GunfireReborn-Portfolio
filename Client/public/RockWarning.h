#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_RectXZ;
END

BEGIN(Client)

class CRockWarning final : public CGameObject
{
public:
	typedef struct tagWarningDesc
	{
		_vector	vPosition;
		_bool*	pDead = nullptr;
	}WARNINGDESC;
private:
	CRockWarning(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CRockWarning(const CRockWarning& rhs);
	virtual ~CRockWarning() = default;

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
	CVIBuffer_RectXZ*		m_pVIBufferCom = nullptr;
private:
	_float				m_fScale = 1.f;
	WARNINGDESC			m_tWarningDesc;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CRockWarning*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END