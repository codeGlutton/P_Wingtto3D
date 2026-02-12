#include "pch.h"
#include "DXSamplerState.h"

#include "Manager/DXGraphicSystem.h"

DXSamplerState::DXSamplerState()
{
	_mResourceFlags = DXResourceFlag::None;
	_mSharedResourceType = DXSharedResourceType::SamplerState;
}

DXSamplerState::~DXSamplerState()
{
}

void DXSamplerState::Init(D3D11_FILTER filter, D3D11_COMPARISON_FUNC func, D3D11_TEXTURE_ADDRESS_MODE uMode, D3D11_TEXTURE_ADDRESS_MODE vMode, D3D11_TEXTURE_ADDRESS_MODE wMode, Color borderColor, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	_mSlot = slot;
	_mUsageFlags = usageFlags;

	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.Filter = filter;
	desc.AddressU = uMode;
	desc.AddressV = vMode;
	desc.AddressW = wMode;
	desc.ComparisonFunc = func;
	desc.BorderColor[0] = borderColor.R();
	desc.BorderColor[1] = borderColor.G();
	desc.BorderColor[2] = borderColor.B();
	desc.BorderColor[3] = borderColor.A();

	desc.MaxAnisotropy = 16;
	desc.MipLODBias = 0.f;
	desc.MinLOD = 0.f;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	CHECK_WIN_MSG(DX_DEVICE->CreateSamplerState(&desc, _mState.GetAddressOf()), "Sample state creation is failed");
}

void DXSamplerState::PushData() const
{
	if (_mUsageFlags & DXResourceUsageFlag::Vertex)
	{
		DX_DEVICE_CONTEXT->VSSetSamplers(_mSlot, 1, _mState.GetAddressOf());
	}

	if (_mUsageFlags & DXResourceUsageFlag::Domain)
	{
		DX_DEVICE_CONTEXT->DSSetSamplers(_mSlot, 1, _mState.GetAddressOf());
	}

	if (_mUsageFlags & DXResourceUsageFlag::Hull)
	{
		DX_DEVICE_CONTEXT->HSSetSamplers(_mSlot, 1, _mState.GetAddressOf());
	}

	if (_mUsageFlags & DXResourceUsageFlag::Geometry)
	{
		DX_DEVICE_CONTEXT->GSSetSamplers(_mSlot, 1, _mState.GetAddressOf());
	}

	if (_mUsageFlags & DXResourceUsageFlag::Pixel)
	{
		DX_DEVICE_CONTEXT->PSSetSamplers(_mSlot, 1, _mState.GetAddressOf());
	}

	if (_mUsageFlags & DXResourceUsageFlag::Compute)
	{
		DX_DEVICE_CONTEXT->CSSetSamplers(_mSlot, 1, _mState.GetAddressOf());
	}
}
