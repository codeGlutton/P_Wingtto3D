#include "pch.h"
#include "DXStaticMesh.h"

#include "Manager/DXGraphicSystem.h"

DXStaticMesh::DXStaticMesh()
{
	_mResourceFlags = DXResourceFlag::None;
	_mSharedResourceType = DXSharedResourceType::Mesh;
}

DXStaticMesh::~DXStaticMesh()
{
}

void DXStaticMesh::Init(std::shared_ptr<DXVertexBuffer> vertices, std::shared_ptr<DXIndexBuffer> indices, std::vector<MeshSlotInfo> slotInfos, uint32 materialCount, D3D11_PRIMITIVE_TOPOLOGY primitive)
{
	_mSlotsPerMaterial.clear();

	_mVertexBuffer = vertices;
	_mIndexBuffer = indices;

	_mSlotInfos = std::move(slotInfos);
	_mPrimitive = primitive;

	_mSlotsPerMaterial.resize(materialCount);

	const std::size_t slotSize = _mSlotInfos.size();
	for (std::size_t i = 0; i < slotSize; ++i)
	{
		_mSlotsPerMaterial[_mSlotInfos[i].mMaterialIndex].push_back(i);
	}
}

void DXStaticMesh::Init(std::shared_ptr<StaticMeshBulkData> bulkData, std::size_t targetLODIndex)
{
	_mSlotsPerMaterial.clear();

	StaticMeshLODInfo& meshLODInfo = bulkData->mLODInfos[targetLODIndex];
	_mVertexBuffer = std::make_shared<DXVertexBuffer>();
	_mVertexBuffer->Init(meshLODInfo.mVertices);
	_mIndexBuffer = std::make_shared<DXIndexBuffer>();
	_mIndexBuffer->Init(meshLODInfo.mIndices);

	_mSlotInfos = meshLODInfo.mSlotInfos;
	_mPrimitive = bulkData->mPrimitive;

	_mSlotsPerMaterial.resize(bulkData->mMaterialCount);

	const std::size_t slotSize = _mSlotInfos.size();
	for (std::size_t i = 0; i < slotSize; ++i)
	{
		_mSlotsPerMaterial[_mSlotInfos[i].mMaterialIndex].push_back(i);
	}
}

void DXStaticMesh::PushData() const
{
	_mVertexBuffer->PushData();
	_mIndexBuffer->PushData();

	DX_DEVICE_CONTEXT->IASetPrimitiveTopology(_mPrimitive);
}