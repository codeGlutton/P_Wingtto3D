#include "pch.h"
#include "DXVertexBuffer.h"

DXVertexBuffer::DXVertexBuffer()
{
}

DXVertexBuffer::~DXVertexBuffer()
{
}

void DXVertexBuffer::Init(uint32 stride, uint32 count, uint32 slot, uint32 offset)
{
	_mStride = stride;
	_mCount = count;

	_mSlot = slot;
	_mResourceFlags = DXResourceFlag::Updatable;
	_mOffset = offset;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		// 정점 버퍼로 바인딩
	desc.ByteWidth = (uint32)(_mStride * _mCount);	// 할당할 메모리 크기

	// GPU는 읽고, CPU가 중간에 Map으로 고침

	desc.Usage = D3D11_USAGE_DYNAMIC; // cpu 쓰기 가능
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 디바이스야! GPU에 메모리 할당 공간 만들어줘~
	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, nullptr, _mVertexBuffer.ReleaseAndGetAddressOf()), "Vertex buffer creation is failed");
}

void DXVertexBuffer::PushData() const
{
	DX_DEVICE_CONTEXT->IASetVertexBuffers(_mSlot, 1, _mVertexBuffer.GetAddressOf(), &_mStride, &_mOffset);
}
