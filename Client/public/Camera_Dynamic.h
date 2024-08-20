#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "GameInstance.h"

BEGIN(Engine)
class CModel;
class CRenderer;
class CShader;
END

BEGIN(Client)
class CPlayer;

class CCamera_Dynamic final : public CCamera
{
private:
	explicit CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;
public:
	void			Set_HorizonShake(_float fTime, _float fPower) { m_bHorizenShake = true; m_fHSTime = fTime; m_fShakePower = fPower; }
	void			Set_VerticalShake(_float fTime, _float fPower) { m_bVerticalShake = true; m_fVSTime = fTime; m_fShakePower = fPower; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);

	HRESULT			Player_Chase(_double dTimeDelta);
	_bool			Camera_ZoomIn(_float fRange, _double dTimeDelta);
	HRESULT			Camera_ZoomOut(_float fRange, _double dTimeDelta);
	HRESULT			Camera_Shake();
	HRESULT			Camera_Setting();
	_matrix			Camera_Projection();
private:
	CPlayer*		m_pPlayer = nullptr;
	CGameInstance*	m_pGameInstance = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	_float			m_fHeight = 0.f;
	_float			m_fZoomValue = 0.f;
	_bool			m_bHorizenShake = false;
	_bool			m_bVerticalShake = false;
	_bool			m_bLeftShake = false;
	_bool			m_bTopShake = false;
	_double			m_TimeDelta = 0.0;
	_float			m_fHSTime = 0.f;
	_float			m_fVSTime = 0.f;
	_uint			m_iHShakeInterval = 0;
	_uint			m_iVShakeInterval = 0;
	_float			m_fShakePower = 1.f;
	_bool			m_bReturnRotationH = false;
	_bool			m_bReturnRotationV = false;
private:
	HRESULT			Camera_Move(_double dTimeDelta);
	HRESULT			Camera_Rotation(_double dTimeDelta);
	HRESULT			Player_DirSetting(_double dTimeDelta);
public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END