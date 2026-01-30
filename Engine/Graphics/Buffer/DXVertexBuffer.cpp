#include "pch.h"
#include "DXVertexBuffer.h"

DXVertexBuffer::DXVertexBuffer()
{
}

DXVertexBuffer::~DXVertexBuffer()
{
}

void DXVertexBuffer::PushData()
{
	DX_DEVICE_CONTEXT->IASetVertexBuffers(_mSlot, 1, _mVertexBuffer.GetAddressOf(), &_mStride, &_mOffset);
}
