#include "pch.h"
#include "DXRasterizerState.h"

#include "Manager/DXGraphicSystem.h"

DXRasterizerState::DXRasterizerState()
{
	_mResourceFlags = DXResourceFlag::None;
	_mSharedResourceType = DXSharedResourceType::RasterizerState;
}

DXRasterizerState::~DXRasterizerState()
{
}

void DXRasterizerState::Init(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, bool isClockwise)
{
	D3D11_RASTERIZER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.FillMode = fillMode;							// 솔리드하게 픽셀을 채움
	desc.CullMode = cullMode;							// 뒷면은 렌더링하지 않음
	desc.FrontCounterClockwise = isClockwise == false;	// 정면은 시계 방향

	CHECK_WIN_MSG(DX_DEVICE->CreateRasterizerState(&desc, _mState.ReleaseAndGetAddressOf()), "Rasterizer state creation is failed");
}

void DXRasterizerState::PushData() const
{
	DX_DEVICE_CONTEXT->RSSetState(_mState.Get());
}