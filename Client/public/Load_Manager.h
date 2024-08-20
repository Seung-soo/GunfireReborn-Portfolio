#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CLoad_Manager final : public CBase
{
	MAKE_SINGLETON(CLoad_Manager)
private:
	explicit CLoad_Manager();
	virtual ~CLoad_Manager() = default;
public:
	HRESULT Load_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelIndex, const _tchar* pLoadFullPath);
	HRESULT Make_GameObject(_uint iLevelIndex);
private:
	vector<INFO_MESH*>	m_vInfo_Meshes[LEVEL_END];
	vector<_tchar*>		m_vPrototypeKey;
public:
	virtual void	Free() override;
};

END
