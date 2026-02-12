#pragma once

#include "Graphics/Resource/DXResourceInclude.h"
#include "Graphics/Resource/DXResource.h"

class DXRasterizerState : public DXSharedResource
{
	friend class ResourceManager;

protected:
	DXRasterizerState();
	~DXRasterizerState();

public:
	ComPtr<ID3D11RasterizerState> GetState()
	{
		return _mState;
	}

public:
	void Init(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, bool isClockwise);
	void PushData() const;

private:
	ComPtr<ID3D11RasterizerState> _mState;
};

