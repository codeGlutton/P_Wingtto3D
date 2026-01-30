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

void DXComputeTextureBuffer::Init(std::shared_ptr<DXTexture2D> initData)
{
	D3D11_TEXTURE2D_DESC srcDesc = initData->GetDesc();

	_width = srcDesc.Width;
	_height = srcDesc.Height;
	_arraySize = srcDesc.ArraySize;
	_format = srcDesc.Format;

	ComPtr<ID3D11Texture2D> texture = initData->GetResource();
	ASSERT_MSG(texture != nullptr, "Empty texture resource can't be used for compute texture buffer");

	CreateInput(texture);
	CreateBuffer();
}

void DXComputeTextureBuffer::PushData(std::shared_ptr<DXTexture2D> data)
{
	Init(data);
}

void DXComputeTextureBuffer::PopData(OUT std::shared_ptr<DXTexture2D> data)
{
	data = std::make_shared<DXTexture2D>();
	data->Init(_outputSRV);
}

void DXComputeTextureBuffer::CreateBuffer()
{
	CreateSRV();
	CreateOutput();
	CreateUAV();
	CreateResult();
}

void DXComputeTextureBuffer::CreateInput(ComPtr<ID3D11Texture2D> templateSrc)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = _width;
	desc.Height = _height;
	desc.ArraySize = _arraySize;
	desc.Format = _format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;

	CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, NULL, _input.GetAddressOf()), "Compute input texture creation is failed");
	DX_DEVICE_CONTEXT->CopyResource(_input.Get(), templateSrc.Get());
}

void DXComputeTextureBuffer::CreateSRV()
{
	D3D11_TEXTURE2D_DESC desc;
	_input->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = _arraySize;

	CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_input.Get(), &srvDesc, _srv.GetAddressOf()), "Compute input SRV creation is failed");
}

void DXComputeTextureBuffer::CreateOutput()
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = _width;
	desc.Height = _height;
	desc.ArraySize = _arraySize;
	desc.Format = _format;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;

	CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, NULL, _output.GetAddressOf()), "Compute output texture creation is failed");
}

void DXComputeTextureBuffer::CreateUAV()
{
	D3D11_TEXTURE2D_DESC desc;
	_output->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = _arraySize;

	CHECK_WIN_MSG(DX_DEVICE->CreateUnorderedAccessView(_output.Get(), &uavDesc, _uav.GetAddressOf()), "Compute output UAV creation is failed");
}

void DXComputeTextureBuffer::CreateResult()
{
	D3D11_TEXTURE2D_DESC desc;
	_input->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = _arraySize;

	CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_output.Get(), &srvDesc, _outputSRV.GetAddressOf()), "Compute result SRV creation is failed");
}