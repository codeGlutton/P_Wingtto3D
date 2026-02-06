#pragma once

#include "SimpleMath/SimpleMath.h"

/* 단위 */

struct Vec2 : public DirectX::SimpleMath::Vector2
{
	GEN_MINIMUM_STRUCT_REFLECTION(Vec2)

	using DirectX::SimpleMath::Vector2::Vector2;

	PROPERTY(x)
	PROPERTY(y)

	static const Vec2 Zero;
	static const Vec2 One;
	static const Vec2 UnitX;
	static const Vec2 UnitY;
};

struct Vec3 : public DirectX::SimpleMath::Vector3
{
	GEN_MINIMUM_STRUCT_REFLECTION(Vec3)

	using DirectX::SimpleMath::Vector3::Vector3;

	PROPERTY(x)
	PROPERTY(y)
	PROPERTY(z)

	static const Vec3 Zero;
	static const Vec3 One;
	static const Vec3 UnitX;
	static const Vec3 UnitY;
	static const Vec3 UnitZ;
	static const Vec3 Up;
	static const Vec3 Down;
	static const Vec3 Right;
	static const Vec3 Left;
	static const Vec3 Forward;
	static const Vec3 Backward;
};

struct Vec4 : public DirectX::SimpleMath::Vector4
{
	GEN_MINIMUM_STRUCT_REFLECTION(Vec4)

	using DirectX::SimpleMath::Vector4::Vector4;

	PROPERTY(x)
	PROPERTY(y)
	PROPERTY(z)
	PROPERTY(w)

	static const Vec4 Zero;
	static const Vec4 One;
	static const Vec4 UnitX;
	static const Vec4 UnitY;
	static const Vec4 UnitZ;
	static const Vec4 UnitW;
};

struct Matrix2D : DirectX::XMFLOAT3X3
{
	GEN_MINIMUM_STRUCT_REFLECTION(Matrix2D)

	PROPERTY(_11)
	PROPERTY(_12)
	PROPERTY(_13)

	PROPERTY(_21)
	PROPERTY(_22)
	PROPERTY(_23)

	PROPERTY(_31)
	PROPERTY(_32)
	PROPERTY(_33)

public:
	Matrix2D() noexcept : 
		DirectX::XMFLOAT3X3(
			1.f, 0, 0,
			0, 1.f, 0,
			0, 0, 1.f
		) 
	{
	}
	Matrix2D(
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22
	) noexcept :
		DirectX::XMFLOAT3X3(
			m00, m01, m02,
			m10, m11, m12,
			m20, m21, m22
		) 
	{
	}
	explicit Matrix2D(const Vec2& r0, const Vec2& r1) noexcept :
		DirectX::XMFLOAT3X3(
			r0.x, r0.y, 0,
			r1.x, r1.y, 0,
			0, 0, 1.f
		) 
	{
	}
	explicit Matrix2D(const Vec3& r0, const Vec3& r1, const Vec3& r2) noexcept :
		DirectX::XMFLOAT3X3(
			r0.x, r0.y, r0.z,
			r1.x, r1.y, r1.z,
			r2.x, r2.y, r2.z
		)
	{
	}
	Matrix2D(const DirectX::XMFLOAT3X3& M) noexcept 
	{
		memcpy(this, &M, sizeof(XMFLOAT3X3));
	}
	explicit Matrix2D(_In_reads_(9) const float* pArray) noexcept : XMFLOAT3X3(pArray) 
	{
	}

	Matrix2D(const Matrix2D&) = default;
	Matrix2D& operator=(const Matrix2D&) = default;

	Matrix2D(Matrix2D&&) = default;
	Matrix2D& operator=(Matrix2D&&) = default;

public:
	operator DirectX::XMMATRIX() const noexcept 
	{
		return DirectX::XMMATRIX(
			_11, _12, 0.f, 0.f,
			_21, _22, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			_31, _32, 0.f, 1.f
		);
	}

public:
	bool operator == (const Matrix2D& M) const noexcept;
	bool operator != (const Matrix2D& M) const noexcept;

	Matrix2D& operator= (const XMFLOAT3X3& M) noexcept;
	Matrix2D& operator+= (const Matrix2D& M) noexcept;
	Matrix2D& operator-= (const Matrix2D& M) noexcept;
	Matrix2D& operator*= (const Matrix2D& M) noexcept;
	Matrix2D& operator*= (float S) noexcept;
	Matrix2D& operator/= (float S) noexcept;

	Matrix2D& operator/= (const Matrix2D& M) noexcept;

public:
	Matrix2D operator+ () const noexcept
	{
		return *this; 
	}
	Matrix2D operator- () const noexcept;

public:
	Vec2 Up() const noexcept
	{
		return Vec2(_21, _22);
	}
	void Up(const Vec2& v) noexcept 
	{ 
		_21 = v.x; 
		_22 = v.y;
	}

	Vec2 Down() const  noexcept 
	{ 
		return Vec2(-_21, -_22);
	}
	void Down(const Vec2& v) noexcept 
	{ 
		_21 = -v.x; 
		_22 = -v.y;
	}

	Vec2 Right() const noexcept 
	{ 
		return Vec2(_11, _12); 
	}
	void Right(const Vec2& v) noexcept 
	{ 
		_11 = v.x; 
		_12 = v.y;
	}

	Vec2 Left() const noexcept
	{ 
		return Vec2(-_11, -_12); 
	}
	void Left(const Vec2& v) noexcept 
	{ 
		_11 = -v.x;
		_12 = -v.y;
	}

	Vec2 Translation() const noexcept 
	{ 
		return Vec2(_31, _32); 
	}
	void Translation(const Vec2& v) noexcept 
	{
		_31 = v.x; 
		_32 = v.y;
	}

public:
	bool Decompose(Vec2& scale, Vec2& translation) const noexcept;
	bool Decompose(Vec2& scale, float& radians, Vec2& translation) const noexcept;

	Matrix2D Transpose() const noexcept;
	void Transpose(Matrix2D& result) const noexcept;

	Matrix2D Invert() const noexcept;
	void Invert(Matrix2D& result) const noexcept;

	float Determinant() const noexcept;

public:
	static Matrix2D CreateTranslation(const Vec2& position) noexcept;
	static Matrix2D CreateTranslation(float x, float y) noexcept;

	static Matrix2D CreateScale(const Vec2& scales) noexcept;
	static Matrix2D CreateScale(float xs, float ys) noexcept;
	static Matrix2D CreateScale(float scale) noexcept;

	static Matrix2D CreateRotation(float radians) noexcept;

	static void Lerp(const Matrix2D& M1, const Matrix2D& M2, float t, Matrix2D& result) noexcept;
	static Matrix2D Lerp(const Matrix2D& M1, const Matrix2D& M2, float t) noexcept;

	static void Transform(const Matrix2D& M, float radians, Matrix2D& result) noexcept;
	static Matrix2D Transform(const Matrix2D& M, float radians) noexcept;

	static const Matrix2D Identity;
};

Matrix2D operator+ (const Matrix2D& M1, const Matrix2D& M2) noexcept;
Matrix2D operator- (const Matrix2D& M1, const Matrix2D& M2) noexcept;
Matrix2D operator* (const Matrix2D& M1, const Matrix2D& M2) noexcept;
Matrix2D operator* (const Matrix2D& M, float S) noexcept;
Matrix2D operator/ (const Matrix2D& M, float S) noexcept;
Matrix2D operator/ (const Matrix2D& M1, const Matrix2D& M2) noexcept;

Matrix2D operator* (float S, const Matrix2D& M) noexcept;

struct Matrix : public DirectX::SimpleMath::Matrix
{
	GEN_MINIMUM_STRUCT_REFLECTION(Matrix)
	
	using DirectX::SimpleMath::Matrix::Matrix;

	PROPERTY(_11)
	PROPERTY(_12)
	PROPERTY(_13)
	PROPERTY(_14)

	PROPERTY(_21)
	PROPERTY(_22)
	PROPERTY(_23)
	PROPERTY(_24)

	PROPERTY(_31)
	PROPERTY(_32)
	PROPERTY(_33)
	PROPERTY(_34)

	PROPERTY(_41)
	PROPERTY(_42)
	PROPERTY(_43)
	PROPERTY(_44)

	static const Matrix Identity;
};

struct Quat : public DirectX::SimpleMath::Quaternion
{
	GEN_MINIMUM_STRUCT_REFLECTION(Quat)

	using DirectX::SimpleMath::Quaternion::Quaternion;

public:
	void ToYawPitchRoll(OUT float& yaw, OUT float& pitch, OUT float& roll)
	{
		Vec3 v = ToYawPitchRoll();
		yaw = v.x;
		pitch = v.y;
		roll = v.z;
	}
	Vec3 ToYawPitchRoll();

public:
	PROPERTY(x)
	PROPERTY(y)
	PROPERTY(z)
	PROPERTY(w)

	static const Quat Identity;
};

struct Color : public DirectX::SimpleMath::Color
{
	GEN_MINIMUM_STRUCT_REFLECTION(Color)

	using DirectX::SimpleMath::Color::Color;

	PROPERTY(x)
	PROPERTY(y)
	PROPERTY(z)
	PROPERTY(w)

	static const Color White;
	static const Color Black;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
};

/* 측정 도구 */

struct Plane;
struct Line;
struct Triangle;

using Ray = DirectX::SimpleMath::Ray;

struct Plane : public DirectX::SimpleMath::Plane
{
	GEN_MINIMUM_STRUCT_REFLECTION(Plane)

	using DirectX::SimpleMath::Plane::Plane;

public:
	bool Contains(const Vec3& point) const noexcept;

public:
	bool Intersects(const Triangle& triangle) const noexcept;
	bool Intersects(const Line& line) const noexcept;
	bool Intersects(const Plane& plane) const noexcept;

public:
	static Plane CreateFromTriangle(const Triangle& triangle) noexcept;

public:
	PROPERTY(x)
	PROPERTY(y)
	PROPERTY(z)
	PROPERTY(w)
};

struct Line
{
	GEN_MINIMUM_STRUCT_REFLECTION(Line)

	bool operator==(const Line& r) const noexcept = default;

public:
	Line() = default;
	Line(Vec3 s, Vec3 e) :
		start(s),
		end(e)
	{
	}
	~Line() = default;

public:
	bool Contains(const Vec3& point) const noexcept;

public:
	bool Intersects(const Plane& plane) const noexcept;

	PROPERTY(start)
	Vec3 start;
	PROPERTY(end)
	Vec3 end;
};

struct Triangle
{
	GEN_MINIMUM_STRUCT_REFLECTION(Triangle)

	bool operator==(const Triangle& r) const noexcept = default;

public:
	bool Contains(const Vec3& point) const noexcept;

public:
	bool Intersects(const Triangle& triangle) const noexcept;
	bool Intersects(const Plane& plane) const noexcept;

public:
	PROPERTY(a)
	PROPERTY(b)
	PROPERTY(c)

	union
	{
		struct
		{
			Vec3 a;
			Vec3 b;
			Vec3 c;
		};

		Vec3 points[3];
		float values[9];
	};
};

struct Frustum : public DirectX::SimpleMath::Viewport
{
	GEN_MINIMUM_STRUCT_REFLECTION(Frustum)

	using DirectX::SimpleMath::Viewport::Viewport;
	
	PROPERTY(x)
	PROPERTY(y)
	PROPERTY(width)
	PROPERTY(height)
	PROPERTY(minDepth)
	PROPERTY(maxDepth)
};

/* 2D 바운딩 */

struct BoundingOBB2D;
struct BoundingAABB2D : public DirectX::SimpleMath::Rectangle
{
	GEN_MINIMUM_STRUCT_REFLECTION(BoundingAABB2D)

	using DirectX::SimpleMath::Rectangle::Rectangle;

public:
	using Rectangle::Contains;
	using Rectangle::Intersects;
	bool Contains(const BoundingOBB2D& obb) const noexcept;
	bool Intersects(const BoundingOBB2D& obb) const noexcept;

public:
	void GetCorners(OUT Vec2* c) const;
	void GetCorners(OUT std::vector<Vec2>& c) const;

public:
	bool IsValid() const
	{
		return width > 0.f && height > 0.f;
	}

public:
	PROPERTY(x)
	PROPERTY(y)
	PROPERTY(width)
	PROPERTY(height)
};

struct BoundingOBB2D
{
	GEN_MINIMUM_STRUCT_REFLECTION(BoundingOBB2D)

public:
	BoundingOBB2D() noexcept : 
		centerX(0), 
		centerY(0),
		axisX(Vec2::UnitX),
		axisY(Vec2::UnitY),
		halfWidth(0),
		halfHeight(0) 
	{
	}
	constexpr BoundingOBB2D(long icx, long icy, Vec2 iaxisX, Vec2 iaxisY, long ihw, long ihh) noexcept :
		centerX(icx),
		centerY(icy),
		axisX(iaxisX),
		axisY(iaxisY),
		halfWidth(ihw), 
		halfHeight(ihh)
	{
	}
	explicit BoundingOBB2D(const RECT& rct, Vec2 iaxisX, Vec2 iaxisY) noexcept :
		centerX((rct.left + rct.right) / 2),
		centerY((rct.top + rct.bottom) / 2),
		axisX(iaxisX),
		axisY(iaxisY),
		halfWidth((rct.right - rct.left) / 2),
		halfHeight((rct.bottom - rct.top) / 2)
	{
	}
	explicit BoundingOBB2D(const BoundingAABB2D& aabb, Vec2 iaxisX, Vec2 iaxisY) noexcept :
		centerX((aabb.x + aabb.width) / 2),
		centerY((aabb.y + aabb.height) / 2),
		axisX(iaxisX),
		axisY(iaxisY),
		halfWidth(aabb.width / 2),
		halfHeight(aabb.height / 2)
	{
	}

	BoundingOBB2D(const BoundingOBB2D&) = default;
	BoundingOBB2D& operator=(const BoundingOBB2D&) = default;

	BoundingOBB2D(BoundingOBB2D&&) = default;
	BoundingOBB2D& operator=(BoundingOBB2D&&) = default;

public:
	bool operator==(const BoundingOBB2D& r) const noexcept = default;
	bool operator!=(const BoundingOBB2D& r) const noexcept = default;

public:
	Vec2 Location() const noexcept;
	Vec2 Center() const noexcept;

	void GetCorners(OUT Vec2* c) const;
	void GetCorners(OUT std::vector<Vec2>& c) const;

	bool IsEmpty() const noexcept 
	{ 
		return (halfWidth == 0 && halfHeight == 0 && centerX == 0 && centerY == 0);
	}

	bool Contains(long ix, long iy) const noexcept
	{
		Vec2 vec = Vec2(float(ix), float(iy));
		return Contains(vec);
	}
	bool Contains(const Vec2& point) const noexcept;
	bool Contains(const BoundingAABB2D& r) const noexcept;
	bool Contains(const RECT& rct) const noexcept;
	bool Contains(const BoundingOBB2D& obb) const noexcept;

	void Inflate(long horizAmount, long vertAmount) noexcept;

	bool Intersects(const BoundingAABB2D& r) const noexcept;
	bool Intersects(const RECT& rct) const noexcept;
	bool Intersects(const BoundingOBB2D& obb) const noexcept;

	void Offset(long ox, long oy) noexcept 
	{ 
		centerX += ox; 
		centerY += oy;
	}
	void OffsetByAxis(long ox, long oy) noexcept
	{
		Vec2 hx = axisX * (float)ox;
		Vec2 hy = axisY * (float)oy;

		centerX += ox + (long)hx.x + (long)hy.x;
		centerY += oy + (long)hx.y + (long)hy.y;
	}

public:
	BoundingAABB2D ToBoundingAABB2D() const;

public:
	PROPERTY(centerX)
	long centerX;
	PROPERTY(centerY)
	long centerY;
	PROPERTY(axisX)
	Vec2 axisX;
	PROPERTY(axisY)
	Vec2 axisY;
	PROPERTY(halfWidth)
	long halfWidth;
	PROPERTY(halfHeight)
	long halfHeight;
};
