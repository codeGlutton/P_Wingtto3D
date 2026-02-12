#include "pch.h"
#include "DXStructureBuffer.h"

#include "Manager/DXGraphicSystem.h"

DXStructureBuffer::DXStructureBuffer()
{
}

DXStructureBuffer::~DXStructureBuffer()
{
}

void DXStructureBuffer::Init(const void* initData, uint32 inputStride, uint32 inputCount, uint32 inputSlot, uint32 outputStride, uint32 outputCount, uint32 outputSlot, bool canCpuWrite)
{
	_mInputStride = inputStride;
	_mInputCount = inputCount;
	_mInputSlot = inputSlot;
	_mOutputStride = outputStride;
	_mOutputCount = outputCount;
	_mOutputSlot = outputSlot;

	if (outputStride == 0 || outputCount == 0)
	{
		_mOutputStride = inputStride;
		_mOutputCount = inputCount;
	}

	_mResourceFlags = DXResourceFlag::HasOutput;
	if (canCpuWrite == true)
	{
		_mResourceFlags |= DXResourceFlag::Updatable;
	}
	
	CreateBuffer(initData);
}

void DXStructureBuffer::PushData() const
{
	DX_DEVICE_CONTEXT->CSSetShaderResources(_mInputSlot, 1, _mInputSRV.GetAddressOf());
	DX_DEVICE_CONTEXT->CSSetUnorderedAccessViews(_mOutputSlot, 1, _mOutputUAV.GetAddressOf(), nullptr);
}

bool DXStructureBuffer::UpdateData(const void* data) const
{
	if (IsUpdatable() == false)
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE subResource;
	DX_DEVICE_CONTEXT->Map(_mInputBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, data, GetInputByteWidth());
	}
	DX_DEVICE_CONTEXT->Unmap(_mInputBuffer.Get(), 0);

	return true;
}

void DXStructureBuffer::PopData(OUT void* data) const
{
	// Output 데이터를 CPU 읽기가 가능한 Result에 옮기기
	DX_DEVICE_CONTEXT->CopyResource(_mResultBuffer.Get(), _mOutputBuffer.Get());

	D3D11_MAPPED_SUBRESOURCE subResource;
	DX_DEVICE_CONTEXT->Map(_mResultBuffer.Get(), 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(data, subResource.pData, GetOutputByteWidth());
	}
	DX_DEVICE_CONTEXT->Unmap(_mResultBuffer.Get(), 0);
}

void DXStructureBuffer::CreateBuffer(const void* initData)
{
	CreateInput(initData);
	CreateSRV();
	CreateOutput();
	CreateUAV();
	CreateResult();
}

void DXStructureBuffer::CreateInput(const void* initData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = GetInputByteWidth();
	// 쉐이더 소스로 쓸꺼임
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	// 리소스를 스트럭쳐 버퍼로 할꺼야
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = _mInputStride;

	if (IsUpdatable() == true)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
	}

	// 존재 시
	if (initData != nullptr)
	{
		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = initData;
		CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, &subResource, _mInputBuffer.GetAddressOf()), "Structure input buffer creation is failed");
	}
	// 기존 데이터 없을 시
	else
	{
		CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, nullptr, _mInputBuffer.GetAddressOf()), "Structure input buffer creation is failed");
	}
}

void DXStructureBuffer::CreateSRV()
{
	D3D11_BUFFER_DESC desc;
	_mInputBuffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srcDesc;
	ZeroMemory(&srcDesc, sizeof(srcDesc));
	srcDesc.Format = DXGI_FORMAT_UNKNOWN;
	srcDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	// 전체 데이터 수
	srcDesc.BufferEx.NumElements = _mInputCount;

	CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_mInputBuffer.Get(), &srcDesc, _mInputSRV.GetAddressOf()), "Structure input SRV creation is failed");
}

void DXStructureBuffer::CreateOutput()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = GetOutputByteWidth();
	// 비순차 접근으로 사용
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	// 리소스를 스트럭쳐 버퍼로 할꺼야
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = _mOutputStride;

	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, nullptr, _mOutputBuffer.GetAddressOf()), "Structure output buffer creation is failed");
}

void DXStructureBuffer::CreateUAV()
{
	D3D11_BUFFER_DESC desc;
	_mOutputBuffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	// 전체 데이터 수
	uavDesc.Buffer.NumElements = _mOutputCount;

	CHECK_WIN_MSG(DX_DEVICE->CreateUnorderedAccessView(_mOutputBuffer.Get(), &uavDesc, _mOutputUAV.GetAddressOf()), "Structure output UAV creation is failed");
}

void DXStructureBuffer::CreateResult()
{
	D3D11_BUFFER_DESC desc;
	_mOutputBuffer->GetDesc(&desc);

	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, nullptr, _mResultBuffer.GetAddressOf()), "Structure result buffer creation is failed");
}