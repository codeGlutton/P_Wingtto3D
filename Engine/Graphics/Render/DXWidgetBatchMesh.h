#pragma once

#include "Graphics/DXDefaultVertexData.h"

#include "Graphics/Buffer/DXVertexBuffer.h"
#include "Graphics/Buffer/DXIndexBuffer.h"

constexpr auto MAX_BATCH_VERTEX = 1u << 15;
constexpr auto MAX_BATCH_INDEX = 1u << 16;

class DXWidgetBatchMesh
{
public:
	void Init(uint32 vertexSlot = 0u, uint32 vertexOffset = 0u, uint32 indexOffset = 0u);
	void PushData() const;
	bool UpdateData(const std::vector<UIVertexData>& vertices, const std::vector<uint32>& indices) const;

public:
	uint32 GetVertexSlot() const
	{
		return _mVertexBuffer->GetSlot();
	}
	uint32 GetVertexOffset() const
	{
		return _mVertexBuffer->GetOffset();
	}
	uint32 GetIndexOffset() const
	{
		return _mIndexBuffer->GetOffset();
	}

	void SetVertexSlot(uint32 slot) const
	{
		_mVertexBuffer->SetSlot(slot);
	}
	void GetVertexOffset(uint32 offset) const
	{
		_mVertexBuffer->SetOffset(offset);
	}
	void GetIndexOffset(uint32 offset) const
	{
		_mIndexBuffer->SetOffset(offset);
	}

private:
	std::shared_ptr<DXVertexBuffer> _mVertexBuffer;
	std::shared_ptr<DXIndexBuffer> _mIndexBuffer;
};

