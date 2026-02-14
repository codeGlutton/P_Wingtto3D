#include "pch.h"
#include "DXBlendState.h"

#include "Manager/DXGraphicSystem.h"

DXBlendState::DXBlendState()
{
	_mResourceFlags = DXResourceFlag::None;
	_mSharedResourceType = DXSharedResourceType::BlendState;
}

DXBlendState::~DXBlendState()
{
}

void DXBlendState::Init(D3D11_RENDER_TARGET_BLEND_DESC desc)
{
	D3D11_BLEND_DESC blendDesc;
	memset(&blendDesc, 0, sizeof(blendDesc));

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	blendDesc.RenderTarget[0] = desc;

	CHECK_WIN_MSG(DX_DEVICE->CreateBlendState(&blendDesc, _mState.ReleaseAndGetAddressOf()), "Blend state creation is failed");
}

void DXBlendState::PushData() const
{
	DX_DEVICE_CONTEXT->OMSetBlendState(_mState.Get(), nullptr, 0xFFFFFFFF);
}
