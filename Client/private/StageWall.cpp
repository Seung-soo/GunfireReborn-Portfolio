#include "stdafx.h"
#include "..\public\StageWall.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Level_Loading.h"
#include "MiniMap_Icon.h"
CStageWall::CStageWall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CStageWall::CStageWall(const CStageWall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStageWall::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStageWall::NativeConstruct(void * pArg)
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
	m_pTransformCom->Scaled(XMVectorSet(5.f, 5.f, 1.f, 0.f));
	
	return S_OK;
}

_int CStageWall::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		m_pMiniMap_Icon->Set_Dead(true);
		m_pMiniMap_Icon = nullptr;
		return DEAD;
	}

	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (true == m_bPortal)
		Portal();
	
	return _int();
}

_int CStageWall::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;
	m_fRot += (_float)TimeDelta * 60.f;
	if (true == m_bPortal)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), -TimeDelta * 0.1f);
	}
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}




HRESULT CStageWall::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CEnergyOrb::Render");
		return E_FAIL;
	}

	if (false == m_bPortal)
	{
		if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 11)))
		{
			MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CEnergyOrb::Render");
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 35)))
		{
			MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CEnergyOrb::Render");
			return E_FAIL;
		}
	}


	//if (true == m_bGoNextStage)
	//{
	//	_uint i = g_eCurrentLevel;
	//	if (FAILED(g_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL(i + 1)))))
	//	{
	//		MSGBOX("g_pGameInstance->OpenLevel returned E_FAIL in CStageWall::Render");
	//		return E_FAIL;
	//	}
	//	m_bGoNextStage = false;
	//}


	return S_OK;
}

HRESULT CStageWall::WallSetting(_uint iCellIndex, LINE eLine, _bool bPortal)
{
	m_bPortal = bPortal;

	m_pCell = m_pPlayerNavigation->Get_Cell(iCellIndex);
	if (false == m_bPortal)
	{
		m_pCell->Set_Type(CCell::TYPE_NOTROAD);
	}

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

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);


	if (FAILED(SetUp_OnTerrain()))
	{
		MSGBOX("SetUp_OnTerrain returned E_FAIL in CStageWall:WallSetting");
		return E_FAIL;
	}

	m_vPoint[0] = vPosition - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.5f + m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.5f;
	m_vPoint[1] = vPosition + m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.5f + m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.5f;
	m_vPoint[2] = vPosition + m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.5f - m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.5f;
	m_vPoint[3] = vPosition - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.5f - m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.5f;

	_tchar wszPrototypeTag[MAX_PATH] = TEXT("Prototype_Component_Texture_");

	if (false == m_bPortal)
	{
		switch (g_eCurrentLevel)
		{
		case LEVEL_STAGE3_1:
		case LEVEL_STAGE3_2:
			lstrcat(wszPrototypeTag, TEXT("Wall_Stage3"));
			break;
		case LEVEL_STAGE4_1:
			lstrcat(wszPrototypeTag, TEXT("Wall_Stage4"));
			break;
		}
	}
	else
	{
		lstrcat(wszPrototypeTag, TEXT("Portal"));

		INFO_UI tInfoUI = {};
		tInfoUI.fScaleX = 16.f;
		tInfoUI.fScaleY = 16.f;
		tInfoUI.fDepth = 3.f;
		tInfoUI.iTextureIndex = CMiniMap_Icon::TARGET_PORTAL;

		if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon"), TEXT("Prototype_GameObject_MiniMap_Icon"), &tInfoUI)))
		{
			MSGBOX("g_pGameInstance->Add_GameObject returned E_FAIL in CStageWall::NativeConstruct");
			return E_FAIL;
		}
		m_pMiniMap_Icon = static_cast<CMiniMap_Icon*>(g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_MiniMap_Icon")));
		m_pMiniMap_Icon->Set_Target(m_pTransformCom);

		/* For.Com_SourTexture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseFire"), TEXT("Com_SourTexture"), (CComponent**)&m_pSourTextureCom)))
		{
			MSGBOX("__super::Add_Component returned E_FAIL in CStageWall::SetUp_Components(Com_SourTexture)");
			return E_FAIL;
		}
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, wszPrototypeTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CStageWall::SetUp_Components(Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStageWall::Push_Cell(_uint iCellIndex)
{
	CCell* pCell = m_pPlayerNavigation->Get_Cell(iCellIndex);
	pCell->Set_Type(CCell::TYPE_NOTROAD);
	m_Cells.push_back(pCell);

	return S_OK;
}

HRESULT CStageWall::Restore()
{
	if (nullptr == m_pCell)
	{
		MSGBOX("nullptr == m_pCell in CStageWall::Restore");
		return E_FAIL;
	}

	m_pCell->Set_Type(CCell::TYPE_NONE);
	for (auto& pCell : m_Cells)
		pCell->Set_Type(CCell::TYPE_NONE);

	return S_OK;
}

HRESULT CStageWall::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CStageWall::SetUp_Components(Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CStageWall::SetUp_Components(Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CStageWall::SetUp_Components(VIBuffer)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStageWall::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CEnergyOrb::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CStageWall::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CStageWall::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CStageWall::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CStageWall::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (true == m_bPortal)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_AddNoiseUV", &m_fRot, sizeof(_float))))
		{
			MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CStageWall::SetUp_ConstantTable(g_AddNoiseUV)");
			return E_FAIL;
		}
		if (FAILED(m_pSourTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 0)))
		{
			MSGBOX("m_pSourTextureCom->SetUp_OnShader returned E_FAIL in CStageWall::SetUp_ConstantTable");
			return E_FAIL;
		}
		if (FAILED(m_pSourTextureCom->SetUp_OnShader(m_pShaderCom, "g_WindowTexture", 97)))
		{
			MSGBOX("m_pSourTextureCom->SetUp_OnShader returned E_FAIL in CStageWall::SetUp_ConstantTable");
			return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CStageWall::SetUp_OnTerrain()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CVIBuffer_Terrain*	pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"));
	if (nullptr == pVIBuffer_Terrain)
	{
		MSGBOX("pVIBuffer_Terrain is nullptr in CStageWall::SetUp_OnTerrain");
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_eCurrentLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"));
	if (nullptr == pTerrainTransform)
	{
		MSGBOX("pTerrainTrasform is nullptr in CStageWall::SetUp_OnTerrain");
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
	if (LEVEL_STAGE3_2 == g_eCurrentLevel)
		fTerrainY += 3.f;
	_float fHalfScaleY = XMVectorGetY(m_pTransformCom->Get_Scale(CTransform::STATE_UP)) * 0.5f;

	/////////////////////////////////////////////////////////////////////


	vPosition = XMVectorSetY(vPosition, fTerrainY + fHalfScaleY);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CStageWall::Portal()
{
	_vector vCameraPos = g_pGameInstance->Get_CamPosition();
	_vector vCameraLook = static_cast<CTransform*>(g_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK);
	_float fDist = 0.f;

	if (true == TriangleTests::Intersects(vCameraPos, vCameraLook, m_vPoint[0], m_vPoint[1], m_vPoint[2], fDist))
	{
		if (3.f > fDist)
		{
			if (false == m_bMakeUI)
			{
				INFO_UI	tInfoUI;
				tInfoUI.fPositionX = 740.f;
				tInfoUI.fPositionY = 420.f;
				tInfoUI.fScaleX = 128.f;
				tInfoUI.fScaleY = 128.f;
				tInfoUI.iTextureIndex = 99;
				tInfoUI.fDepth = 5.f;
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
				{
					__debugbreak();
					return;
				}
				m_pUI = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_UI"));
				m_bMakeUI = true;
			}
			if (true == g_pGameInstance->Get_KeyUp(DIK_F))
			{
				if (nullptr != m_pUI)
				{
					m_pUI->Set_Dead(true);
					m_pUI = nullptr;
					m_bMakeUI = false;
				}
				g_bGoNextLevel = true;
				m_bDead = true;
			}
		}
		else if (nullptr != m_pUI)
		{
			m_pUI->Set_Dead(true);
			m_pUI = nullptr;
			m_bMakeUI = false;
		}
	}
	else if (true == TriangleTests::Intersects(vCameraPos, vCameraLook, m_vPoint[0], m_vPoint[2], m_vPoint[3], fDist))
	{
		if (3.f > fDist)
		{
			if (false == m_bMakeUI)
			{
				INFO_UI	tInfoUI;
				tInfoUI.fPositionX = 740.f;
				tInfoUI.fPositionY = 420.f;
				tInfoUI.fScaleX = 128.f;
				tInfoUI.fScaleY = 128.f;
				tInfoUI.iTextureIndex = 99;
				tInfoUI.fDepth = 5.f;
				if (FAILED(g_pGameInstance->Add_GameObject(g_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &tInfoUI)))
				{
					__debugbreak();
					return;
				}
				m_pUI = g_pGameInstance->Get_Back(g_eCurrentLevel, TEXT("Layer_UI"));
				m_bMakeUI = true;
			}
			if (true == g_pGameInstance->Get_KeyUp(DIK_F))
			{
				g_bGoNextLevel = true;
				m_bDead = true;
				if (nullptr != m_pUI)
				{
					m_pUI->Set_Dead(true);
					m_pUI = nullptr;
					m_bMakeUI = false;
				}
			}
		}
		else if (nullptr != m_pUI)
		{
			m_pUI->Set_Dead(true);
			m_pUI = nullptr;
			m_bMakeUI = false;
		}
	}
	else if (nullptr != m_pUI)
	{
		m_pUI->Set_Dead(true);
		m_pUI = nullptr;
		m_bMakeUI = false;
	}


}



CStageWall * CStageWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStageWall*	pInstance = new CStageWall(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CStageWall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStageWall::Clone(void * pArg)
{
	CStageWall*	pInstance = new CStageWall(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CStageWall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStageWall::Free()
{
	__super::Free();

	m_Cells.clear();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
