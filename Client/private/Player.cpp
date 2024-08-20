#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "HpBar.h"
#include "HpNumber.h"
#include "PoisonSmoke.h"
#include "CoolTime.h"

#define	DASHSPEED	5

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Set_Navigation(const _tchar* pPrototypeTag)
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pNavigationCom);
	m_Components.erase(TEXT("Com_Navigation"));
	/* For.Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;
	NaviDesc.pParent = (CTransform*)g_pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (FAILED(__super::Add_Component(g_eCurrentLevel, pPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer::Set_Navigation(Navigation)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CPlayer::NativeContruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 6.5f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CPlayer::NativeConstruct");
		return E_FAIL;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.5f));

	return S_OK;
}

_int CPlayer::Tick(_double dTimeDelta)
{
	if (nullptr != m_pCursor && g_pGameInstance->Get_KeyUp(DIK_BACKSPACE))
	{
		g_pGameInstance->Get_Front(LEVEL_STATIC, TEXT("Layer_UI_Tab"))->Set_Dead(true);
		g_bCameraAction = false;
		Safe_Release(m_pCursor);
	}

	if (nullptr != m_pCursor)
	{
		m_pCursor->Tick(dTimeDelta);
		return 0;
	}


	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CPlayer::Tick");
		return -1;
	}

	if (FAILED(StateManage(dTimeDelta)))
	{
		MSGBOX("StateManage retuned E_FAIL in CPlayer::Tick");
		return -1;
	}
	if (true == g_bCameraAction)
		return NOPROBLEM;

	if (nullptr == m_pCursor && g_pGameInstance->Get_KeyUp(DIK_TAB))
	{
		g_bCameraAction = true;
		INFO_UI tInfoUI;
		tInfoUI.iTextureIndex = 53;
		tInfoUI.fScaleX = g_iWinCX;
		tInfoUI.fScaleY = g_iWinCY;
		tInfoUI.fPositionX = g_iWinCX * 0.5f;
		tInfoUI.fPositionY = g_iWinCY * 0.5f;
		tInfoUI.fDepth = 0.f;

		if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Tab"), TEXT("Prototype_GameObject_UI_TabUI"), &tInfoUI)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer::Tick");

			return -1;
		}
		m_pCursor = CCursor::Create(m_pDevice, m_pDeviceContext);
	}


	return _int();
}

_int CPlayer::LateTick(_double dTimeDelta)
{
	if (nullptr != m_pCursor)
		return 0;

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CPlayer::LateTick");
		return -1;
	}

	if (CCell::TYPE_ROAD == m_pNavigationCom->Get_Type())
	{
		_float fPlayerHalfScaleY = XMVectorGetY(m_pTransformCom->Get_Scale(CTransform::STATE_UP)) * 0.5f;
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vNavigationPos = XMVectorSetY(vPos, m_pNavigationCom->Compute_Y(vPos) + fPlayerHalfScaleY) ;
		if (true == m_bJump && 0.4f < m_fJumpTime && XMVectorGetY(vPos) < XMVectorGetY(vNavigationPos))
		{
			m_bJump = false;
			m_fJumpTime = 0.f;
		}
		else if(false == m_bJump && XMVectorGetY(vNavigationPos) + 0.05f < XMVectorGetY(vPos))
		{
			m_bJump = true;
			m_fJumpTime = 0.4f;
		}

		if (false == m_bJump)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNavigationPos);
	}
	else if (LEVEL_LOADING != g_eCurrentLevel && FAILED(SetUp_OnTerrain()))
	{
		MSGBOX("SetUp_OnTerrain returned E_FAIL in CPlayer::LateTick");
		return E_FAIL;
	}

	//#ifdef _DEBUG	
	//	m_pRendererCom->Add_Debug(m_pNavigationCom);
	//#endif // _DEBUG

	//static_cast<CCamera_Dynamic*>(g_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Camera")))->Render();

	return _int();
}




HRESULT CPlayer::Render()
{
	if (nullptr != m_pCursor)
		m_pCursor->Render();
	if (nullptr == m_pModelCom)
	{
		MSGBOX("nullptr == m_pModelCom in CPlayer::Render");
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CPlayer::Render");
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 0, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CPlayer::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPlayer::SetUp_Components(Model)");
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CPlayer::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CSky::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPlayer::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CPlayer::SetUp_OnTerrain()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CPlayer::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CPlayer::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	// 로컬 포지션 구하기
	_matrix	WorldMatrixInverse = pTerrainTransform->Get_WorldMatrixInverse();

	_vector	vLocalPos = XMVector3TransformCoord(vPosition, WorldMatrixInverse);
	_float3 vLocalPosFloat3;
	XMStoreFloat3(&vLocalPosFloat3, vLocalPos);

	vPosition = XMVector3TransformCoord(vLocalPos, pTerrainTransform->Get_WorldMatrix());
	
	////////////////////////// 점프 멈춰!!!!!!!!////////////////////////
	_float fTerrainY = pVIBuffer_Terrain->Compute_Y(vLocalPosFloat3);
	_float fPlayerHalfScaleY = XMVectorGetY(m_pTransformCom->Get_Scale(CTransform::STATE_UP)) * 0.5f;
	if (0.4f < m_fJumpTime && fTerrainY > vLocalPosFloat3.y - fPlayerHalfScaleY)
	{
		m_bJump = false;
		m_fJumpTime = 0.f;
	}
	else if (false == m_bJump && fTerrainY + 0.05f < vLocalPosFloat3.y - fPlayerHalfScaleY)
	{
		m_bJump = true;
		m_fJumpTime = 0.4f;
	}
	/////////////////////////////////////////////////////////////////////

	// 플레이어에 대입
	if (false == m_bJump)
	{
		vPosition = XMVectorSetY(vPosition, fTerrainY + fPlayerHalfScaleY);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::StateManage(_double dTimeDelta)
{
	if (false == g_bCameraAction)
	{
		// 대쉬
		if (FAILED(Dash(dTimeDelta)))
		{
			MSGBOX("Move retuned E_FAIL in CPlayer::StateManage");
			return E_FAIL;
		}

		// 움직임
		if (PLAYER_DASH != m_ePlayerState)
		{
			m_ePlayerState = PLAYER_IDLE;
			if (FAILED(Move(dTimeDelta)))
			{
				MSGBOX("Move retuned E_FAIL in CPlayer::StateManage");
				return E_FAIL;
			}
		}

		// 점프
		if (FAILED(Jump(dTimeDelta)))
		{
			MSGBOX("Jump retuned E_FAIL in CPlayer::StateManage");
			return E_FAIL;
		}
	}

	// 플레이어에 카메라 붙히기
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	static_cast<CCamera_Dynamic*>(pGameInstance->Get_Front(LEVEL_STATIC, TEXT("Layer_Camera")))->Player_Chase(dTimeDelta);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::Move(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("nullptr == m_pTransformCom in CPlayer::Move");
		return E_FAIL;
	}
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_bool KeyDown = false;

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
			m_pTransformCom->Go_FrontLeft(dTimeDelta, m_pNavigationCom);
		else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
			m_pTransformCom->Go_FrontRight(dTimeDelta, m_pNavigationCom);
		else
			m_pTransformCom->Go_Straight(dTimeDelta, m_pNavigationCom);
		KeyDown = true;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
			m_pTransformCom->Go_BackLeft(dTimeDelta, m_pNavigationCom);
		else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
			m_pTransformCom->Go_BackRight(dTimeDelta, m_pNavigationCom);
		else
			m_pTransformCom->Go_Backward(dTimeDelta, m_pNavigationCom);
		KeyDown = true;
	}

	else if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransformCom->Go_Left(dTimeDelta, m_pNavigationCom);
		KeyDown = true;
	}


	else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransformCom->Go_Right(dTimeDelta, m_pNavigationCom);
		KeyDown = true;
	}

	if (true == KeyDown)
		m_ePlayerState = PLAYER_WALK;

	if (PLAYER_WALK == m_ePlayerState && false == KeyDown)
		m_ePlayerState = PLAYER_IDLE;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CPlayer::Dash(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("nullptr == m_pTransformCom in CPlayer::Dash");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	static _int		iDashTime = 10;
	static _vector	vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	if (PLAYER_DASH != m_ePlayerState && true == m_bUseDash && pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{
		pGameInstance->StopSound(CSoundMgr::PLAYER_EFFECT);
		pGameInstance->LetsPlaySound(TEXT("dash.mp3"), CSoundMgr::PLAYER_EFFECT, 0.5f);

		vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
			vDir = -m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

		if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
			vDir = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
		{
			if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
				vDir = XMVector4Normalize(-m_pTransformCom->Get_State(CTransform::STATE_RIGHT) + m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
				vDir = XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT) + m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			else
				vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		}


		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
		{
			if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
				vDir = XMVector4Normalize(-m_pTransformCom->Get_State(CTransform::STATE_RIGHT) - m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
				vDir = XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT) - m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			else
				vDir = -m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		}

		if (FAILED(Dash_Effect()))
		{
			MSGBOX("Dash_Effect returned E_FAIL in CPlayer::Dash");
			return E_FAIL;
		}

		m_ePlayerState = PLAYER_DASH;
		m_bUseDash = false;
	}

	if (PLAYER_DASH == m_ePlayerState && 0 < iDashTime--)
		m_pTransformCom->Go_Vector(vDir, dTimeDelta * DASHSPEED, m_pNavigationCom);
	else if (0 >= iDashTime)
	{
		iDashTime = 10;
		m_ePlayerState = PLAYER_IDLE;
	}

	if (false == m_bUseDash)
	{
		if (0.f > m_fDashCoolTime)
		{
			m_bUseDash = true;
			m_fDashCoolTime = m_fDashMaxCoolTime;
		}
		else
			m_fDashCoolTime -= (_float)dTimeDelta;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CPlayer::Jump(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		MSGBOX("nullptr == m_pTransformCom in CPlayer::Jump");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == m_bJump && pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		pGameInstance->StopSound(CSoundMgr::PLAYER);
		pGameInstance->LetsPlaySound(TEXT("Jump.mp3"), CSoundMgr::PLAYER, 0.5f);
		m_bJump = true;
	}
	// 점프 상태일 때 값 변화
	if (PLAYER_DASH != m_ePlayerState && true == m_bJump)
	{
		m_fJumpTime += (_float)dTimeDelta;
		m_pTransformCom->Go_Up(dTimeDelta);
		m_pTransformCom->Go_Down(dTimeDelta * m_fJumpTime * 2.5f);
	}
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::Dash_Effect()
{
	CCoolTime::COOLTIMEDESC tCoolTimeDesc;
	tCoolTimeDesc.tInfoUI.fPositionX = 1040.f;
	tCoolTimeDesc.tInfoUI.fPositionY = 620.f;
	tCoolTimeDesc.tInfoUI.fScaleX = 64.f;
	tCoolTimeDesc.tInfoUI.fScaleY = 64.f;
	tCoolTimeDesc.tInfoUI.fDepth = -0.5f;
	tCoolTimeDesc.tInfoUI.iTextureIndex = 1;
	tCoolTimeDesc.eSkill_Type = CCoolTime::TYPE_DASH;
	tCoolTimeDesc.pCurrentCoolTime = &m_fDashCoolTime;
	tCoolTimeDesc.pMaxCoolTime = &m_fDashMaxCoolTime;

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_CoolTime"), TEXT("Prototype_GameObject_CoolTime"), &tCoolTimeDesc)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer::Dash(CoolTime)");
		return E_FAIL;
	}

	if (FAILED(g_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_DashEffect"), TEXT("Prototype_GameObject_DashEffect"))))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CPlayer::Dash(DashEffect)");
		return E_FAIL;
	}

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pCursor);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
