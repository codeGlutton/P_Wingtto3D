#pragma once

#include "Core/Resource/BulkData.h"

using Texture2DBulkData = BulkWrapper<std::vector<char>>;

class DXTexture2D : public std::enable_shared_from_this<DXTexture2D>
{
public:
	DXTexture2D();
	~DXTexture2D();

public:
	ComPtr<ID3D11Texture2D> GetResource()
	{
		return _mTexture;
	}
	ComPtr<ID3D11ShaderResourceView> GetSRV()
	{
		return _mSRV;
	}
	D3D11_TEXTURE2D_DESC GetDesc() const
	{
		D3D11_TEXTURE2D_DESC desc;
		_mTexture->GetDesc(&desc);
		return desc;
	}

public:
	void Init(ComPtr<ID3D11ShaderResourceView> srv);
	void Init(std::shared_ptr<Texture2DBulkData> bulkData);

public:
	void PushData();

private:
	ComPtr<ID3D11Texture2D> _mTexture;
	ComPtr<ID3D11ShaderResourceView> _mSRV;
	DirectX::ScratchImage _mImg = {};
	uint32 _mWidth;
	uint32 _mHeight;
};

