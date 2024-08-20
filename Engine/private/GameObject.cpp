#include "..\public\GameObject.h"
#include "Component.h"
#include "GameInstance.h"

const _tchar* CGameObject::m_pTransformTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice), m_pDeviceContext(rhs.m_pDeviceContext)
	, m_isCloned(true)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentTag)
{
	return Find_Component(pComponentTag);
}

_float CGameObject::Get_AlphaDepth()
{
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pPipeLine->Get_CamPosition();
	_float fDistance = XMVectorGetX(XMVector4Length(vDir));
	RELEASE_INSTANCE(CPipeLine);
	return fDistance;
}

HRESULT CGameObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void * pArg)
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransform is nullptr in CGameObject::NativeConstruct");
		return E_FAIL;
	}

	if (nullptr != pArg)
		m_pTransformCom->Set_TransformDesc(*(CTransform::TRANSFORMDESC*)pArg);

	if (nullptr != Find_Component(m_pTransformTag))
	{
		MSGBOX("nullptr != Find_Component in CGameObject::NativeConstruct");
		return E_FAIL;
	}

	m_Components.emplace(m_pTransformTag, m_pTransformCom);
	Safe_AddRef(m_pTransformCom);
	return S_OK;
}

_int CGameObject::Tick(_double dTimeDelta)
{


	return NOPROBLEM;
}

_int CGameObject::LateTick(_double dTimeDelta)
{
	return NOPROBLEM;
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** pOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
	{
		MSGBOX("Find_Component is not nullptr in CGameObject::Add_Component");
		return E_FAIL;
	}

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CComponent*		pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
	{
		MSGBOX("pComponent is nullptr in CGameObject::Add_Component");
		return E_FAIL;
	}

	m_Components.emplace(pComponentTag, pComponent);

	*pOut = pComponent;

	Safe_AddRef(pComponent);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

// 이 게임 오브젝트가 해당 컴포넌트를 들고 있는지 검사
CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();
}
