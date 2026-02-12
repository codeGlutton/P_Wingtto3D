#pragma once

#include "Graphics/Resource/DXResourceInclude.h"
#include "Graphics/Resource/DXResource.h"

class DXSamplerState : public DXSharedResource
{
	friend class ResourceManager;

protected:
	DXSamplerState();
	~DXSamplerState();

public:
	ComPtr<ID3D11SamplerState> GetState()
	{
		return _mState;
	}
	uint32 GetSlot() const
	{
		return _mSlot;
	}
	DXResourceUsageFlag::Type GetUsageFlags() const
	{
		return _mUsageFlags;
	}

	void SetSlot(uint32 slot) const
	{
		_mSlot = slot;
	}
	void SetUsageFlags(DXResourceUsageFlag::Type flags) const
	{
		_mUsageFlags = flags;
	}

public:
	bool CanBeUsedFor(DXResourceUsageFlag::Type flag) const
	{
		return _mUsageFlags & flag;
	}

public:
	void Init(
		D3D11_FILTER filter, 
		D3D11_COMPARISON_FUNC func, 
		D3D11_TEXTURE_ADDRESS_MODE uMode,
		D3D11_TEXTURE_ADDRESS_MODE vMode,
		D3D11_TEXTURE_ADDRESS_MODE wMode,
		Color borderColor = Color::Black,
		uint32 slot = 0,
		DXResourceUsageFlag::Type usageFlags = DXResourceUsageFlag::None
	);

	void PushData() const;

private:
	ComPtr<ID3D11SamplerState> _mState;

	/* 비고정적인 부분 */
private:
	mutable uint32 _mSlot;
	mutable DXResourceUsageFlag::Type _mUsageFlags;
};

