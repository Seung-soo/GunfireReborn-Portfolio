#pragma once

#include "Base.h"

BEGIN(Engine)

class CComponent;
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;
public:
	CComponent*		Get_Component(const _tchar* pComponentTag);

	_float			Get_Depth() { return m_fDepth; }

	virtual void	Set_Dead(_bool bDead) { m_bDead = bDead; }
	virtual _bool	Get_Dead() { return m_bDead; }

	void			Set_Pass(_uint iPass) { m_iPass = iPass; }
	_float			Get_AlphaDepth();
public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
public:
	HRESULT			Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** pOut, void* pArg = nullptr);
protected:
	ID3D11Device*			m_pDevice; 
	ID3D11DeviceContext*	m_pDeviceContext;
	
	class CTransform*		m_pTransformCom = nullptr;
	static const _tchar*	m_pTransformTag;
protected:
	map<const _tchar*, CComponent*>			m_Components;
	typedef map<const _tchar*, CComponent*> COMPONENTS;

	_float					m_fDepth = 0.f;
	_uint					m_iPass = 0;
	_bool					m_isCloned = false;
	_bool					m_bDead = false;
	ContainmentType			m_FrustumType = CONTAINS;
protected:
	CComponent*		Find_Component(const _tchar* pComponentTag);
public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END