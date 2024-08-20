#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CModel;
class CTransform;
END

BEGIN(Client)

class CMesh final : public CGameObject
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
public:
	void				Set_Dead(_bool bDead) { m_bDead = bDead; }
	HRESULT				Set_Model(_tchar* pPrototypeTag);
	HRESULT				Set_Tex(_tchar* pPrototypeTag);
	HRESULT				SetUp_OnTerrain();
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
private:
	_bool				m_bDead = false;
	INFO_MESH*			m_pInfoMesh = nullptr;
	_float				m_fFrustumRadius = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	//HRESULT	SetUp_OnTerrain();
public:
	static CMesh*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END