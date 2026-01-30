#pragma once

/* 랜덤 */

float GetRandomFloat(float min, float max);
Vec2 GetRandomVec2(float min, float max);
Vec3 GetRandomVec3(float min, float max);

/* 단위 */

bool IsNearlyZero(float target);
bool IsNearlyZero(const Vec2& target);
bool IsNearlyZero(const Vec3& target);
bool IsNearlyZero(const Vec4& target);
bool IsNearlyZero(const Quat& target);
bool IsNearlyZero(const Color& target);

/* 기하 */

template<typename T>
struct Interval
{
	T mMin;
	T mMax;
};

struct Manifold
{
    Vec3 mNormal;       // Src To Desc
    float mPenetration; // 침투 깊이
    Vec3 mContactPoint;
};

// 꼭짓점 비교 함수들

Vec3 GetMinAABBConner(const BoundingAABB& aabb);
Vec3 GetMaxAABBConner(const BoundingAABB& aabb);
Interval<Vec3> GetMinMaxAABBConners(const BoundingAABB& aabb);

// 특정 좌표계 크기를 축에 투영

float ProjectExtent2DOnAxis(const Vec2& axis, const Vec2* otherAxes, const Vec2& extents);
float ProjectExtent2DOnAxis(const Vec2& axis, const Vec2& otherAxisX, const Vec2& otherAxisY, float otherHalfSizeX, float otherHalfSizeY);

float ProjectExtent3DOnAxis(const Vec3& axis, const Vec3* otherAxes, const Vec3& extents);

// 주어진 방향에서 Box의 가장 먼 지점 (극점, support point) 반환

Vec2 SupportPointBox2D(const Vec2& center, const Vec2* axes, const Vec2& halfSizes, const Vec2& dir);
Vec2 SupportPointBox2D(const Vec2& center, const Vec2& axisX, const Vec2& axisY, float halfSizeX, float halfSizeY, const Vec2& dir);

Vec3 SupportPointBox3D(const Vec3& center, const Vec3* axes, const Vec3& halfSizes, const Vec3& dir);

// 가장 최근접 점을 찾아주는 함수

Vec3 ClosestPoint(const Line& line, const Vec3& point);
Vec3 ClosestPoint(const Ray& ray, const Vec3& point);
Vec3 ClosestPoint(const BoundingSphere& sphere, const Vec3& point);
Vec3 ClosestPoint(const BoundingAABB& aabb, const Vec3& point);
Vec3 ClosestPoint(const BoundingOBB& obb, const Vec3& point);
Vec3 ClosestPoint(const Plane& plane, const Vec3& point);
Vec3 ClosestPoint(const Triangle& triangle, const Vec3& point);

// 박스를 축에 투영해보는 함수

bool OverlapBox2DOnAxis(const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2* otherAxes, const Vec2& otherHalfSizes);
bool OverlapBox2DOnAxis(const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2* otherAxes, float otherHalfSizeX, float otherHalfSizeY);
bool OverlapBox2DOnAxis(const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2& otherAxisX, const Vec2& otherAxisY, const Vec2& otherHalfSizes);
bool OverlapBox2DOnAxis(const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2& otherAxisX, const Vec2& otherAxisY, float otherHalfSizeX, float otherHalfSizeY);

bool OverlapBox3DOnAxis(const Vec3& centerLine, const Vec3& axis, const Vec3* lhsAxes, const Vec3& lhsHalfSizes, const Vec3* rhsAxes, const Vec3& rhsHalfSizes);

bool OverlapBox2DOnAxis(OUT float& penetration, const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2* otherAxes, const Vec2& otherHalfSizes);
bool OverlapBox2DOnAxis(OUT float& penetration, const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2* otherAxes, float otherHalfSizeX, float otherHalfSizeY);
bool OverlapBox2DOnAxis(OUT float& penetration, const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2& otherAxisX, const Vec2& otherAxisY, const Vec2& otherHalfSizes);
bool OverlapBox2DOnAxis(OUT float& penetration, const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2& otherAxisX, const Vec2& otherAxisY, float otherHalfSizeX, float otherHalfSizeY);

bool OverlapBox3DOnAxis(OUT float& penetration, const Vec3& centerLine, const Vec3& axis, const Vec3* lhsAxes, const Vec3& lhsHalfSizes, const Vec3* rhsAxes, const Vec3& rhsHalfSizes);

// 충돌 검사 및 연관 결과 데이터를 반환하는 함수 (일반 intersect보다 비용 비쌈)

bool IntersectsWithMTV(const BoundingSphere& lhsSphere, const BoundingSphere& rhsSphere, OUT Manifold& result, const Vec3& preLhsNormal = Vec3::Zero);
bool IntersectsWithMTV(const BoundingAABB& lhsAABB, const BoundingAABB& rhsAABB, OUT Manifold& result, const Vec3& preLhsNormal = Vec3::Zero);
bool IntersectsWithMTV(const BoundingOBB& lhsOBB, const BoundingOBB& rhsOBB, OUT Manifold& result, const Vec3& preLhsNormal = Vec3::Zero);

bool IntersectsWithMTV(const BoundingSphere& lhsSphere, const BoundingAABB& rhsAABB, OUT Manifold& result, const Vec3& preLhsNormal = Vec3::Zero);
bool IntersectsWithMTV(const BoundingAABB& lhsAABB, const BoundingSphere& rhsSphere, OUT Manifold& result, const Vec3& preLhsNormal = Vec3::Zero);

bool IntersectsWithMTV(const BoundingSphere& lhsSphere, const BoundingOBB& rhsOBB, OUT Manifold& result, const Vec3& preLhsNormal = Vec3::Zero);
bool IntersectsWithMTV(const BoundingOBB& lhsOBB, const BoundingSphere& rhsSphere, OUT Manifold& result, const Vec3& preLhsNormal = Vec3::Zero);

bool IntersectsWithMTV(const BoundingAABB& lhsAABB, const BoundingOBB& rhsOBB, OUT Manifold& result, const Vec3& preLhsNormal = Vec3::Zero);
bool IntersectsWithMTV(const BoundingOBB& lhsOBB, const BoundingAABB& rhsAABB, OUT Manifold& result, const Vec3& preLhsNormal = Vec3::Zero);

