#include "stdafx.h"
#include "..\public\NextAria.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "MonsterWave.h"
#include "StageWall.h"
#include "MiniMap_Icon.h"
#include "AbyssalSerpent.h"

CNextAria::CNextAria(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CNextAria::CNextAria(const CNextAria & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNextAria::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNextAria::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pPlayerNavigation = static_cast<CNavigation*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Navigation")));

	m_pPlayerTransform = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player_Hand"), TEXT("Com_Transform")));
	m_vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	m_vOldPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));
	m_fDepth = 5.f;
	return S_OK;
}

_int CNextAria::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		CStageWall* pStageWall = nullptr;
		switch (m_iAriaIndex)
		{
		case CMonsterWave::STAGE3_1_1:
			break;
		case CMonsterWave::STAGE3_1_2:
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_StageWall"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CNextAria::Tick");
				return E_FAIL;
			}
			pStageWall = static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Wall")));
			pStageWall->WallSetting(51, CStageWall::LINE_BC, false);
			pStageWall->Push_Cell(50);
			pStageWall->Push_Cell(48);
			pStageWall->Push_Cell(49);
			pStageWall->Push_Cell(52);
			pStageWall->Push_Cell(53);

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_StageWall"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CNextAria::Tick");
				return E_FAIL;
			}
			pStageWall = static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Wall")));
			pStageWall->WallSetting(92, CStageWall::LINE_AB, false);
			break;
		case CMonsterWave::STAGE3_1_3:
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_StageWall"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CNextAria::Tick");
				return E_FAIL;
			}
			pStageWall = static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Wall")));
			pStageWall->WallSetting(91, CStageWall::LINE_BC, false);
			break;
		case CMonsterWave::STAGE4_1_1:
			break;
		case CMonsterWave::STAGE4_1_2:
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_StageWall"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CNextAria::Tick");
				return E_FAIL;
			}
			pStageWall = static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Wall")));
			pStageWall->WallSetting(34, CStageWall::LINE_BC, false);

			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_StageWall"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CNextAria::Tick");
				return E_FAIL;
			}
			pStageWall = static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Wall")));
			pStageWall->WallSetting(63, CStageWall::LINE_AB, false);
			break;
		case CMonsterWave::STAGE4_1_3:
			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_StageWall"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CNextAria::Tick");
				return E_FAIL;
			}
			pStageWall = static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Wall")));
			pStageWall->WallSetting(62, CStageWall::LINE_CA, false);
			break;
		}

		m_pMiniMap_Icon->Set_Dead(true);
		m_pMiniMap_Icon = nullptr;

		return DEAD;
	}

	if (0 > __super::Tick(TimeDelta))
		return -1;

	m_vPlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vLocalPosition = XMVector3TransformCoord(m_vPlayerPosition, m_WorldMatrixInv);
	_vector vLocalOldPosition = XMVector3TransformCoord(m_vOldPosition, m_WorldMatrixInv);

	if (LEVEL_STAGE3_2 == g_eCurrentLevel)
	{
		if (XMVectorGetZ(vLocalPosition) > 0.f && XMVectorGetZ(vLocalOldPosition) <= 0.f)
		{
			if (FAILED(g_pGameInstance->ThrowToLayer(LEVEL_STAGE3_2, TEXT("Layer_Boss"))))
			{
				MSGBOX("g_pGameInstance->ThrowToLayer returned E_FAIL in CLevel_Stage3_2::Tick");
				return DEAD;
			}
			static_cast<CAbyssalSerpent*>(g_pGameInstance->Get_Back(LEVEL_STAGE3_2, TEXT("Layer_Boss")))->Set_Type(CAbyssalSerpent::TYPE_REAL);


			if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_StageWall"))))
			{
				MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CNextAria::Tick");
				return E_FAIL;
			}
			static_cast<CStageWall*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_Wall")))->WallSetting(18, CStageWall::LINE_AB, false);

			g_pGameInstance->StopSound(CSoundMgr::BGM);
			g_pGameInstance->PlayBGM(TEXT("Stage3_2_2.mp3"), 0.5f);
			m_bDead = true;
		}
	}
	else
	{
		if (XMVectorGetZ(vLocalPosition) < 0.f && XMVectorGetZ(vLocalOldPosition) >= 0.f)
		{
			CMonsterWave::GetInstance()->WaveAppear((CMonsterWave::STAGE)m_iAriaIndex);
			m_bDead = true;
		}
		else if (XMVectorGetZ(vLocalPosition) > 0.f && XMVectorGetZ(vLocalOldPosition) <= 0.f)
		{
			CMonsterWave::GetInstance()->WaveAppear((CMonsterWave::STAGE)m_iAriaIndex);
			m_bDead = true;
		}
	}



	return _int();
}

_int CNextAria::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;


	m_vOldPosition = m_vPlayerPosition;

	SetUp_ViewPortPos();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(m_vViewPortPosition) - g_iWinCX * 0.5f, -XMVectorGetY(m_vViewPortPosition) + g_iWinCY * 0.5f, 0.f, 1.f));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}




HRESULT CNextAria::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CNextAria::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 3)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CNextAria::Render");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CNextAria::WallSetting(_uint iCellIndex, LINE eLine, _uint iAriaIndex)
{
	m_iAriaIndex = iAriaIndex;
	m_pCell = m_pPlayerNavigation->Get_Cell(iCellIndex);

	_vector vRight, vUp, vLook, vPosition;

	switch (eLine)
	{
	case LINE_AB:
		vRight = XMVector4Normalize(XMLoadFloat3(&m_pCell->Get_Line(CCell::LINE_AB)));
		vPosition = (m_pCell->Get_Point(CCell::POINT_A) + m_pCell->Get_Point(CCell::POINT_B)) * 0.5f;
		break;
	case LINE_BC:
		vRight = XMVector4Normalize(XMLoadFloat3(&m_pCell->Get_Line(CCell::LINE_BC)));
		vPosition = (m_pCell->Get_Point(CCell::POINT_C) + m_pCell->Get_Point(CCell::POINT_B)) * 0.5f;
		break;
	case LINE_CA:
		vRight = XMVector4Normalize(XMLoadFloat3(&m_pCell->Get_Line(CCell::LINE_CA)));
		vPosition = (m_pCell->Get_Point(CCell::POINT_A) + m_pCell->Get_Point(CCell::POINT_C)) * 0.5f;
		break;
	}
	vPosition = XMVectorSetW(vPosition, 1.f);

	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	vUp = XMVector3Cross(vLook, vRight);
	m_WorldMatrixInv.r[0] = vRight;
	m_WorldMatrixInv.r[1] = vUp;
	m_WorldMatrixInv.r[2] = vLook;
	m_WorldMatrixInv.r[3] = XMVectorSetW(vPosition, 1.f);

	m_WorldMatrixInv = XMMatrixInverse(nullptr, m_WorldMatrixInv);

	m_vWorldPosition = vPosition;
	XMStoreFloat4(&m_vWorldPos, m_vWorldPosition);

	if (FAILED(SetUp_OnTerrain()))
	{
		MSGBOX("SetUp_OnTerrain returned E_FAIL in CNextAria:WallSetting");
		return E_FAIL;
	}

	INFO_UI tInfoUI = {};
	tInfoUI.fScaleX = 16.f;
	tInfoUI.fScaleY = 16.f;
	tInfoUI.fDepth = 3.f;
	tInfoUI.iTextureIndex = CMiniMap_Icon::TARGET_ARIA;

	if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon"), TEXT("Prototype_GameObject_MiniMap_Icon"), &tInfoUI)))
	{
		MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CNextAria::NativeConstruct");
		return E_FAIL;
	}
	m_pMiniMap_Icon = static_cast<CMiniMap_Icon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon")));
	m_pMiniMap_Icon->Set_AriaPos(&m_vWorldPos);

	SetUp_ViewPortPos();

	return S_OK;
}

HRESULT CNextAria::Push_Cell(_uint iCellIndex)
{
	CCell* pCell = m_pPlayerNavigation->Get_Cell(iCellIndex);
	pCell->Set_Type(CCell::TYPE_NOTROAD);
	m_Cells.push_back(pCell);

	return S_OK;
}

HRESULT CNextAria::Restore()
{
	if (nullptr == m_pCell)
	{
		MSGBOX("nullptr == m_pCell in CNextAria::Restore");
		return E_FAIL;
	}

	m_pCell->Set_Type(CCell::TYPE_NONE);
	for (auto& pCell : m_Cells)
		pCell->Set_Type(CCell::TYPE_NONE);

	return S_OK;
}

HRESULT CNextAria::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CNextAria::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CNextAria::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CNextAria::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NextAria"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CNextAria::SetUp_Components(Texture)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CNextAria::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CNextAria::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CNextAria::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNextAria::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CNextAria::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CNextAria::SetUp_ConstantTable");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CNextAria::SetUp_OnTerrain()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CNextAria::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CNextAria::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	// 로컬 포지션 구하기
	_matrix	WorldMatrixInverse = pTerrainTransform->Get_WorldMatrixInverse();

	_vector	vLocalPos = XMVector3TransformCoord(m_vWorldPosition, WorldMatrixInverse);
	_float3 vLocalPosFloat3;
	XMStoreFloat3(&vLocalPosFloat3, vLocalPos);

	m_vWorldPosition = XMVector3TransformCoord(vLocalPos, pTerrainTransform->Get_WorldMatrix());

	////////////////////////// 점프 멈춰!!!!!!!!////////////////////////
	_float fTerrainY = pVIBuffer_Terrain->Compute_Y(vLocalPosFloat3);

	/////////////////////////////////////////////////////////////////////


	m_vWorldPosition = XMVectorSetY(m_vWorldPosition, fTerrainY + 4.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vWorldPosition);


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CNextAria::SetUp_ViewPortPos()
{
	_matrix	ViewMatrix;
	ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	m_vViewPortPosition = XMVector3TransformCoord(m_vWorldPosition, ViewMatrix);

	_matrix	ProjMatrix;
	ProjMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	m_vViewPortPosition = XMVector3TransformCoord(m_vViewPortPosition, ProjMatrix);

	if (XMVectorGetZ(m_vViewPortPosition) < 0.f)
	{
		m_vViewPortPosition = XMVectorSetY(m_vViewPortPosition, -1.f);
	}
	else if (XMVectorGetZ(m_vViewPortPosition) > 1.f)
	{
		m_vViewPortPosition = XMVectorSetY(m_vViewPortPosition, -1.f);
		m_vViewPortPosition = XMVectorSetX(m_vViewPortPosition, XMVectorGetX(m_vViewPortPosition) * -1.f);
	}
	if (XMVectorGetX(m_vViewPortPosition) < -1.f + m_fHalfSize / g_iWinCX)
	{
		_float fMulValue = (-1.f + m_fHalfSize / g_iWinCX) / XMVectorGetX(m_vViewPortPosition);
		m_vViewPortPosition = XMVectorSetX(m_vViewPortPosition, -1.f + m_fHalfSize / g_iWinCX);
		m_vViewPortPosition = XMVectorSetY(m_vViewPortPosition, XMVectorGetY(m_vViewPortPosition) * fMulValue);
	}
	else if (XMVectorGetX(m_vViewPortPosition) > 1.f - m_fHalfSize / g_iWinCX)
	{
		_float fMulValue = (1.f - m_fHalfSize / g_iWinCX) / XMVectorGetX(m_vViewPortPosition);
		m_vViewPortPosition = XMVectorSetX(m_vViewPortPosition, 1.f - m_fHalfSize / g_iWinCX);
		m_vViewPortPosition = XMVectorSetY(m_vViewPortPosition, XMVectorGetY(m_vViewPortPosition) * fMulValue);
	}
	if (XMVectorGetY(m_vViewPortPosition) < -1.f + m_fHalfSize / g_iWinCY)
	{
		_float fMulValue = (-1.f + m_fHalfSize / g_iWinCY) / XMVectorGetY(m_vViewPortPosition);
		m_vViewPortPosition = XMVectorSetX(m_vViewPortPosition, XMVectorGetX(m_vViewPortPosition) * fMulValue);
		m_vViewPortPosition = XMVectorSetY(m_vViewPortPosition, -1.f + m_fHalfSize / g_iWinCY);
	}
	else if (XMVectorGetY(m_vViewPortPosition) > 1.f - m_fHalfSize / g_iWinCY)
	{
		_float fMulValue = (1.f - m_fHalfSize / g_iWinCY) / XMVectorGetY(m_vViewPortPosition);
		m_vViewPortPosition = XMVectorSetX(m_vViewPortPosition, XMVectorGetX(m_vViewPortPosition) * fMulValue);
		m_vViewPortPosition = XMVectorSetY(m_vViewPortPosition, 1.f - m_fHalfSize / g_iWinCY);
	}

	m_vViewPortPosition = XMVectorSet((XMVectorGetX(m_vViewPortPosition) + 1) * (g_iWinCX * 0.5f)
		, (XMVectorGetY(m_vViewPortPosition) - 1) * -(g_iWinCY * 0.5f)
		, 0.f, 1.f);
}

CNextAria * CNextAria::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNextAria*	pInstance = new CNextAria(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CNextAria");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNextAria::Clone(void * pArg)
{
	CNextAria*	pInstance = new CNextAria(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CNextAria");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNextAria::Free()
{
	__super::Free();

	m_Cells.clear();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
