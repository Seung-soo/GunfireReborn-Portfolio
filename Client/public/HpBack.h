#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CElementIcon;
class CHpBack final : public CGameObject
{
public:
	typedef struct tagHpBackDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
		CGameObject*	pParent = nullptr;
	}HPBACKDESC;
private:
	CHpBack(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CHpBack(const CHpBack& rhs);
	virtual ~CHpBack() = default;
public:
	void			Set_Render(_bool bUIRender) { m_bUIRender = bUIRender; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
public:
	HRESULT			Add_ElementIcon(_uint iElementType);
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	HPBACKDESC			m_tHpBackDesc = {};
	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;
	
	_bool				m_bUIRender = false;
private:
	vector<CElementIcon*>	m_ElementIcons;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT	ElementIcon_Arrange();
public:
	static CHpBack*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END