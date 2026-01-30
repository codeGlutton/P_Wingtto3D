#pragma once

class DXTexture2D;

class DXComputeTextureBuffer : public std::enable_shared_from_this<DXComputeTextureBuffer>
{
public:
	DXComputeTextureBuffer();
	~DXComputeTextureBuffer();

public:
	ComPtr<ID3D11ShaderResourceView> GetSRV()
	{
		return _srv;
	}
	ComPtr<ID3D11UnorderedAccessView> GetUAV()
	{
		return _uav;
	}

public:
	uint32 GetWidth() const
	{
		return _width;
	}
	uint32 GetHeight() const
	{ 
		return _height;
	}
	uint32 GetArraySize() const
	{ 
		return _arraySize; 
	}

public:
	void Init(std::shared_ptr<DXTexture2D> initData);

public:
	void PushData(std::shared_ptr<DXTexture2D> data);
	void PopData(OUT std::shared_ptr<DXTexture2D> data);

private:
	void CreateInput(ComPtr<ID3D11Texture2D> templateSrc);
	void CreateBuffer();

private:
	void CreateSRV();
	void CreateOutput();
	void CreateUAV();
	void CreateResult();

private:
	ComPtr<ID3D11Texture2D> _input;
	ComPtr<ID3D11ShaderResourceView> _srv;
	ComPtr<ID3D11Texture2D> _output;
	ComPtr<ID3D11UnorderedAccessView> _uav;

private:
	uint32 _width = 0;
	uint32 _height = 0;
	uint32 _arraySize = 0;
	DXGI_FORMAT _format;
	ComPtr<ID3D11ShaderResourceView> _outputSRV;
};

