#pragma once

#include <vector>

#include "Types.h"

#include "Reflection/Reflection.h"
#include "Reflection/Property.h"
#include "Reflection/Method.h"

namespace DXResourceFlag
{
	enum Type : uint8
	{
		None = 0,
		Updatable = 1 << 0,
		HasOutput = 1 << 1,
	};

	GEN_ENUM_OPERATORS(Type)
}

namespace DXSharedResourceType
{
	enum Type : uint8
	{
		None = 0xFF,

		/* 패키징 파일 저장 리소스 */

		Texture = 0,
		Material,
		Mesh,

		/* 패키징 파일 비저장 리소스 */

		Shader,
		SamplerState,
		BlendState,
		RasterizerState,

		Count,
	};

	GEN_ENUM_OPERATORS(Type)
}

namespace DXResourceUsageBit
{
	enum Type : uint8
	{
		None = 0xFF,

		VertexBit = 0,
		PixelBit,
		DomainBit,
		HullBit,
		GeometryBit,
		ComputeBit,

		Count,
	};

	GEN_ENUM_OPERATORS(Type)
}

namespace DXResourceUsageFlag
{
	enum Type : uint8
	{
		None = 0,
		Vertex = 1 << 0,
		Pixel = 1 << 1,
		Domain = 1 << 2,
		Hull = 1 << 3,
		Geometry = 1 << 4,
		Compute = 1 << 5,

		GrapicMinimum = Vertex | Pixel,
		GrapicMaximum = Vertex | Pixel | Domain | Hull | Geometry,
		All = Vertex | Pixel | Domain | Hull | Geometry | Compute,
	};

	GEN_ENUM_OPERATORS(Type)
}

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

