#include "pch.h"
#include "MathUtils.h"

/* 랜덤 */

float GetRandomFloat(float min, float max)
{
    float r = ((float)rand()) / (float)RAND_MAX;
    float diff = max - min;
    float val = r * diff;

    return min + val;
}

Vec2 GetRandomVec2(float min, float max)
{
    Vec2 r;
    r.x = GetRandomFloat(min, max);
    r.y = GetRandomFloat(min, max);

    return r;
}

Vec3 GetRandomVec3(float min, float max)
{
    Vec3 r;
    r.x = GetRandomFloat(min, max);
    r.y = GetRandomFloat(min, max);
    r.z = GetRandomFloat(min, max);

    return r;
}


/* 단위 */

bool IsNearlyZero(float target)
{
    return std::fabs(target) < std::numeric_limits<float>::epsilon();
}

bool IsNearlyZero(const Vec2& target)
{
    return IsNearlyZero(target.x) && IsNearlyZero(target.y);
}

bool IsNearlyZero(const Vec3& target)
{
    return IsNearlyZero(target.x) && IsNearlyZero(target.y) && IsNearlyZero(target.z);
}

bool IsNearlyZero(const Vec4& target)
{
    return IsNearlyZero(target.x) && IsNearlyZero(target.y) && IsNearlyZero(target.z) && IsNearlyZero(target.w);
}

bool IsNearlyZero(const Quat& target)
{
    return IsNearlyZero(target.x) && IsNearlyZero(target.y) && IsNearlyZero(target.z) && IsNearlyZero(target.w);
}

bool IsNearlyZero(const Color& target)
{
    return IsNearlyZero(target.x) && IsNearlyZero(target.y) && IsNearlyZero(target.z) && IsNearlyZero(target.w);
}

float ConvertRotationToRadian(float rotation)
{
    return rotation / 180.f * DirectX::XM_PI;
}

Vec3 ConvertRotationToRadian(Vec3 rotations)
{
    return rotations / 180.f * DirectX::XM_PI;
}

float ConvertRadianToRotation(float radian)
{
    return radian / DirectX::XM_PI * 180.f;
}

Vec3 ConvertRadianToRotation(Vec3 radians)
{
    return radians / DirectX::XM_PI * 180.f;
}


/* 기하 */

Vec3 GetMinAABBConner(const BoundingAABB& aabb)
{
    return Vec3(aabb.Center) - aabb.Extents;
}

Vec3 GetMaxAABBConner(const BoundingAABB& aabb)
{
    return Vec3(aabb.Center) + aabb.Extents;
}

Interval<Vec3> GetMinMaxAABBConners(const BoundingAABB& aabb)
{
    return Interval<Vec3>{ Vec3(aabb.Center) - aabb.Extents, Vec3(aabb.Center) + aabb.Extents };
}


float ProjectExtent2DOnAxis(const Vec2& axis, const Vec2* otherAxes, const Vec2& extents)
{
    return ProjectExtent2DOnAxis(axis, otherAxes[0], otherAxes[1], extents.x, extents.y);
}

float ProjectExtent2DOnAxis(const Vec2& axis, const Vec2& otherAxisX, const Vec2& otherAxisY, float otherHalfSizeX, float otherHalfSizeY)
{
    return std::abs(axis.Dot(otherAxisX)) * otherHalfSizeX +
        std::abs(axis.Dot(otherAxisY)) * otherHalfSizeY;
}

float ProjectExtent3DOnAxis(const Vec3& axis, const Vec3* otherAxes, const Vec3& extents)
{
    return std::abs(axis.Dot(otherAxes[0])) * extents.x +
        std::abs(axis.Dot(otherAxes[1])) * extents.y +
        std::abs(axis.Dot(otherAxes[2])) * extents.z;
}

Vec2 SupportPointBox2D(const Vec2& center, const Vec2* axes, const Vec2& halfSizes, const Vec2& dir)
{
    return SupportPointBox2D(center, axes[0], axes[1], halfSizes.x, halfSizes.y, dir);
}

Vec2 SupportPointBox2D(const Vec2& center, const Vec2& axisX, const Vec2& axisY, float halfSizeX, float halfSizeY, const Vec2& dir)
{
    float projectedDirX = axisX.Dot(dir);
    float projectedDirY = axisY.Dot(dir);

    float projectedDirXNormal = -1.f;
    float projectedDirYNormal = -1.f;
    if (projectedDirX >= 0.f)
    {
        projectedDirXNormal = 1.f;
    }
    if (projectedDirY >= 0.f)
    {
        projectedDirYNormal = 1.f;
    }

    return center +
        axisX * (halfSizeX * projectedDirXNormal) +
        axisY * (halfSizeY * projectedDirYNormal);
}

Vec3 SupportPointBox3D(const Vec3& center, const Vec3* axes, const Vec3& halfSizes, const Vec3& dir)
{
    float projectedDirX = axes[0].Dot(dir);
    float projectedDirY = axes[1].Dot(dir);
    float projectedDirZ = axes[2].Dot(dir);

    float projectedDirXNormal = -1.f;
    float projectedDirYNormal = -1.f;
    float projectedDirZNormal = -1.f;
    if (projectedDirX >= 0.f)
    {
        projectedDirXNormal = 1.f;
    }
    if (projectedDirY >= 0.f)
    {
        projectedDirYNormal = 1.f;
    }
    if (projectedDirZ >= 0.f)
    {
        projectedDirZNormal = 1.f;
    }

    return center +
        axes[0] * (halfSizes.x * projectedDirXNormal) +
        axes[1] * (halfSizes.y * projectedDirYNormal) +
        axes[2] * (halfSizes.z * projectedDirZNormal);
}

Vec3 ClosestPoint(const Line& line, const Vec3& point)
{
    Vec3 straight = line.end - line.start;
    float rate = (point - line.start).Dot(straight) / straight.Dot(straight);

    rate = std::max<float>(rate, 0.f);
    rate = std::min<float>(rate, 1.f);

    return line.start + straight * rate;
}

Vec3 ClosestPoint(const Ray& ray, const Vec3& point)
{
    float rate = (point - ray.position).Dot(ray.direction);
    rate = fmaxf(rate, 0.f);

    return ray.position + ray.direction * rate;
}

Vec3 ClosestPoint(const BoundingSphere& sphere, const Vec3& point)
{
    Vec3 sphereToPointDir = point - sphere.Center;
    sphereToPointDir.Normalize();

    return sphere.Center + sphereToPointDir * sphere.Radius;
}

Vec3 ClosestPoint(const BoundingAABB& aabb, const Vec3& point)
{
    Vec3 result;

    result.x = std::clamp(point.x,
        aabb.Center.x - aabb.Extents.x,
        aabb.Center.x + aabb.Extents.x);

    result.y = std::clamp(point.y,
        aabb.Center.y - aabb.Extents.y,
        aabb.Center.y + aabb.Extents.y);

    result.z = std::clamp(point.z,
        aabb.Center.z - aabb.Extents.z,
        aabb.Center.z + aabb.Extents.z);

    return result;
}

Vec3 ClosestPoint(const BoundingOBB& obb, const Vec3& point)
{
    Vec3 dir = point - obb.Center;
    Matrix mat = Matrix::CreateFromQuaternion(Quat(obb.Orientation));
    Vec3 axis[3] = {
        mat.Right(),
        mat.Up(),
        mat.Backward()
    };
    float size[3] = {
        obb.Extents.x,
        obb.Extents.y,
        obb.Extents.z
    };

    Vec3 result = obb.Center;
    for (int32 i = 0; i < 3; ++i)
    {
        float dist = dir.Dot(axis[i]);
        if (dist > size[i])
        {
            dist = size[i];
        }
        if (dist < -size[i])
        {
            dist = -size[i];
        }

        result += axis[i] * dist;
    }
    return result;
}

Vec3 ClosestPoint(const Plane& plane, const Vec3& point)
{
    float normalAxisPos = point.Dot(plane.Normal());
    float normalAxisDiff = normalAxisPos - plane.D();
    return point - plane.Normal() * normalAxisDiff;
}

Vec3 ClosestPoint(const Triangle& triangle, const Vec3& point)
{
    Plane plane = Plane::CreateFromTriangle(triangle);
    Vec3 closestPlanePoint = ClosestPoint(plane, point);

    // 삼각형을 포함한 평면으로의 최근점 정점이 삼각형 내부에 위치한 경우
    // 삼각형으로의 최근점 정점 결과와 동일하다.
    if (triangle.Contains(closestPlanePoint) == true)
    {
        return closestPlanePoint;
    }

    // 그 외의 경우
    // 2D 평면에서 삼각형의 각 선분의 최근점 정점을 비교해보아야 한다
    std::array<Line, 3> lines = {
        Line{triangle.a, triangle.b},
        Line{triangle.b, triangle.c},
        Line{triangle.c, triangle.a},
    };

    float minDistSq = FLT_MAX;
    Vec3 result;
    for (auto& line : lines)
    {
        Vec3 closestLinePoint = ClosestPoint(line, closestPlanePoint);
        float distSq = (point - closestLinePoint).LengthSquared();
        if (minDistSq > distSq)
        {
            minDistSq = distSq;
            result = closestLinePoint;
        }
    }
    return result;
}


bool OverlapBox2DOnAxis(const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2* otherAxes, const Vec2& otherHalfSizes)
{
    return OverlapBox2DOnAxis(centerLine, axis, halfSize, otherAxes[0], otherAxes[1], otherHalfSizes);
}

bool OverlapBox2DOnAxis(const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2* otherAxes, float otherHalfSizeX, float otherHalfSizeY)
{
    return OverlapBox2DOnAxis(centerLine, axis, halfSize, otherAxes[0], otherAxes[1], otherHalfSizeX, otherHalfSizeY);
}

bool OverlapBox2DOnAxis(const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2& otherAxisX, const Vec2& otherAxisY, const Vec2& otherHalfSizes)
{
    return OverlapBox2DOnAxis(centerLine, axis, halfSize, otherAxisX, otherAxisY, otherHalfSizes.x, otherHalfSizes.y);
}

bool OverlapBox2DOnAxis(const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2& otherAxisX, const Vec2& otherAxisY, float otherHalfSizeX, float otherHalfSizeY)
{
    float projectedCenterLine = std::abs(centerLine.Dot(axis));

    float projectedOtherAxis = ProjectExtent2DOnAxis(axis, otherAxisX, otherAxisY, otherHalfSizeX, otherHalfSizeY);

    return (halfSize + projectedOtherAxis > projectedCenterLine);
}

bool OverlapBox3DOnAxis(const Vec3& centerLine, const Vec3& axis, const Vec3* lhsAxes, const Vec3& lhsHalfSizes, const Vec3* rhsAxes, const Vec3& rhsHalfSizes)
{
    float projectedCenterLine = std::abs(centerLine.Dot(axis));

    float projectedLhsAxis = ProjectExtent3DOnAxis(axis, lhsAxes, lhsHalfSizes);
    float projectedRhsAxis = ProjectExtent3DOnAxis(axis, rhsAxes, rhsHalfSizes);

    return (projectedLhsAxis + projectedRhsAxis > projectedCenterLine);
}


bool OverlapBox2DOnAxis(OUT float& penetration, const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2* otherAxes, const Vec2& otherHalfSizes)
{
    return OverlapBox2DOnAxis(penetration, centerLine, axis, halfSize, otherAxes[0], otherAxes[1], otherHalfSizes);
}

bool OverlapBox2DOnAxis(OUT float& penetration, const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2* otherAxes, float otherHalfSizeX, float otherHalfSizeY)
{
    return OverlapBox2DOnAxis(penetration, centerLine, axis, halfSize, otherAxes[0], otherAxes[1], otherHalfSizeX, otherHalfSizeY);
}

bool OverlapBox2DOnAxis(OUT float& penetration, const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2& otherAxisX, const Vec2& otherAxisY, const Vec2& otherHalfSizes)
{
    return OverlapBox2DOnAxis(penetration, centerLine, axis, halfSize, otherAxisX, otherAxisY, otherHalfSizes.x, otherHalfSizes.y);
}

bool OverlapBox2DOnAxis(OUT float& penetration, const Vec2& centerLine, const Vec2& axis, float halfSize, const Vec2& otherAxisX, const Vec2& otherAxisY, float otherHalfSizeX, float otherHalfSizeY)
{
    float projectedCenterLine = std::abs(centerLine.Dot(axis));

    float projectedOtherAxis = ProjectExtent2DOnAxis(axis, otherAxisX, otherAxisY, otherHalfSizeX, otherHalfSizeY);

    penetration = halfSize + projectedOtherAxis - projectedCenterLine;
    if (penetration <= 0.f)
    {
        penetration = 0.f;
        return false;
    }
    return true;
}

bool OverlapBox3DOnAxis(OUT float& penetration, const Vec3& centerLine, const Vec3& axis, const Vec3* lhsAxes, const Vec3& lhsHalfSizes, const Vec3* rhsAxes, const Vec3& rhsHalfSizes)
{
    float projectedCenterLine = std::abs(centerLine.Dot(axis));

    float projectedLhsAxis = ProjectExtent3DOnAxis(axis, lhsAxes, lhsHalfSizes);
    float projectedRhsAxis = ProjectExtent3DOnAxis(axis, rhsAxes, rhsHalfSizes);

    penetration = projectedLhsAxis + projectedRhsAxis - projectedCenterLine;
    if (penetration <= 0.f)
    {
        penetration = 0.f;
        return false;
    }
    return true;
}


bool IntersectsWithMTV(const BoundingSphere& lhsSphere, const BoundingSphere& rhsSphere, OUT Manifold& result, const Vec3& preLhsNormal)
{
    float radiusSum = lhsSphere.Radius + rhsSphere.Radius;
    Vec3 centerLine = Vec3(rhsSphere.Center) - lhsSphere.Center; // lhs to rhs
    float centerDist = centerLine.Length();

    if (centerDist > radiusSum)
    {
        return false;
    }

    if (IsNearlyZero(centerDist) == true)
    {
        result.mNormal = -preLhsNormal;
        result.mPenetration = radiusSum;
        result.mContactPoint = lhsSphere.Center;
        return true;
    }

    centerLine.Normalize();
    result.mNormal = centerLine;
    result.mPenetration = radiusSum - centerDist;
    result.mContactPoint = lhsSphere.Center + result.mNormal * (lhsSphere.Radius - result.mPenetration * 0.5f);
    return true;
}

bool IntersectsWithMTV(const BoundingAABB& lhsAABB, const BoundingAABB& rhsAABB, OUT Manifold& result, const Vec3& preLhsNormal)
{
    Vec3 centerLine = Vec3(lhsAABB.Center) - rhsAABB.Center; // src to dest
    result.mPenetration = FLT_MAX;

    if (IsNearlyZero(centerLine) == true)
    {
        float maxDistOnPreNormal = -1.f;

        float px = (lhsAABB.Extents.x + rhsAABB.Extents.x) - fabs(centerLine.x);
        if (result.mPenetration > px)
        {
            maxDistOnPreNormal = fabs(preLhsNormal.Dot(Vec3::UnitX));
            result.mPenetration = px;
            result.mNormal = Vec3::UnitX;
        }

        float py = (lhsAABB.Extents.y + rhsAABB.Extents.y) - fabs(centerLine.y);
        if (result.mPenetration > py)
        {
            maxDistOnPreNormal = fabs(preLhsNormal.Dot(Vec3::UnitY));
            result.mPenetration = py;
            result.mNormal = Vec3::UnitY;
        }
        else if (IsNearlyZero(result.mPenetration - py))
        {
            float distOnPreNormal = fabs(preLhsNormal.Dot(Vec3::UnitY));
            if (distOnPreNormal > maxDistOnPreNormal)
            {
                maxDistOnPreNormal = distOnPreNormal;
                result.mPenetration = py;
                result.mNormal = Vec3::UnitY;
            }
        }

        float pz = (lhsAABB.Extents.z + rhsAABB.Extents.z) - fabs(centerLine.z);
        if (result.mPenetration > pz)
        {
            maxDistOnPreNormal = fabs(preLhsNormal.Dot(Vec3::UnitZ));
            result.mPenetration = pz;
            result.mNormal = Vec3::UnitZ;
        }
        else if (IsNearlyZero(result.mPenetration - pz))
        {
            float distOnPreNormal = fabs(preLhsNormal.Dot(Vec3::UnitZ));
            if (distOnPreNormal > maxDistOnPreNormal)
            {
                maxDistOnPreNormal = distOnPreNormal;
                result.mPenetration = pz;
                result.mNormal = Vec3::UnitZ;
            }
        }
        if (preLhsNormal.Dot(result.mNormal) < 0.f)
        {
            result.mNormal *= 1.f;
        }
        result.mContactPoint = lhsAABB.Center;
        return true;
    }
    else
    {
        float px = (lhsAABB.Extents.x + rhsAABB.Extents.x) - fabs(centerLine.x);
        if (px <= 0.f)
        {
            return false;
        }
        if (result.mPenetration > px)
        {
            result.mPenetration = px;
            result.mNormal = Vec3::UnitX;
        }

        float py = (lhsAABB.Extents.y + rhsAABB.Extents.y) - fabs(centerLine.y);
        if (py <= 0.f)
        {
            return false;
        }
        if (result.mPenetration > py)
        {
            result.mPenetration = py;
            result.mNormal = Vec3::UnitY;
        }

        float pz = (lhsAABB.Extents.z + rhsAABB.Extents.z) - fabs(centerLine.z);
        if (pz <= 0.f)
        {
            return false;
        }
        if (result.mPenetration > pz)
        {
            result.mPenetration = pz;
            result.mNormal = Vec3::UnitZ;
        }
    }

    if (centerLine.Dot(result.mNormal) < 0.f)
    {
        result.mNormal *= -1.f;
    }

    result.mContactPoint = (Vec3(lhsAABB.Center) + rhsAABB.Center) * 0.5f;
    return true;
}

bool IntersectsWithMTV(const BoundingOBB& lhsOBB, const BoundingOBB& rhsOBB, OUT Manifold& result, const Vec3& preLhsNormal)
{
    Vec3 centerLine = Vec3(rhsOBB.Center) - lhsOBB.Center; // src to dest
    Matrix lhsMat = Matrix::CreateFromQuaternion(Quat(lhsOBB.Orientation));
    Matrix rhsMat = Matrix::CreateFromQuaternion(Quat(rhsOBB.Orientation));
    Vec3 axes[15] =
    {
        lhsMat.Right(),
        lhsMat.Up(),
        lhsMat.Backward(),
        rhsMat.Right(),
        rhsMat.Up(),
        rhsMat.Backward()
    };

    for (int32 i = 0; i < 3; ++i)
    {
        axes[6 + i * 3 + 0] = axes[3 + i].Cross(axes[0]);
        axes[6 + i * 3 + 1] = axes[3 + i].Cross(axes[1]);
        axes[6 + i * 3 + 2] = axes[3 + i].Cross(axes[2]);
    }

    result.mPenetration = FLT_MAX;
    if (IsNearlyZero(centerLine) == true)
    {
        float maxDistOnPreNormal = -1.f;
        for (int32 i = 0; i < 15; ++i)
        {
            if (IsNearlyZero(axes[i]) == true)
            {
                continue;
            }

            float penetration;
            OverlapBox3DOnAxis(penetration, centerLine, axes[i], &axes[0], lhsOBB.Extents, &axes[3], rhsOBB.Extents);

            if (result.mPenetration > penetration)
            {
                maxDistOnPreNormal = fabs(preLhsNormal.Dot(axes[i]));
                result.mPenetration = penetration;
                result.mNormal = axes[i];
            }
            else if (IsNearlyZero(result.mPenetration - penetration))
            {
                float distOnPreNormal = fabs(preLhsNormal.Dot(axes[i]));
                if (distOnPreNormal > maxDistOnPreNormal)
                {
                    maxDistOnPreNormal = distOnPreNormal;
                    result.mPenetration = penetration;
                    result.mNormal = axes[i];
                }
            }
        }
        if (preLhsNormal.Dot(result.mNormal) < 0.f)
        {
            result.mNormal *= 1.f;
        }
        result.mContactPoint = lhsOBB.Center;
        return true;
    }
    else
    {
        for (int32 i = 0; i < 15; ++i)
        {
            if (IsNearlyZero(axes[i]) == true)
            {
                continue;
            }

            float penetration;
            if (OverlapBox3DOnAxis(penetration, centerLine, axes[i], &axes[0], lhsOBB.Extents, &axes[3], rhsOBB.Extents) == false)
            {
                return false;
            }
            if (result.mPenetration > penetration)
            {
                result.mPenetration = penetration;
                result.mNormal = axes[i];
            }
        }
    }

    if (centerLine.Dot(result.mNormal) < 0.f)
    {
        result.mNormal *= -1.f;
    }

    Vec3 lhsSupportPoint = SupportPointBox3D(lhsOBB.Center, &axes[0], lhsOBB.Extents, result.mNormal);
    Vec3 rhsSupportPoint = SupportPointBox3D(rhsOBB.Center, &axes[3], rhsOBB.Extents, result.mNormal * -1.f);
    result.mContactPoint = (lhsSupportPoint + rhsSupportPoint) * 0.5f;
    return true;
}

bool IntersectsWithMTV(const BoundingSphere& lhsSphere, const BoundingAABB& rhsAABB, OUT Manifold& result, const Vec3& preLhsNormal)
{
    bool isIntersected = IntersectsWithMTV(rhsAABB, lhsSphere, result, preLhsNormal);
    result.mNormal *= -1.f;
    return isIntersected;
}

bool IntersectsWithMTV(const BoundingAABB& lhsAABB, const BoundingSphere& rhsSphere, OUT Manifold& result, const Vec3& preLhsNormal)
{
    Vec3 closestPos = ClosestPoint(lhsAABB, rhsSphere.Center);
    Vec3 closestPosToCenter = rhsSphere.Center - closestPos;

    float dist = closestPosToCenter.Length();
    if (dist > rhsSphere.Radius)
    {
        return false;
    }

    if (IsNearlyZero(dist) == true)
    {
        Vec3 centerLine = Vec3(rhsSphere.Center) - lhsAABB.Center;
        float dx = (lhsAABB.Extents.x) - fabsf(centerLine.x);
        float dy = (lhsAABB.Extents.y) - fabsf(centerLine.y);
        float dz = (lhsAABB.Extents.z) - fabsf(centerLine.z);

        if (dx <= dy && dx <= dz)
        {
            result.mNormal = (centerLine.x >= 0.f) ? +Vec3::UnitX : -Vec3::UnitX;
            result.mPenetration = rhsSphere.Radius + dx;
        }
        else if (dy <= dx && dy <= dz)
        {
            result.mNormal = (centerLine.y >= 0.f) ? +Vec3::UnitY : -Vec3::UnitY;
            result.mPenetration = rhsSphere.Radius + dy;
        }
        else
        {
            result.mNormal = (centerLine.z >= 0.f) ? +Vec3::UnitZ : -Vec3::UnitZ;
            result.mPenetration = rhsSphere.Radius + dz;
        }
        result.mContactPoint = rhsSphere.Center;
        return true;
    }

    result.mNormal = (rhsSphere.Center - closestPos) / dist;
    result.mPenetration = rhsSphere.Radius - dist;
    result.mContactPoint = closestPos;
    return result.mPenetration >= 0.f;
}

bool IntersectsWithMTV(const BoundingSphere& lhsSphere, const BoundingOBB& rhsOBB, OUT Manifold& result, const Vec3& preLhsNormal)
{
    bool isIntersected = IntersectsWithMTV(rhsOBB, lhsSphere, result, preLhsNormal);
    result.mNormal *= -1.f;
    return isIntersected;
}

bool IntersectsWithMTV(const BoundingOBB& lhsOBB, const BoundingSphere& rhsSphere, OUT Manifold& result, const Vec3& preLhsNormal)
{
    Vec3 centerLine = Vec3(rhsSphere.Center) - lhsOBB.Center;
    Matrix mat = Matrix::CreateFromQuaternion(lhsOBB.Orientation);

    float lx = centerLine.Dot(mat.Right());
    float ly = centerLine.Dot(mat.Up());
    float lz = centerLine.Dot(mat.Backward());

    float clx = std::clamp(lx, -lhsOBB.Extents.x, lhsOBB.Extents.x);
    float cly = std::clamp(ly, -lhsOBB.Extents.y, lhsOBB.Extents.y);
    float clz = std::clamp(lz, -lhsOBB.Extents.z, lhsOBB.Extents.z);

    Vec3 closestPos = lhsOBB.Center + mat.Right() * clx + mat.Up() * cly + mat.Backward() * clz;
    Vec3 closestPosToCenter = rhsSphere.Center - closestPos;

    float dist = closestPosToCenter.Length();
    if (dist > rhsSphere.Radius)
    {
        return false;
    }

    if (IsNearlyZero(dist) == true)
    {
        float dx = lhsOBB.Extents.x - fabsf(lx);
        float dy = lhsOBB.Extents.y - fabsf(ly);
        float dz = lhsOBB.Extents.z - fabsf(lz);

        if (dx <= dy && dx <= dz)
        {
            float sign = (lx >= 0.f) ? 1.f : -1.f;
            result.mNormal = mat.Right() * sign;
            result.mPenetration = rhsSphere.Radius + dx;
        }
        else if (dy <= dx && dy <= dz)
        {
            float sign = (ly >= 0.f) ? 1.f : -1.f;
            result.mNormal = mat.Up() * sign;
            result.mPenetration = rhsSphere.Radius + dy;
        }
        else
        {
            float sign = (lz >= 0.f) ? 1.f : -1.f;
            result.mNormal = mat.Backward() * sign;
            result.mPenetration = rhsSphere.Radius + dz;
        }
        result.mNormal.Normalize();
        result.mContactPoint = rhsSphere.Center;
        return true;
    }

    result.mNormal = (rhsSphere.Center - closestPos) / dist;
    result.mPenetration = rhsSphere.Radius - dist;
    result.mContactPoint = closestPos;
    return result.mPenetration >= 0.f;
}

bool IntersectsWithMTV(const BoundingAABB& lhsAABB, const BoundingOBB& rhsOBB, OUT Manifold& result, const Vec3& preLhsNormal)
{
    Vec3 centerLine = Vec3(rhsOBB.Center) - lhsAABB.Center; // src to dest
    Matrix rhsMat = Matrix::CreateFromQuaternion(Quat(rhsOBB.Orientation));
    Vec3 axes[15] =
    {
        Vec3::UnitX,
        Vec3::UnitY,
        Vec3::UnitZ,
        rhsMat.Right(),
        rhsMat.Up(),
        rhsMat.Backward()
    };

    for (int32 i = 0; i < 3; ++i)
    {
        axes[6 + i * 3 + 0] = axes[3 + i].Cross(axes[0]);
        axes[6 + i * 3 + 1] = axes[3 + i].Cross(axes[1]);
        axes[6 + i * 3 + 2] = axes[3 + i].Cross(axes[2]);
    }

    result.mPenetration = FLT_MAX;
    if (IsNearlyZero(centerLine) == true)
    {
        float minDistOnPreNormal = FLT_MAX;
        for (int32 i = 0; i < 15; ++i)
        {
            if (IsNearlyZero(axes[i]) == true)
            {
                continue;
            }

            float penetration;
            OverlapBox3DOnAxis(penetration, centerLine, axes[i], &axes[0], lhsAABB.Extents, &axes[3], rhsOBB.Extents);

            if (result.mPenetration > penetration)
            {
                minDistOnPreNormal = preLhsNormal.Dot(axes[i]);
                result.mPenetration = penetration;
                result.mNormal = axes[i];
            }
            else if (IsNearlyZero(result.mPenetration - penetration))
            {
                float distOnPreNormal = preLhsNormal.Dot(axes[i]);
                if (distOnPreNormal < minDistOnPreNormal)
                {
                    minDistOnPreNormal = distOnPreNormal;
                    result.mPenetration = penetration;
                    result.mNormal = axes[i];
                }
            }
        }
        result.mContactPoint = lhsAABB.Center;
        return true;
    }
    else
    {
        for (int32 i = 0; i < 15; ++i)
        {
            if (IsNearlyZero(axes[i]) == true)
            {
                continue;
            }

            float penetration;
            if (OverlapBox3DOnAxis(penetration, centerLine, axes[i], &axes[0], lhsAABB.Extents, &axes[3], rhsOBB.Extents) == false)
            {
                return false;
            }
            if (result.mPenetration > penetration)
            {
                result.mPenetration = penetration;
                result.mNormal = axes[i];
            }
        }
    }

    if (centerLine.Dot(result.mNormal) < 0.f)
    {
        result.mNormal *= -1.f;
    }

    Vec3 lhsSupportPoint = SupportPointBox3D(lhsAABB.Center, &axes[0], lhsAABB.Extents, result.mNormal);
    Vec3 rhsSupportPoint = SupportPointBox3D(rhsOBB.Center, &axes[3], rhsOBB.Extents, result.mNormal * -1.f);
    result.mContactPoint = (lhsSupportPoint + rhsSupportPoint) * 0.5f;
    return true;
}

bool IntersectsWithMTV(const BoundingOBB& lhsOBB, const BoundingAABB& rhsAABB, OUT Manifold& result, const Vec3& preLhsNormal)
{
    bool isIntersected = IntersectsWithMTV(rhsAABB, lhsOBB, result, preLhsNormal);
    result.mNormal *= -1.f;
    return isIntersected;
}


