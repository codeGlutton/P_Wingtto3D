#pragma once

#include "Graphics/Resource/DXResource.h"
#include "Utils/MathUtils.h"

class DXTexture2DBase;

class DXComputeTextureBuffer : public DXResource
{
public:
	DXComputeTextureBuffer();
	~DXComputeTextureBuffer();

public:
	ComPtr<ID3D11ShaderResourceView> GetSRV()
	{
		return _mInputSRV;
	}
	ComPtr<ID3D11UnorderedAccessView> GetUAV()
	{
		return _mOutputUAV;
	}

public:
	uint32 GetWidth() const
	{
		return _mWidth;
	}
	uint32 GetHeight() const
	{ 
		return _mHeight;
	}
	IntPoint<uint32> GetWidthHeight() const
	{
		return IntPoint<uint32>(_mWidth, _mHeight);
	}
	uint32 GetArraySize() const
	{ 
		return _mArraySize; 
	}

	uint32 GetInputSlot() const
	{
		return _mInputSlot;
	}
	uint32 GetOutputSlot() const
	{
		return _mOutputSlot;
	}

	void SetInputSlot(uint32 slot) const
	{
		_mInputSlot = slot;
	}
	void SetOutputSlot(uint32 slot) const
	{
		_mOutputSlot = slot;
	}

public:
	void Init(const void* initData, uint32 width, uint32 height, DXGI_FORMAT format);
	void Init(const void* initData, uint32 width, uint32 height, uint32 arraySize, DXGI_FORMAT format);
	void Init(ComPtr<ID3D11ShaderResourceView> initData);

public:
	void PushData() const;
	void PopData(OUT ComPtr<ID3D11ShaderResourceView>& data) const;

private:
	void CreateBuffer();

private:
	void CreateSRV();
	void CreateOutput();
	void CreateUAV();
	void CreateResult();

private:
	ComPtr<ID3D11Texture2D> _mInputTexture;
	ComPtr<ID3D11ShaderResourceView> _mInputSRV;

	ComPtr<ID3D11Texture2D> _mOutputTexture;
	ComPtr<ID3D11UnorderedAccessView> _mOutputUAV;

	ComPtr<ID3D11ShaderResourceView> _mResultSRV;

private:
	uint32 _mWidth = 0;
	uint32 _mHeight = 0;
	uint32 _mArraySize = 0;
	DXGI_FORMAT _mFormat;

	mutable uint32 _mInputSlot = 0;
	mutable uint32 _mOutputSlot = 0;
};

