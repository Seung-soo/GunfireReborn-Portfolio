#pragma once

#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Input_Device.h"
#include "SoundMgr.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "Picking.h"
#include "Collision_Manager.h"
#include "Target_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	MAKE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;
public:
	HRESULT			Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const CGraphic_Device::GRAPHICDESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	_int			Tick_Engine(_double dTimeDelta);
	HRESULT			Clear_LevelResource(_uint iLevelIndex);
public:		// �׷��� ����̽� ���
	HRESULT			Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT			Clear_DepthStencil_View();
	HRESULT			Present();
public:		// Ÿ�̸� �Ŵ��� ���
	_double			Get_TimeDelta(const _tchar* pTimerTag); // ��ŸŸ�� ��ȯ
	HRESULT			Add_Timer(const _tchar* pTimerTag);		// ���ο� Ÿ�̸� �߰�
public:		// ���� �Ŵ��� ���
	HRESULT			OpenLevel(_uint iNextLevelIndex, CLevel* pNextLevel);
	HRESULT			Render_Level();
	_uint			Get_LevelIndex();
public:		// ������Ʈ �Ŵ��� ���
	CLayer*				Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag);
	CComponent*			Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	HRESULT				Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT				Add_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT				ThrowToLayer(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT				Add_ObjectPool(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT				FrontChange(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT				Release_LayerObject(_uint iLevelIndex, const _tchar* pLayerTag);
	size_t				Get_Size(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject*		Get_Front(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject*		Get_Back(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject*		Get_ObjectPoolBack(_uint iLevelIndex, const _tchar* pLayerTag);
	list<CGameObject*>*	Get_ObjectList(_uint iLevelIndex, const _tchar* pLayerTag);
public:		// ������Ʈ �Ŵ��� ���
	HRESULT			Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent*		Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
	CComponent*		Find_Component(_uint iLevelIndex, const _tchar* pPrototypeTag);
public:		// ��ǲ ����̽� ���
	_byte			Get_DIKeyState(_ubyte eKeyID);

	_long			Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState);
	_byte			Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState);
	_bool			Get_Mouse_Enter(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState);
	_bool			Get_Mouse_Up(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState);
	_bool			Get_KeyEnter(_ubyte eKeyID);
	_bool			Get_KeyUp(_ubyte eKeyID);
public:		// ���� �Ŵ��� ���
	int				VolumeUp(CSoundMgr::CHANNELID eID, _float _vol);
	int				VolumeDown(CSoundMgr::CHANNELID eID, _float _vol);
	int				BGMVolumeUp(_float _vol);
	int				BGMVolumeDown(_float _vol);
	int				Pause(CSoundMgr::CHANNELID eID);
	void			LetsPlaySound(TCHAR* pSoundKey, CSoundMgr::CHANNELID eID, _float _vol);
	void			PlayRoofSound(TCHAR* pSoundKey, CSoundMgr::CHANNELID eID, _float _vol);
	void			PlayBGM(TCHAR* pSoundKey, _float _vol);
	void			StopSound(CSoundMgr::CHANNELID eID);
	void			StopAll();
public:		// ���������� ���
	HRESULT			Set_Transform(CPipeLine::TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix);
	_matrix			Get_TransformMatrix(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_float4x4		Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_float4x4		Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_vector			Get_CamPosition();
	ContainmentType	Frustum(_vector vPosition, _float fRadius);
	void			PipeLineUpdate();
public:		// ����Ʈ �Ŵ��� ���
	const LIGHTDESC*	Get_LightDesc(_uint iIndex) const;
	HRESULT				Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	HRESULT				Release_Light();
public:		// �ݸ��� �Ŵ��� ���
	list<CCollider*>*	Get_ColliderList(const _tchar* pColliderTag, CCollision_Manager::TARGET eTarget);
	HRESULT				Add_Collider(const _tchar* pColliderTag, CCollision_Manager::TARGET eTarget, CCollider* pCollider);
	void				Clear_Collider();
	_bool				Collision_AABB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_OBB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_RayToSphere(_fvector vStartPosition, _fvector vDir, CCollider* pDestCollider, _float* pDist);
	_bool				Collision_AABBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider);
	_bool				Collision_OBBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider);
private:
	CGraphic_Device*		m_pGraphic_Device = nullptr;
	CTimer_Manager*			m_pTimer_Manager = nullptr;
	CLevel_Manager*			m_pLevel_Manager = nullptr;
	CObject_Manager*		m_pObject_Manager = nullptr;
	CComponent_Manager*		m_pComponent_Manager = nullptr;
	CInput_Device*			m_pInput_Device = nullptr;
	CSoundMgr*				m_pSound_Manager = nullptr;
	CPipeLine*				m_pPipeLine = nullptr;
	CLight_Manager*			m_pLight_Manager = nullptr;
	CPicking*				m_pPicking = nullptr;
	CCollision_Manager*		m_pCollision_Manager = nullptr;
	CTarget_Manager*		m_pTargetManager = nullptr;
public:
	static void		Release_Engine();
	virtual void	Free() override;
};

END