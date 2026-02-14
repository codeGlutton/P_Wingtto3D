#include "pch.h"
#include "DXWidgetBatchMesh.h"

void DXWidgetBatchMesh::Init(uint32 vertexSlot, uint32 vertexOffset, uint32 indexOffset)
{
	_mVertexBuffer = std::make_shared<DXVertexBuffer>();
	_mVertexBuffer->Init(sizeof(UIVertexData), MAX_BATCH_VERTEX, vertexSlot, vertexOffset);
	_mIndexBuffer = std::make_shared<DXIndexBuffer>();
	_mIndexBuffer->Init(MAX_BATCH_INDEX, indexOffset);
}

void DXWidgetBatchMesh::PushData() const
{
	_mVertexBuffer->PushData();
	_mIndexBuffer->PushData();

	DX_DEVICE_CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool DXWidgetBatchMesh::UpdateData(const std::vector<UIVertexData>& vertices, const std::vector<uint32>& indices) const
{
	if (_mVertexBuffer->UpdateData(vertices) == false)
	{
		return false;
	}
	if (_mIndexBuffer->UpdateData(indices) == false)
	{
		return false;
	}
	return true;
}

