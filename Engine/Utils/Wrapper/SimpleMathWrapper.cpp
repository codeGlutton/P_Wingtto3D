#include "pch.h"
#include "SimpleMathWrapper.h"

#include "Utils/MathUtils.h"

const Vec2 Vec2::Zero = { 0.f, 0.f };
const Vec2 Vec2::One = { 1.f, 1.f };
const Vec2 Vec2::UnitX = { 1.f, 0.f };
const Vec2 Vec2::UnitY = { 0.f, 1.f };

const Vec3 Vec3::Zero = { 0.f, 0.f, 0.f };
const Vec3 Vec3::One = { 1.f, 1.f, 1.f };
const Vec3 Vec3::UnitX = { 1.f, 0.f, 0.f };
const Vec3 Vec3::UnitY = { 0.f, 1.f, 0.f };
const Vec3 Vec3::UnitZ = { 0.f, 0.f, 1.f };
const Vec3 Vec3::Up = { 0.f, 1.f, 0.f };
const Vec3 Vec3::Down = { 0.f, -1.f, 0.f };
const Vec3 Vec3::Right = { 1.f, 0.f, 0.f };
const Vec3 Vec3::Left = { -1.f, 0.f, 0.f };
const Vec3 Vec3::Forward = { 0.f, 0.f, -1.f };
const Vec3 Vec3::Backward = { 0.f, 0.f, 1.f };

const Vec4 Vec4::Zero = { 0.f, 0.f, 0.f, 0.f };
const Vec4 Vec4::One = { 1.f, 1.f, 1.f, 1.f };
const Vec4 Vec4::UnitX = { 1.f, 0.f, 0.f, 0.f };
const Vec4 Vec4::UnitY = { 0.f, 1.f, 0.f, 0.f };
const Vec4 Vec4::UnitZ = { 0.f, 0.f, 1.f, 0.f };
const Vec4 Vec4::UnitW = { 0.f, 0.f, 0.f, 1.f };

const Matrix Matrix::Identity = { 1.f, 0.f, 0.f, 0.f,
                                  0.f, 1.f, 0.f, 0.f,
                                  0.f, 0.f, 1.f, 0.f,
                                  0.f, 0.f, 0.f, 1.f };

const Quat Quat::Identity = { 0.f, 0.f, 0.f, 1.f };

inline bool Plane::Contains(const Vec3& point) const noexcept
{
    return IsNearlyZero(point.Dot(Normal()) + D());
}

inline bool Plane::Intersects(const Triangle& triangle) const noexcept
{
    return triangle.Intersects(*this);
}

inline bool Plane::Intersects(const Line& line) const noexcept
{
    Vec3 dir = line.end - line.start;
    float dot = dir.Dot(Normal());
    if (IsNearlyZero(dot) == true)
    {
        return Contains(line.start);
    }
    return true;
}

inline bool Plane::Intersects(const Plane& plane) const noexcept
{
    Vec3 cross = Normal().Cross(plane.Normal());
    if (IsNearlyZero(std::fabs(cross.Dot(cross))) == false)
    {
        return true;
    }

    // 평행
    float dot = Normal().Dot(plane.Normal());
    if (dot > 0.0f)
    {
        return IsNearlyZero(std::fabs(D() - plane.D()));
    }
    else
    {
        return IsNearlyZero(D() + plane.D());
    }
}

Plane Plane::CreateFromTriangle(const Triangle& triangle) noexcept
{
    Vec3 normal = (triangle.b - triangle.a).Cross(triangle.c - triangle.a);
    normal.Normalize();
    float dist = normal.Dot(triangle.a);

    return Plane(normal, dist);
}

bool Line::Contains(const Vec3& point) const noexcept
{
    Vec3 closestPos = ClosestPoint(*this, point);
    return IsNearlyZero(std::fabs((closestPos - point).LengthSquared()));
}

bool Line::Intersects(const Plane& plane) const noexcept
{
    plane.Intersects(*this);
}

bool Triangle::Contains(const Vec3& point) const noexcept
{
    Vec3 posToX = a - point;
    Vec3 posToY = b - point;
    Vec3 posToZ = c - point;

    Vec3 normalPYZ = posToY.Cross(posToZ);
    Vec3 normalPZX = posToZ.Cross(posToX);
    Vec3 normalPXY = posToX.Cross(posToY);

    /*
    *  X
    *  |\
    *  | \  P --> 중간에 역방향 노말
    *  |__\
    *  Y   Z
    */
    /*
    *  X
    *  |\
    *  |P\    --> 모두 동일 방향(위 or 아래) 노말
    *  |__\
    *  Y   Z
    */
    /* 노말 벡터가 같은 방향인지 검사 (삼각형) */

    if (normalPYZ.Dot(normalPZX) < 0.f)
    {
        return false;
    }
    if (normalPYZ.Dot(normalPXY) < 0.f)
    {
        return false;
    }
    return true;
}

inline bool Triangle::Intersects(const Triangle& triangle) const noexcept
{
    return DirectX::TriangleTests::Intersects(a, b, c, triangle.a, triangle.b, triangle.c);
}

inline bool Triangle::Intersects(const Plane& plane) const noexcept
{
    return DirectX::TriangleTests::Intersects(a, b, c, plane);
}

inline bool BoundingAABB2D::Contains(const BoundingOBB2D& obb) const noexcept
{
    obb.Contains(*this);
    return false;
}

inline bool BoundingAABB2D::Intersects(const BoundingOBB2D& obb) const noexcept
{
    obb.Intersects(*this);
    return false;
}

void BoundingAABB2D::GetConners(OUT Vec2* c) const
{
    c[0].x = (float)x;
    c[0].y = (float)y;

    c[1].x = (float)x + (float)width;
    c[1].y = (float)y;

    c[2].x = (float)x + (float)width;
    c[2].y = (float)y + (float)height;

    c[3].x = (float)x;
    c[3].y = (float)y + (float)height;
}

void BoundingAABB2D::GetConners(OUT std::vector<Vec2>& c) const
{
    GetConners(c.data());
}

inline Vec2 BoundingOBB2D::Location() const noexcept
{
    Vec2 hx = axisX * (float)halfWidth;
    Vec2 hy = axisY * (float)halfHeight;

    return Center() - hx - hy;
}

inline Vec2 BoundingOBB2D::Center() const noexcept
{
    return Vec2((float)centerX, (float)centerY);
}

void BoundingOBB2D::GetConners(OUT Vec2* c) const
{
    Vec2 hx = axisX * (float)halfWidth;
    Vec2 hy = axisY * (float)halfHeight;
    Vec2 center = Center();

    c[0] = center - hx - hy;
    c[1] = center + hx - hy;
    c[2] = center + hx + hy;
    c[3] = center - hx + hy;
}

void BoundingOBB2D::GetConners(OUT std::vector<Vec2>& c) const
{
    GetConners(c.data());
}

inline bool BoundingOBB2D::Contains(const Vec2& point) const noexcept
{
    Vec2 local = point - Center();
    float x = local.Dot(axisX);
    float y = local.Dot(axisY);
    return (std::abs(x) <= halfWidth && std::abs(y) <= halfHeight);
}

bool BoundingOBB2D::Contains(const BoundingAABB2D& r) const noexcept
{
    Vec2 otherCorners[4];
    r.GetConners(otherCorners);

    for (int i = 0; i < 4; ++i)
    {
        if (Contains(otherCorners[i]) == false)
        {
            return false;
        }
    }
    return true;
}

bool BoundingOBB2D::Contains(const RECT& rct) const noexcept
{
    Vec2 otherCorners[4] = {
        { (float)rct.left, (float)rct.top },
        { (float)rct.right, (float)rct.top },
        { (float)rct.right, (float)rct.bottom },
        { (float)rct.left, (float)rct.bottom }
    };

    for (int i = 0; i < 4; ++i)
    {
        if (Contains(otherCorners[i]) == false)
        {
            return false;
        }
    }
    return true;
}

bool BoundingOBB2D::Contains(const BoundingOBB2D& obb) const noexcept
{
    Vec2 otherCorners[4];
    obb.GetConners(otherCorners);

    for (int i = 0; i < 4; ++i)
    {
        if (Contains(otherCorners[i]) == false)
        {
            return false;
        }
    }
    return true;
}

inline void BoundingOBB2D::Inflate(long horizAmount, long vertAmount) noexcept
{
    halfWidth += horizAmount;
    halfHeight += vertAmount;
}

bool BoundingOBB2D::Intersects(const BoundingAABB2D& r) const noexcept
{
    Vec2 centerLine = Center() - r.Center();
    float otherHalfSizeX = r.width / 2.f;
    float otherHalfSizeY = r.height / 2.f;

    if (OverlapBox2DOnAxis(centerLine, axisX, (float)halfWidth, Vec2::UnitX, Vec2::UnitY, otherHalfSizeX, otherHalfSizeY) == false)
    {
        return false;
    }
    if (OverlapBox2DOnAxis(centerLine, axisY, (float)halfHeight, Vec2::UnitX, Vec2::UnitY, otherHalfSizeX, otherHalfSizeY) == false)
    {
        return false;
    }

    if (OverlapBox2DOnAxis(centerLine, Vec2::UnitX, otherHalfSizeX, axisX, axisY, (float)halfWidth, (float)halfHeight) == false)
    {
        return false;
    }
    if (OverlapBox2DOnAxis(centerLine, Vec2::UnitY, otherHalfSizeY, axisX, axisY, (float)halfWidth, (float)halfHeight) == false)
    {
        return false;
    }

    return true;
}

inline bool BoundingOBB2D::Intersects(const RECT& rct) const noexcept
{
    BoundingAABB2D aabb = BoundingAABB2D(rct);
    return Intersects(aabb);
}

bool BoundingOBB2D::Intersects(const BoundingOBB2D& obb) const noexcept
{
    Vec2 centerLine = Center() - obb.Center();

    if (OverlapBox2DOnAxis(centerLine, axisX, (float)halfWidth, obb.axisX, obb.axisY, (float)obb.halfWidth, (float)obb.halfHeight) == false)
    {
        return false;
    }
    if (OverlapBox2DOnAxis(centerLine, axisY, (float)halfHeight, obb.axisX, obb.axisY, (float)obb.halfWidth, (float)obb.halfHeight) == false)
    {
        return false;
    }

    if (OverlapBox2DOnAxis(centerLine, obb.axisX, (float)obb.halfWidth, axisX, axisY, (float)halfWidth, (float)halfHeight) == false)
    {
        return false;
    }
    if (OverlapBox2DOnAxis(centerLine, obb.axisY, (float)obb.halfHeight, axisX, axisY, (float)halfWidth, (float)halfHeight) == false)
    {
        return false;
    }

    return true;
}
