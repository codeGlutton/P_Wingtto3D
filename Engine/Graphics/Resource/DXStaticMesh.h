#pragma once

#include "Graphics/Resource/DXResourceInclude.h"
#include "Graphics/Resource/DXResource.h"

#include "Core/Resource/BulkData.h"
#include "Graphics/DXDefaultVertexData.h"

#include "Graphics/Buffer/DXVertexBuffer.h"
#include "Graphics/Buffer/DXIndexBuffer.h"

struct MeshSlotInfo
{
	GEN_STRUCT_REFLECTION(MeshSlotInfo)

	bool operator==(const MeshSlotInfo&) const = default;

	PROPERTY(mStartIndex)
	uint32 mStartIndex;
	PROPERTY(mIndexCount)
	uint32 mIndexCount;

	PROPERTY(mMaterialIndex)
	uint32 mMaterialIndex;
};

struct StaticMeshLODInfo
{
	GEN_STRUCT_REFLECTION(StaticMeshLODInfo)

	bool operator==(const StaticMeshLODInfo&) const = default;

	PROPERTY(mVertices)
	std::vector<StaticVertexData> mVertices;
	PROPERTY(mIndices)
	std::vector<uint32> mIndices;
	PROPERTY(mSlotInfos)
	std::vector<MeshSlotInfo> mSlotInfos;
};

struct StaticMeshBulkData : public BulkData
{
	GEN_STRUCT_REFLECTION(StaticMeshBulkData)

	bool operator==(const StaticMeshBulkData&) const = default;

	/* 에셋 데이터 */

	PROPERTY(mLODInfos)
	std::vector<StaticMeshLODInfo> mLODInfos;

	PROPERTY(mMaterialCount)
	uint32 mMaterialCount = 1u;

	/* 단순 검사 용 */

	PROPERTY(mAABBData)
	BoundingAABBData mAABBData;
	PROPERTY(mSphereData)
	BoundingSphereData mSphereData;

	/* 랜더 정보 */

	PROPERTY(mPrimitive)
	D3D11_PRIMITIVE_TOPOLOGY mPrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

class DXStaticMesh : public DXSharedResource
{
	friend class ResourceManager;

protected:
	DXStaticMesh();
	~DXStaticMesh();

public:
	uint32 GetMeshSlot() const
	{
		return _mSlot;
	}
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

	void SetMeshSlot(uint32 slot) const
	{
		_mSlot = slot;
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

public:
	std::size_t GetMaterialCount() const
	{
		return _mSlotsPerMaterial.size();
	}
	const std::vector<std::size_t> GetMeshSlots(uint32 materialIndex) const
	{
		return _mSlotsPerMaterial[materialIndex];
	}

public:
	void Init(std::shared_ptr<DXVertexBuffer> vertices, std::shared_ptr<DXIndexBuffer> indices, std::vector<MeshSlotInfo> slotInfos, uint32 materialCount, D3D11_PRIMITIVE_TOPOLOGY primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void Init(std::shared_ptr<StaticMeshBulkData> bulkData, std::size_t targetLODIndex = 0);
	void PushData() const;

private:
	std::shared_ptr<DXVertexBuffer> _mVertexBuffer;
	std::shared_ptr<DXIndexBuffer> _mIndexBuffer;

	D3D11_PRIMITIVE_TOPOLOGY _mPrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	std::vector<MeshSlotInfo> _mSlotInfos;
	std::vector<std::vector<std::size_t>> _mSlotsPerMaterial;

private:
	mutable uint32 _mSlot = 0u;
};

