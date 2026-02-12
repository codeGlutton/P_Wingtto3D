#pragma once

#include "Graphics/Resource/DXResourceInclude.h"
#include "Graphics/Resource/DXResource.h"

class DXBlendState : public DXSharedResource
{
	friend class ResourceManager;

protected:
	DXBlendState();
	~DXBlendState();

public:
	ComPtr<ID3D11BlendState> GetState()
	{
		return _mState;
	}

public:
	void Init(D3D11_RENDER_TARGET_BLEND_DESC desc);
	void PushData() const;

private:
	ComPtr<ID3D11BlendState> _mState;
};

