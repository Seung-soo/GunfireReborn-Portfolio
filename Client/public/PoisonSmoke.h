#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CPoisonSmoke final : public CGameObject
{
private:
	CPoisonSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPoisonSmoke(const CPoisonSmoke& rhs);
	virtual ~CPoisonSmoke() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
private:
	_vector				m_vStartPos = {};
	_float				m_fThrowPower = 3.5f;

	_float				m_fGravity = 3.f;
	_float				m_fJumpTime = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT	Collision_Terrain();
private:
	HRESULT Throw(_double dTimeDelta);
	void	Smoke_Effect();
public:
	static CPoisonSmoke*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END