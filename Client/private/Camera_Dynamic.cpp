#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "Player.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;
	m_pGameInstance = CGameInstance::GetInstance();
#ifndef USE_IMGUI
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Front(LEVEL_STATIC, TEXT("Layer_Player")));
#endif

	m_pTransformCom->Set_Speed(10.f);

	return S_OK;
}

_int CCamera_Dynamic::Tick(_double dTimeDelta)
{
	if (true == m_bReturnRotationH)
	{
		if (true == m_bLeftShake)
		{
			m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_fShakePower));
			m_bLeftShake = false;
		}
		else
		{
			m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-m_fShakePower));
			m_bLeftShake = true;
		}
		m_bReturnRotationH = false;
	}

	if (true == m_bReturnRotationV)
	{
		if (true == m_bTopShake)
		{
			m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fShakePower));
			m_bTopShake = false;
		}
		else
		{
			m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-m_fShakePower));
			m_bTopShake = true;
		}
		m_bReturnRotationV = false;
	}

	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CCamera_Dynamic::Tick");
		return -1;
	}
	if (FAILED(Camera_Move(dTimeDelta)))
		return -1;
	if (FAILED(Camera_Rotation(dTimeDelta)))
		return -1;



	return _int();
}

_int CCamera_Dynamic::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_TimeDelta = dTimeDelta;

	static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Camera_Shake();


	return _int();
}

HRESULT CCamera_Dynamic::Camera_Shake()
{
	if (true == m_bHorizenShake)
	{
		if (m_iHShakeInterval % 2 == 0)
		{
			if (true == m_bLeftShake)
			{
				m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_fShakePower));
				m_bLeftShake = false;
				m_bReturnRotationH = true;
			}
			else
			{
				m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-m_fShakePower));
				m_bLeftShake = true;
				m_bReturnRotationH = true;
			}
		}
		m_fHSTime -= (_float)m_TimeDelta;
		if (0.f > m_fHSTime)
			m_bHorizenShake = false;
		m_iHShakeInterval++;
		
	}

	if (true == m_bVerticalShake)
	{
		if (m_iVShakeInterval % 2 == 0)
		{
			if (true == m_bTopShake)
			{
				m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fShakePower));
				m_bTopShake = false;
				m_bReturnRotationV = true;
			}
			else
			{
				m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-m_fShakePower));
				m_bTopShake = true;
				m_bReturnRotationV = true;
			}
		}
		m_fVSTime -= (_float)m_TimeDelta;
		if (0.f > m_fVSTime)
			m_bVerticalShake = false;
		m_iVShakeInterval++;
	}
	if (true == m_bHorizenShake || true == m_bVerticalShake)
	{
		if (FAILED(Camera_Setting()))
		{
			__debugbreak();
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CCamera_Dynamic::Camera_Setting()
{
	if (FAILED(g_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInverse())))
	{
		MSGBOX("m_pPipeLine->Set_Transform returned E_FAIL in CCamera::Tick(View)");
		return E_FAIL;
	}

	_matrix		ProjMatrix = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);

	if (FAILED(g_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix)))
	{
		MSGBOX("m_pPipeLine->Set_Transform returned E_FAIL in CCamera::Tick(Proj)");
		return E_FAIL;
	}
	g_pGameInstance->PipeLineUpdate();

	return S_OK;
}

_matrix CCamera_Dynamic::Camera_Projection()
{
	return XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);
}

HRESULT CCamera_Dynamic::Camera_Move(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nulltpr in CCamera_Dynamic::Camera_Move");
		return E_FAIL;
	}
#ifdef USE_IMGUI

	if (m_pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
	{
		m_pTransformCom->Go_Straight(dTimeDelta);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) & 0x80)
	{
		m_pTransformCom->Go_Backward(dTimeDelta);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
	{
		m_pTransformCom->Go_Left(dTimeDelta);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
	{
		m_pTransformCom->Go_Right(dTimeDelta);
	}

#else
	//if (FAILED(Player_Chase(dTimeDelta)))
	//	return E_FAIL;
	if (g_eCurrentLevel != LEVEL_MAINMANU)
	{
		if (FAILED(Player_DirSetting(dTimeDelta)))
			return E_FAIL;
	}

#endif
	return S_OK;
}

HRESULT CCamera_Dynamic::Camera_Rotation(_double dTimeDelta)
{
#ifdef USE_IMGUI
	static bool cameraStop = false;
	if (m_pGameInstance->Get_DIKeyState(DIK_1) & 0x80)
		cameraStop = true;

	if (m_pGameInstance->Get_DIKeyState(DIK_2) & 0x80)
		cameraStop = false;

	if (cameraStop)
		return S_OK;
#endif
	if (true == g_bCameraAction)
	{
		m_fHeight = 1.1f;
		return NOPROBLEM;
	}

	if (nullptr == m_pTransformCom)
	{
		MSGBOX("m_pTransformCom is nulltpr in CCamera_Dynamic::Camera_Rotation");
		return E_FAIL;
	}
	_long		MouseMove = 0;

	if (MouseMove = m_pGameInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta * MouseMove * 0.1f);
	}

	if (MouseMove = m_pGameInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
	{
		m_pTransformCom->CameraTurn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * MouseMove * 0.1f);
	}

	return S_OK;
}

_bool CCamera_Dynamic::Camera_ZoomIn(_float fRange, _double dTimeDelta)
{
	CTransform* pPlayerHandTrans = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")));
	if (XMConvertToRadians(fRange) < m_CameraDesc.fFovy)
	{
		m_CameraDesc.fFovy -= XMConvertToRadians((_float)dTimeDelta * 2.f);
		m_fZoomValue += 0.0150f;
	}
	else if (XMConvertToRadians(fRange) > m_CameraDesc.fFovy)
	{
		m_CameraDesc.fFovy = XMConvertToRadians(fRange);
		return true;
	}
	else
		return true;

	pPlayerHandTrans->Set_State(CTransform::STATE_POSITION, pPlayerHandTrans->Get_State(CTransform::STATE_POSITION) - pPlayerHandTrans->Get_State(CTransform::STATE_RIGHT) * (m_fZoomValue * 0.9f) + pPlayerHandTrans->Get_State(CTransform::STATE_LOOK) * m_fZoomValue * 2.f + pPlayerHandTrans->Get_State(CTransform::STATE_UP) * m_fZoomValue * 0.4f);


	return false;
}

HRESULT CCamera_Dynamic::Camera_ZoomOut(_float fRange, _double dTimeDelta)
{
	CTransform* pPlayerHandTrans = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")));

	if (XMConvertToRadians(fRange) > m_CameraDesc.fFovy)
	{
		m_CameraDesc.fFovy += XMConvertToRadians((_float)dTimeDelta * 2.f);
		m_fZoomValue -= 0.0150f;
	}
	else if (XMConvertToRadians(fRange) < m_CameraDesc.fFovy)
	{
		m_fZoomValue = 0.f;
		m_CameraDesc.fFovy = XMConvertToRadians(fRange);
	}
	pPlayerHandTrans->Set_State(CTransform::STATE_POSITION, pPlayerHandTrans->Get_State(CTransform::STATE_POSITION) - pPlayerHandTrans->Get_State(CTransform::STATE_RIGHT) * (m_fZoomValue * 0.9f) + pPlayerHandTrans->Get_State(CTransform::STATE_LOOK) * m_fZoomValue * 2.f + pPlayerHandTrans->Get_State(CTransform::STATE_UP) * m_fZoomValue * 0.4f);

	return S_OK;
}

HRESULT CCamera_Dynamic::Player_Chase(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_pPlayer)
	{
		MSGBOX("m_pTransformCom or m_pPlayer is nulltpr in CCamera_Dynamic::Player_Chase");
		return E_FAIL;
	}

	_vector vPlayerPosition = static_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLook = static_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_LOOK);

	vPlayerPosition = XMVectorSetY(vPlayerPosition, XMVectorGetY(vPlayerPosition) + 1.f);
	//vPlayerPosition += vPlayerLook * 0.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition);
	return S_OK;
}

HRESULT CCamera_Dynamic::Player_DirSetting(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_pPlayer)
	{
		MSGBOX("m_pTransformCom or m_pPlayer is nulltpr in CCamera_Dynamic::Player_DirSetting");
		return E_FAIL;
	}

	CTransform* vPlayerTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	_vector vPlayerWantLook = XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	
	vPlayerTransform->Set_State(CTransform::STATE_RIGHT, m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	vPlayerTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	vPlayerTransform->Set_State(CTransform::STATE_LOOK, vPlayerWantLook);

	return S_OK;
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void * pArg)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	/*XMPlaneFromPoints()*/
	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();
}
