#include "pch.h"
#include "DXIndexBuffer.h"

#include "Manager/DXGraphicSystem.h"

DXIndexBuffer::DXIndexBuffer()
{
}

DXIndexBuffer::~DXIndexBuffer()
{
}

void DXIndexBuffer::Init(std::shared_ptr<IndexBulkData> bulkData, uint32 offset)
{
	const std::vector<uint32>& indices = bulkData->mValue;
	_mStride = sizeof(uint32);
	_mCount = static_cast<uint32>(indices.size());
	_mOffset = offset;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	//ZeroMemory(&desc, size(desc));

	desc.Usage = D3D11_USAGE_IMMUTABLE;				// GPU만 읽을 수 있음
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;		// 입력 어셈블러(사용자 데이터 정리)에서 인덱스 버퍼
	desc.ByteWidth = (uint32)(_mStride * _mCount);	// 할당할 메모리 크기

	// 관련 CPU 데이터 연결 (GPU에 복제됨)
	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = indices.data();

	// 디바이스야! GPU에 메모리 할당 공간 만들어줘~
	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, &data, _mIndexBuffer.GetAddressOf()), "Index buffer creation is failed");
}

void DXIndexBuffer::PushData()
{
	DX_DEVICE_CONTEXT->IASetIndexBuffer(_mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, _mOffset);
}

void DXIndexBuffer::PushData(uint32 offset)
{
	DX_DEVICE_CONTEXT->IASetIndexBuffer(_mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, offset);
}
