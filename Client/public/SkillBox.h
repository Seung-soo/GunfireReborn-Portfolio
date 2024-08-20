#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Item_Weapon.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CSkillBox final : public CGameObject
{
private:
	CSkillBox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSkillBox(const CSkillBox& rhs);
	virtual ~CSkillBox() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
private:
	CShader*		m_pShaderCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;
private:
	_uint			m_iAnimIndex = 0;
private:
	CTransform*			m_pPlayerTransform = nullptr;
	_bool				m_bOpen = false;
	_bool				m_bItemDrop = false;
	_bool				m_bMakeUI = false;
	CGameObject*		m_pUI = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT	SetUp_OnTerrain();
public:
	static	CSkillBox*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END