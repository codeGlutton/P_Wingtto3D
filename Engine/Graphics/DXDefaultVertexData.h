#pragma once

struct DXInputElementList;

/**
 * 정점 정보 및 텍스처 uv 소유한 VS Input
 */
struct UIVertexData
{
	GEN_MINIMUM_STRUCT_REFLECTION(UIVertexData)

	bool operator==(const UIVertexData&) const = default;

public:
	static const DXInputElementList& MakeLayout();

public:
	PROPERTY(mPosition)
	Vec3 mPosition = { 0.f, 0.f, 0.f };
	PROPERTY(mUV)
	Vec2 mUV = { 0.f, 0.f };
	PROPERTY(mTint)
	Color mTint = { 0.f, 0.f, 0.f, 0.f };
};

/**
 * 정점 정보 및 텍스처 uv, 노말, 탄젠트를 소유한 VS Input
 */
struct StaticVertexData
{
	GEN_MINIMUM_STRUCT_REFLECTION(StaticVertexData)

	bool operator==(const StaticVertexData&) const = default;

public:
	static const DXInputElementList& MakeLayout();

public:
	PROPERTY(mPosition)
	Vec3 mPosition = { 0.f, 0.f, 0.f };
	PROPERTY(mUV)
	Vec2 mUV = { 0.f, 0.f };
	PROPERTY(mNormal)
	Vec3 mNormal = { 0.f, 0.f, 0.f };
	PROPERTY(mTangent)
	Vec3 mTangent = { 0.f, 0.f, 0.f };
};

/**
 * 정점 정보 및 텍스처 uv, 노말, 탄젠트, 스키닝 정보를 소유한 VS Input
 */
struct SkeletalVertexData
{
	GEN_MINIMUM_STRUCT_REFLECTION(SkeletalVertexData)

	bool operator==(const SkeletalVertexData&) const = default;

public:
	static const DXInputElementList& MakeLayout();

public:
	PROPERTY(mPosition)
	Vec3 mPosition = { 0.f, 0.f, 0.f };
	PROPERTY(mUV)
	Vec2 mUV = { 0.f, 0.f };
	PROPERTY(mNormal)
	Vec3 mNormal = { 0.f, 0.f, 0.f };
	PROPERTY(mTangent)
	Vec3 mTangent = { 0.f, 0.f, 0.f };

	/* 스키닝 정보 (최대 4개로 제한 -> GPU에 전달해야하므로) */

	// 연관된 본 정보
	PROPERTY(mBlendIndices)
	Vec4 mBlendIndices = { 0.f, 0.f, 0.f, 0.f };
	// 본의 영향 받는 비율
	PROPERTY(mBlendWeights)
	Vec4 mBlendWeights = { 0.f, 0.f, 0.f, 0.f };
};

