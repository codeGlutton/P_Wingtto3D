#include "pch.h"
#include "DXInstancingBuffer.h"


DXInstancingBufferBase::DXInstancingBufferBase()
{
	_mInstancingBuffer = std::make_shared<DXVertexBuffer>();
}

DXInstancingBufferBase::~DXInstancingBufferBase()
{
}

