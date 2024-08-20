#include "..\public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pSound_Manager(CSoundMgr::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pPicking(CPicking::GetInstance())
	, m_pCollision_Manager(CCollision_Manager::GetInstance())
	, m_pTargetManager(CTarget_Manager::GetInstance())
{
	Safe_AddRef(m_pTargetManager);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pLevel_Manager);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const CGraphic_Device::GRAPHICDESC & GraphicDesc, ID3D11Device ** ppDeviceOut, ID3D11DeviceContext ** ppDeviceContextOut)
{
	if (nullptr == m_pGraphic_Device)
	{
		MSGBOX("m_pGraphic_Device is nullptr in CGameInstance::Initialize_Engine");
		return E_FAIL;
	}

	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Initialize_Engine");
		return E_FAIL;
	}

	if (nullptr == m_pComponent_Manager)
	{
		MSGBOX("m_pComponent_Manager is nullptr in CGameInstance::Initialize_Engine");
		return E_FAIL;
	}

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinCX, GraphicDesc.iWinCY, ppDeviceOut, ppDeviceContextOut)))
	{
		MSGBOX("m_pGraphic_Device->Ready_Graphic_Device returned E_FAIL in CGameInstance::Initialize_Engine");
		return E_FAIL;
	}

	if (FAILED(m_pInput_Device->Ready_Input_Device(hInstance, GraphicDesc.hWnd)))
	{
		MSGBOX("m_pInput_Device->Ready_Input_Device returned E_FAIL in CGameInstance::Initialize_Engine");
		return E_FAIL;
	}

	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
	{
		MSGBOX("m_pObject_Manager->Reserve_Container returned E_FAIL in CGameInstance::Initialize_Engine");
		return E_FAIL;
	}

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
	{
		MSGBOX("m_pComponent_Manager->Reserve_Container returned E_FAIL in CGameInstance::Initialize_Engine");
		return E_FAIL;
	}

	if (FAILED(m_pTargetManager->Initialize(*ppDeviceOut, *ppDeviceContextOut)))
	{
		MSGBOX("m_pTargetManager->Initialize returned E_FAIL in CGameInstance::Initialize_Engine");
		return E_FAIL;
	}

	if (FAILED(m_pLight_Manager->Initialize(*ppDeviceOut, *ppDeviceContextOut)))
	{
		MSGBOX("m_pLight_Manager->Initialize returned E_FAIL in CGameInstance::Initialize_Engine");
		return E_FAIL;
	}

	if (FAILED(m_pPicking->NativeConstruct(*ppDeviceOut, *ppDeviceContextOut, GraphicDesc.hWnd)))
		return E_FAIL;

	m_pSound_Manager->Initialize();

	return S_OK;
}

_int CGameInstance::Tick_Engine(_double dTimeDelta)
{
	if (nullptr == m_pLevel_Manager)
	{
		MSGBOX("m_pLevel_Manager is nullptr in CGameInstance::Tick_Engine");
		return -1;
	}

	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Tick_Engine");
		return -1;
	}

	if (FAILED(m_pInput_Device->SetUp_InputDeviceState()))
	{
		MSGBOX("m_pInput_Device->SetUp_InputDeviceState returned E_FAIL in CGameInstance::Tick_Engine");
		return -1;
	}


	if (0 > m_pObject_Manager->Tick(dTimeDelta))
	{
		MSGBOX("m_pObject_Manager->Tick returned nagative number in CGameInstance::Tick_Engine");
		return -1;
	}

	if (0 > m_pLevel_Manager->Tick(dTimeDelta))
	{
		MSGBOX("m_pLevel_Manager->Tick returned nagative number in CGameInstance::Tick_Engine");
		return -1;
	}

	m_pPipeLine->Update();
#ifdef _DEBUG
	m_pTargetManager->Update();
#endif
	//m_pCollision_Manager->TargetList_Sort();

	if (0 > m_pObject_Manager->LateTick(dTimeDelta))
	{
		MSGBOX("m_pObject_Manager->LateTick returned nagative number in CGameInstance::Tick_Engine");
		return -1;
	}

	if (0 > m_pLevel_Manager->LateTick(dTimeDelta))
	{
		MSGBOX("m_pLevel_Manager->LateTick returned nagative number in CGameInstance::Tick_Engine");
		return -1;
	}

	m_pLight_Manager->Update();

	Clear_Collider();

	if (FAILED(m_pPicking->Transform_ToWorldSpace()))
		return -1;

	return 0;
}

HRESULT CGameInstance::Clear_LevelResource(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Clear_LevelResource");
		return E_FAIL;
	}

	if (nullptr == m_pComponent_Manager)
	{
		MSGBOX("m_pComponent_Manager is nullptr in CGameInstance::Clear_LevelResource");
		return E_FAIL;
	}

	if (FAILED(m_pObject_Manager->Clear_LevelObject(iLevelIndex)))
	{
		MSGBOX("m_pObject_Manager->Clear_LevelObject returned E_FAIL in CGameInstance::Clear_LevelResource");
		return E_FAIL;
	}

	if (FAILED(m_pComponent_Manager->Clear_LevelComponent(iLevelIndex)))
	{
		MSGBOX("m_pComponent_Manager->Clear_LevelComponent returned E_FAIL in CGameInstance::Clear_LevelResource");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
	{
		MSGBOX("m_pGraphic_Device is nullptr in CGameInstance::Clear_BackBuffer_View");
		return E_FAIL;
	}

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
	{
		MSGBOX("m_pGraphic_Device is nullptr in CGameInstance::Clear_DepthStencil_View");
		return E_FAIL;
	}
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
	{
		MSGBOX("m_pGraphic_Device is nullptr in CGameInstance::Present");
		return E_FAIL;
	}

	return m_pGraphic_Device->Present();
}

_double CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
	{
		MSGBOX("CGameInstance::m_pGraphic_Device is nullptr in CGameInstance::Get_TimeDelta");
		return 0.f;
	}

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
	{
		MSGBOX("CGameInstance::m_pTimer_Manager is nullptr in CGameInstance::Add_Timer");
		return E_FAIL;
	}

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

HRESULT CGameInstance::OpenLevel(_uint iNextLevelIndex, CLevel * pNextLevel)
{
	if (nullptr == m_pLevel_Manager)
	{
		MSGBOX("m_pLevel_Manager is nullptr in CGameInstance::OpenLevel");
		return E_FAIL;
	}

	return m_pLevel_Manager->OpenLevel(iNextLevelIndex, pNextLevel);
}

HRESULT CGameInstance::Render_Level()
{
	if (nullptr == m_pLevel_Manager)
	{
		MSGBOX("m_pLevel_Manager is nullptr in CGameInstance::Render_Level");
		return E_FAIL;
	}

	return m_pLevel_Manager->Render();
}

_uint CGameInstance::Get_LevelIndex()
{
	if (nullptr == m_pLevel_Manager)
	{
		MSGBOX("m_pLevel_Manager is nullptr in CGameInstance::Get_LevelIndex");
		return E_FAIL;
	}

	return m_pLevel_Manager->Get_LevelIndex();
}

CLayer * CGameInstance::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Find_Layer");
		return nullptr;
	}

	return m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Get_Component");
		return nullptr;
	}

	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Add_Prototype");
		return E_FAIL;
	}

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Add_GameObject");
		return E_FAIL;
	}

	return m_pObject_Manager->Add_GameObject(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

HRESULT CGameInstance::ThrowToLayer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::ThrowToLayer");
		return E_FAIL;
	}

	return m_pObject_Manager->ThrowToLayer(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Add_ObjectPool(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Add_ObjectPool");
		return E_FAIL;
	}

	return m_pObject_Manager->Add_ObjectPool(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

HRESULT CGameInstance::FrontChange(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::FrontChange");
		return E_FAIL;
	}

	return m_pObject_Manager->FrontChange(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Release_LayerObject(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Release_LayerObject");
		return E_FAIL;
	}

	return m_pObject_Manager->Release_LayerObject(iLevelIndex, pLayerTag);
}

size_t CGameInstance::Get_Size(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Get_size");
		return E_FAIL;
	}

	return m_pObject_Manager->Get_Size(iLevelIndex, pLayerTag);
}

CGameObject * CGameInstance::Get_Front(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Get_Front");
		return nullptr;
	}

	return m_pObject_Manager->Get_Front(iLevelIndex, pLayerTag);
}

CGameObject * CGameInstance::Get_Back(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Get_Back");
		return nullptr;
	}

	return m_pObject_Manager->Get_Back(iLevelIndex, pLayerTag);
}

CGameObject * CGameInstance::Get_ObjectPoolBack(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Get_ObjectPoolBack");
		return nullptr;
	}

	return m_pObject_Manager->Get_ObjectPoolBack(iLevelIndex, pLayerTag);
}

list<CGameObject*>* CGameInstance::Get_ObjectList(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		MSGBOX("m_pObject_Manager is nullptr in CGameInstance::Get_Front");
		return nullptr;
	}

	return m_pObject_Manager->Get_ObjectList(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
	{
		MSGBOX("m_pComponent_Manager is nullptr in CGameInstance::Add_Prototype");
		return E_FAIL;
	}

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
	{
		MSGBOX("m_pComponent_Manager is nullptr in CGameInstance::Clone_Component");
		return nullptr;
	}

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

CComponent * CGameInstance::Find_Component(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
	{
		MSGBOX("m_pComponent_Manager is nullptr in CGameInstance::Find_Component");
		return nullptr;
	}

	return m_pComponent_Manager->Find_Component(iLevelIndex, pPrototypeTag);
}

_byte CGameInstance::Get_DIKeyState(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(eKeyID);
}

_long CGameInstance::Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMoveState(eMouseMoveState);
}

_byte CGameInstance::Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseButtonState(eMouseButtonState);
}

_bool CGameInstance::Get_Mouse_Enter(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Get_Mouse_Enter(eMouseButtonState);
}

_bool CGameInstance::Get_Mouse_Up(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Get_Mouse_Up(eMouseButtonState);
}

_bool CGameInstance::Get_KeyEnter(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Get_KeyEnter(eKeyID);
}

_bool CGameInstance::Get_KeyUp(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Get_KeyUp(eKeyID);
}

int CGameInstance::VolumeUp(CSoundMgr::CHANNELID eID, _float _vol)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->VolumeUp(eID, _vol);
}

int CGameInstance::VolumeDown(CSoundMgr::CHANNELID eID, _float _vol)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->VolumeDown(eID, _vol);
}

int CGameInstance::BGMVolumeUp(_float _vol)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->BGMVolumeUp(_vol);
}

int CGameInstance::BGMVolumeDown(_float _vol)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->BGMVolumeDown(_vol);
}

int CGameInstance::Pause(CSoundMgr::CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->Pause(eID);
}

void CGameInstance::LetsPlaySound(TCHAR * pSoundKey, CSoundMgr::CHANNELID eID, _float _vol)
{
	if (nullptr == m_pSound_Manager)
	{
		MSGBOX("failed PlaySound");
		return;
	}

	return m_pSound_Manager->PlaySound(pSoundKey, eID, _vol);
}

void CGameInstance::PlayRoofSound(TCHAR * pSoundKey, CSoundMgr::CHANNELID eID, _float _vol)
{
	if (nullptr == m_pSound_Manager)
	{
		MSGBOX("failed PlayRoofSound");
		return;
	}

	return m_pSound_Manager->PlayRoofSound(pSoundKey, eID, _vol);
}

void CGameInstance::PlayBGM(TCHAR* pSoundKey, _float _vol)
{
	if (nullptr == m_pSound_Manager)
	{
		MSGBOX("Failed PlayBGM");
		return;
	}

	return m_pSound_Manager->PlayBGM(pSoundKey, _vol);
}

void CGameInstance::StopSound(CSoundMgr::CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
	{
		MSGBOX("Failed StopSound");
		return;
	}

	return m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	if (nullptr == m_pSound_Manager)
	{
		MSGBOX("Failed StopAll");
		return;
	}

	return m_pSound_Manager->StopAll();
}

HRESULT CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
	{
		MSGBOX("m_pPipeLine is nullptr in CGameInstance::Set_Transform");
		return E_FAIL;
	}

	return m_pPipeLine->Set_Transform(eStateType, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	if (nullptr == m_pPipeLine)
	{
		MSGBOX("m_pPipeLine is nullptr in CGameInstance::Get_TransformMatrix");
		return XMMatrixIdentity();
	}

	return m_pPipeLine->Get_TransformMatrix(eStateType);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	if (nullptr == m_pPipeLine)
	{
		MSGBOX("m_pPipeLine is nullptr in CGameInstance::Get_TransformFloat4x4");
		return _float4x4();
	}

	return m_pPipeLine->Get_TransformFloat4x4(eStateType);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	if (nullptr == m_pPipeLine)
	{
		MSGBOX("m_pPipeLine is nullptr in CGameInstance::Get_TransformFloat4x4_TP");
		return _float4x4();
	}

	return m_pPipeLine->Get_TransformFloat4x4_TP(eStateType);
}

_vector CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
	{
		MSGBOX("m_pPipeLine is nullptr in CGameInstance::Get_TransformFloat4x4_TP");
		return XMVectorZero();
	}

	return m_pPipeLine->Get_CamPosition();
}

ContainmentType CGameInstance::Frustum(_vector vPosition, _float fRadius)
{
	if (nullptr == m_pPipeLine)
	{
		MSGBOX("m_pPipeLine is nullptr in CGameInstance::Get_TransformFloat4x4_TP");
		return CONTAINS;
	}

	return m_pPipeLine->Frustum(vPosition, fRadius);
}

void CGameInstance::PipeLineUpdate()
{
	if (nullptr == m_pPipeLine)
	{
		MSGBOX("m_pPipeLine is nullptr in CGameInstance::PipeLineUpdate");
		return;
	}
	m_pPipeLine->Update();
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex) const
{
	if (nullptr == m_pLight_Manager)
	{
		MSGBOX("m_pLight_Manager is nullptr in CGameInstance::Get_LightDesc");
		return nullptr;
	}

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
	{
		MSGBOX("m_pLight_Manager is nullptr in CGameInstance::Add_Light");
		return E_FAIL;
	}

	return m_pLight_Manager->Add_Light(pDevice, pDeviceContext, LightDesc);
}

HRESULT CGameInstance::Release_Light()
{
	if (nullptr == m_pLight_Manager)
	{
		MSGBOX("m_pLight_Manager is nullptr in CGameInstance::Release_Light");
		return E_FAIL;
	}

	return m_pLight_Manager->Release_Light();
}

list<CCollider*>* CGameInstance::Get_ColliderList(const _tchar * pColliderTag, CCollision_Manager::TARGET eTarget)
{
	if (nullptr == m_pCollision_Manager)
	{
		MSGBOX("m_pCollision_Manager is nullptr in CGameInstance::Get_ColliderList");
		return nullptr;
	}

	return m_pCollision_Manager->Get_ColliderList(pColliderTag, eTarget);
}

HRESULT CGameInstance::Add_Collider(const _tchar * pColliderTag, CCollision_Manager::TARGET eTarget, CCollider * pCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		MSGBOX("m_pCollision_Manager is nullptr in CGameInstance::Add_Collider");
		return E_FAIL;
	}

	return m_pCollision_Manager->Add_Collider(pColliderTag, eTarget, pCollider);
}

void CGameInstance::Clear_Collider()
{
	if (nullptr == m_pCollision_Manager)
	{
		MSGBOX("m_pCollision_Manager is nullptr in CGameInstance::Clear_Collider");
		return;
	}

	return m_pCollision_Manager->Clear_Collider();
}

_bool CGameInstance::Collision_AABB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		MSGBOX("m_pCollision_Manager is nullptr in CGameInstance::Collision_AABB");
		return false;
	}

	return m_pCollision_Manager->Collision_AABB(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_OBB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		MSGBOX("m_pCollision_Manager is nullptr in CGameInstance::Collision_OBB");
		return false;
	}

	return m_pCollision_Manager->Collision_OBB(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Sphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		MSGBOX("m_pCollision_Manager is nullptr in CGameInstance::Collision_Sphere");
		return false;
	}

	return m_pCollision_Manager->Collision_Sphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_RayToSphere(_fvector vStartPosition, _fvector vDir, CCollider * pDestCollider, _float* pDist)
{
	if (nullptr == m_pCollision_Manager)
	{
		MSGBOX("m_pCollision_Manager is nullptr in CGameInstance::Collision_RayToSphere");
		return false;
	}

	return m_pCollision_Manager->Collision_RayToSphere(vStartPosition, vDir, pDestCollider, pDist);
}

_bool CGameInstance::Collision_AABBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		MSGBOX("m_pCollision_Manager is nullptr in CGameInstance::Collision_AABBToSphere");
		return false;
	}

	return m_pCollision_Manager->Collision_AABBToSphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_OBBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		MSGBOX("m_pCollision_Manager is nullptr in CGameInstance::Collision_OBBToSphere");
		return false;
	}

	return m_pCollision_Manager->Collision_OBBToSphere(pSrcCollider, pDestCollider);
}

void CGameInstance::Release_Engine()
{
	if (0 != CGameInstance::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CGameInstance");

	if (0 != CTimer_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CTimer_Manager");

	if (0 != CObject_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CObject_Manager");

	if (0 != CCollision_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CCollision_Manager");

	if (0 != CComponent_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CComponent_Manager");

	if (0 != CPicking::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CPicking");

	if (0 != CSoundMgr::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CSoundMgr");

	if (0 != CPipeLine::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CPipeLine");

	if (0 != CLevel_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CLevel_Manager");

	if (0 != CInput_Device::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CInput_Device");

	if (0 != CLight_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CLight_Manager");

	if (0 != CTarget_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CTarget_Manager");

	if (0 != CGraphic_Device::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CGraphic_Device");
}

void CGameInstance::Free()
{
	Safe_Release(m_pTargetManager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}
