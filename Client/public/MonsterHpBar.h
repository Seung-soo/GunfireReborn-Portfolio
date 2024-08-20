#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CMonster;
class CMonsterHpBar final : public CGameObject
{
public:
	enum BarType { HP, SHIELD, GUARD, BARTYPE_END };
public:
	typedef struct tagHpBarDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
		CGameObject*	pParent = nullptr;
	}HPBARDESC;
private:
	CMonsterHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonsterHpBar(const CMonsterHpBar& rhs);
	virtual ~CMonsterHpBar() = default;
public:
	void			Set_Render(_bool bUIRender) { m_bUIRender = bUIRender; }
public:
	virtual HRESULT NativeConstruct_Prototype(BarType eBarType);
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
	BarType				m_eBarType = BARTYPE_END;
	HPBARDESC			m_tHpBarDesc = {};
	_float4x4*			m_pCombineMatrix = nullptr;
	_float4x4*			m_pOffsetMatrix = nullptr;
	_matrix				m_PivotMatrix;
	_matrix				m_TransformMatrix;

	_bool				m_bUIRender = false;
	CMonster::STATUS*	m_pMonsterStatus = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CMonsterHpBar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, BarType eBarType);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END