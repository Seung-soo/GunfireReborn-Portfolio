#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectXZ final : public CVIBuffer
{
public:
	explicit CVIBuffer_RectXZ(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_RectXZ(const CVIBuffer_RectXZ& rhs);
	virtual ~CVIBuffer_RectXZ() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	static CVIBuffer_RectXZ*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*			Clone(void* pArg);
	virtual void				Free() override;
};

END