#pragma once

#include <vector>

#include "Types.h"

#include "Reflection/Reflection.h"
#include "Reflection/Property.h"
#include "Reflection/Method.h"

struct BoundingAABBData
{
	GEN_STRUCT_REFLECTION(BoundingAABBData)

	bool operator==(const BoundingAABBData&) const = default;

public:
	BoundingAABB MakeAABB()
	{
		return BoundingAABB(mCenter, mExtents);
	}

public:
	PROPERTY(mCenter)
	Vec3 mCenter;
	PROPERTY(mExtents)
	Vec3 mExtents;
};

struct BoundingSphereData
{
	GEN_STRUCT_REFLECTION(BoundingSphereData)

	bool operator==(const BoundingSphereData&) const = default;

public:
	BoundingSphere MakeSphere()
	{
		return BoundingSphere(mCenter, mRadius);
	}

public:
	PROPERTY(mCenter)
	Vec3 mCenter;
	PROPERTY(mRadius)
	float mRadius;
};

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
