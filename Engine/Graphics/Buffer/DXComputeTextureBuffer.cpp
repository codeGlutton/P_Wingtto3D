#include "pch.h"
#include "DXComputeTextureBuffer.h"

#include "Manager/DXGraphicSystem.h"
#include "Graphics/Resource/DXTexture.h"

DXComputeTextureBuffer::DXComputeTextureBuffer()
{
}

DXComputeTextureBuffer::~DXComputeTextureBuffer()
{
}

void DXComputeTextureBuffer::Init(const void* initData, uint32 width, uint32 height, DXGI_FORMAT format)
{
	ASSERT_MSG(initData != nullptr, "Null init data can't be allowed to initialize compute texture buffer");

	_mWidth = width;
	_mHeight = height;
	_mArraySize = 1;
	_mFormat = format;

	_mResourceFlags = DXResourceFlag::HasOutput;

	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = _mWidth;
		desc.Height = _mHeight;
		desc.ArraySize = _mArraySize;
		desc.Format = _mFormat;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		if (IsUpdatable() == true)
		{
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		std::size_t pitch, slice;
		DirectX::ComputePitch(format, width, height, pitch, slice);

		D3D11_SUBRESOURCE_DATA data;
		::memset(&data, 0, sizeof(data));

		data.pSysMem = initData;
		data.SysMemPitch = static_cast<uint32>(pitch);

		CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, &data, _mInputTexture.ReleaseAndGetAddressOf()), "Compute input texture creation is failed");
	}
	CreateBuffer();
}

void DXComputeTextureBuffer::Init(const void* initData, uint32 width, uint32 height, uint32 arraySize, DXGI_FORMAT format)
{
	ASSERT_MSG(initData != nullptr, "Null init data can't be allowed to initialize compute texture buffer");

	_mWidth = width;
	_mHeight = height;
	_mArraySize = arraySize;
	_mFormat = format;

	_mResourceFlags = DXResourceFlag::HasOutput;

	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = _mWidth;
		desc.Height = _mHeight;
		desc.ArraySize = _mArraySize;
		desc.Format = _mFormat;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		std::size_t pitch, slice;
		DirectX::ComputePitch(format, width, height, pitch, slice);

		std::vector<D3D11_SUBRESOURCE_DATA> datas(arraySize);
		for (uint32 i = 0; i < arraySize; ++i)
		{
			::memset(&datas[i], 0, sizeof(datas[i]));
			datas[i].pSysMem = reinterpret_cast<const BYTE*>(initData) + i * slice;
			datas[i].SysMemPitch = static_cast<uint32>(pitch);
			datas[i].SysMemSlicePitch = static_cast<uint32>(slice);
		}

		CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, datas.data(), _mInputTexture.ReleaseAndGetAddressOf()), "Compute input texture creation is failed");
	}
	CreateBuffer();
}

void DXComputeTextureBuffer::Init(ComPtr<ID3D11ShaderResourceView> initData)
{
	ComPtr<ID3D11Texture2D> texture;
	initData->GetResource((ID3D11Resource**)texture.ReleaseAndGetAddressOf());

	ASSERT_MSG(texture != nullptr, "Empty texture resource can't be used for compute texture buffer");

	D3D11_TEXTURE2D_DESC srcDesc;
	texture->GetDesc(&srcDesc);

	_mWidth = srcDesc.Width;
	_mHeight = srcDesc.Height;
	_mArraySize = srcDesc.ArraySize;
	_mFormat = srcDesc.Format;

	_mResourceFlags = DXResourceFlag::HasOutput;

	{
		D3D11_TEXTURE2D_DESC inputDesc;
		ZeroMemory(&inputDesc, sizeof(inputDesc));
		inputDesc.Width = _mWidth;
		inputDesc.Height = _mHeight;
		inputDesc.ArraySize = _mArraySize;
		inputDesc.Format = _mFormat;
		inputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		inputDesc.MipLevels = 1;
		inputDesc.SampleDesc.Count = 1;

		CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&inputDesc, nullptr, _mInputTexture.ReleaseAndGetAddressOf()), "Compute input texture creation is failed");
		DX_DEVICE_CONTEXT->CopyResource(_mInputTexture.Get(), texture.Get());
	}
	CreateBuffer();
}

void DXComputeTextureBuffer::PushData() const
{
	DX_DEVICE_CONTEXT->CSSetShaderResources(_mInputSlot, 1, _mInputSRV.GetAddressOf());
	DX_DEVICE_CONTEXT->CSSetUnorderedAccessViews(_mOutputSlot, 1, _mOutputUAV.GetAddressOf(), nullptr);
}

void DXComputeTextureBuffer::PopData(OUT ComPtr<ID3D11ShaderResourceView>& data) const
{
	data = _mResultSRV;
}

void DXComputeTextureBuffer::CreateBuffer()
{
	CreateSRV();
	CreateOutput();
	CreateUAV();
	CreateResult();
}

void DXComputeTextureBuffer::CreateSRV()
{
	D3D11_TEXTURE2D_DESC desc;
	_mInputTexture->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = _mArraySize;

	CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_mInputTexture.Get(), &srvDesc, _mInputSRV.ReleaseAndGetAddressOf()), "Compute input SRV creation is failed");
}

void DXComputeTextureBuffer::CreateOutput()
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = _mWidth;
	desc.Height = _mHeight;
	desc.ArraySize = _mArraySize;
	desc.Format = _mFormat;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;

	CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, NULL, _mOutputTexture.ReleaseAndGetAddressOf()), "Compute output texture creation is failed");
}

void DXComputeTextureBuffer::CreateUAV()
{
	D3D11_TEXTURE2D_DESC desc;
	_mOutputTexture->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = _mArraySize;

	CHECK_WIN_MSG(DX_DEVICE->CreateUnorderedAccessView(_mOutputTexture.Get(), &uavDesc, _mOutputUAV.ReleaseAndGetAddressOf()), "Compute output UAV creation is failed");
}

void DXComputeTextureBuffer::CreateResult()
{
	D3D11_TEXTURE2D_DESC desc;
	_mInputTexture->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = _mArraySize;

	CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_mOutputTexture.Get(), &srvDesc, _mResultSRV.ReleaseAndGetAddressOf()), "Compute result SRV creation is failed");
}