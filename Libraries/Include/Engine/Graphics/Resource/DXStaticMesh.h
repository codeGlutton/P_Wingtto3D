#pragma once

#include "Graphics/Resource/DXResourceInclude.h"

#include "Core/Resource/BulkData.h"
#include "Graphics/DXDefaultVertexData.h"

#include "Graphics/Buffer/DXVertexBuffer.h"
#include "Graphics/Buffer/DXIndexBuffer.h"

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

class DXStaticMesh
{
public:
	void Init(std::shared_ptr<StaticMeshBulkData> bulkData);

private:
	std::shared_ptr<DXVertexBuffer> _mVertexBuffer;
	std::shared_ptr<DXIndexBuffer> _mIndexBuffer;

	D3D11_PRIMITIVE_TOPOLOGY _mPrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	std::vector<MeshSlotInfo> _mSlotInfos;
	std::vector<std::vector<std::size_t>> _mSlotsPerMaterial;
};

