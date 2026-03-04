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

const Matrix2D Matrix2D::Identity = { 1.f, 0.f, 0.f,
                                      0.f, 1.f, 0.f,
                                      0.f, 0.f, 1.f };
const Matrix Matrix::Identity = { 1.f, 0.f, 0.f, 0.f,
                                  0.f, 1.f, 0.f, 0.f,
                                  0.f, 0.f, 1.f, 0.f,
                                  0.f, 0.f, 0.f, 1.f };

const Quat Quat::Identity = { 0.f, 0.f, 0.f, 1.f };

const Color Color::White = { 1.f, 1.f, 1.f, 1.f };
const Color Color::Black = { 0.f, 0.f, 0.f, 1.f };
const Color Color::Red = { 1.f, 0.f, 0.f, 1.f };
const Color Color::Green = { 0.f, 1.f, 0.f, 1.f };
const Color Color::Blue = { 0.f, 0.f, 1.f, 1.f };

Matrix2D::operator Matrix() const noexcept
{
    return Matrix(
        _11, _12, 0.f, _13,
        _21, _22, 0.f, _23,
        0.f, 0.f, 1.f, 0.f,
        _31, _32, 0.f, _33
    );
}

bool Matrix2D::operator== (const Matrix2D& M) const noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_31));

    XMVECTOR y1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._11));
    XMVECTOR y2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._21));
    XMVECTOR y3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._31));

    return (XMVector3Equal(x1, y1)
        && XMVector3Equal(x2, y2)
        && XMVector3Equal(x3, y3)) != 0;
}

bool Matrix2D::operator != (const Matrix2D& M) const noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_31));

    XMVECTOR y1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._11));
    XMVECTOR y2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._21));
    XMVECTOR y3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._31));

    return (XMVector3NotEqual(x1, y1)
        || XMVector3NotEqual(x2, y2)
        || XMVector3NotEqual(x3, y3)) != 0;
}

Matrix2D& Matrix2D::operator= (const DirectX::XMFLOAT3X3& M) noexcept
{
    _11 = M._11; _12 = M._12; _13 = M._13;
    _21 = M._21; _22 = M._22; _23 = M._23;
    _31 = M._31; _32 = M._32; _33 = M._33;
    return *this;
}

Matrix2D& Matrix2D::operator+= (const Matrix2D& M) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_31));

    XMVECTOR y1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._11));
    XMVECTOR y2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._21));
    XMVECTOR y3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._31));

    x1 = XMVectorAdd(x1, y1);
    x2 = XMVectorAdd(x2, y2);
    x3 = XMVectorAdd(x3, y3);

    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_31), x3);
    return *this;
}

Matrix2D& Matrix2D::operator-= (const Matrix2D& M) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_31));

    XMVECTOR y1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._11));
    XMVECTOR y2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._21));
    XMVECTOR y3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._31));

    x1 = XMVectorSubtract(x1, y1);
    x2 = XMVectorSubtract(x2, y2);
    x3 = XMVectorSubtract(x3, y3);

    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_31), x3);
    return *this;
}

Matrix2D& Matrix2D::operator*= (const Matrix2D& M) noexcept
{
    using namespace DirectX;
    XMMATRIX M1 = XMLoadFloat3x3(this);
    XMMATRIX M2 = XMLoadFloat3x3(&M);
    XMMATRIX X = XMMatrixMultiply(M1, M2);
    XMStoreFloat3x3(this, X);
    return *this;
}

Matrix2D& Matrix2D::operator*= (float S) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_31));

    x1 = XMVectorScale(x1, S);
    x2 = XMVectorScale(x2, S);
    x3 = XMVectorScale(x3, S);

    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_31), x3);
    return *this;
}

Matrix2D& Matrix2D::operator/= (float S) noexcept
{
    using namespace DirectX;
    assert(S != 0.f);
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_31));

    float rs = 1.f / S;

    x1 = XMVectorScale(x1, rs);
    x2 = XMVectorScale(x2, rs);
    x3 = XMVectorScale(x3, rs);

    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_31), x3);
    return *this;
}

Matrix2D& Matrix2D::operator/= (const Matrix2D& M) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_31));

    XMVECTOR y1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._11));
    XMVECTOR y2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._21));
    XMVECTOR y3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._31));

    x1 = XMVectorDivide(x1, y1);
    x2 = XMVectorDivide(x2, y2);
    x3 = XMVectorDivide(x3, y3);

    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&_31), x3);
    return *this;
}

Matrix2D Matrix2D::operator- () const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_11));
    XMVECTOR v2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_21));
    XMVECTOR v3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&_31));

    v1 = XMVectorNegate(v1);
    v2 = XMVectorNegate(v2);
    v3 = XMVectorNegate(v3);

    Matrix2D R;
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), v1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), v2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), v3);
    return R;
}

Matrix2D operator+ (const Matrix2D& M1, const Matrix2D& M2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._31));

    XMVECTOR y1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._11));
    XMVECTOR y2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._21));
    XMVECTOR y3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._31));

    x1 = XMVectorAdd(x1, y1);
    x2 = XMVectorAdd(x2, y2);
    x3 = XMVectorAdd(x3, y3);

    Matrix2D R;
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), x3);
    return R;
}

Matrix2D operator- (const Matrix2D& M1, const Matrix2D& M2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._31));

    XMVECTOR y1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._11));
    XMVECTOR y2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._21));
    XMVECTOR y3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._31));

    x1 = XMVectorSubtract(x1, y1);
    x2 = XMVectorSubtract(x2, y2);
    x3 = XMVectorSubtract(x3, y3);

    Matrix2D R;
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), x3);
    return R;
}

Matrix2D operator* (const Matrix2D& M1, const Matrix2D& M2) noexcept
{
    using namespace DirectX;
    XMMATRIX m1 = XMLoadFloat3x3(&M1);
    XMMATRIX m2 = XMLoadFloat3x3(&M2);
    XMMATRIX X = XMMatrixMultiply(m1, m2);

    Matrix2D R;
    XMStoreFloat3x3(&R, X);
    return R;
}

Matrix2D operator* (const Matrix2D& M, float S) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._31));

    x1 = XMVectorScale(x1, S);
    x2 = XMVectorScale(x2, S);
    x3 = XMVectorScale(x3, S);

    Matrix2D R;
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), x3);
    return R;
}

Matrix2D operator/ (const Matrix2D& M, float S) noexcept
{
    using namespace DirectX;
    assert(S != 0.f);

    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._31));

    float rs = 1.f / S;

    x1 = XMVectorScale(x1, rs);
    x2 = XMVectorScale(x2, rs);
    x3 = XMVectorScale(x3, rs);

    Matrix2D R;
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), x3);
    return R;
}

Matrix2D operator/ (const Matrix2D& M1, const Matrix2D& M2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._31));

    XMVECTOR y1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._11));
    XMVECTOR y2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._21));
    XMVECTOR y3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._31));

    x1 = XMVectorDivide(x1, y1);
    x2 = XMVectorDivide(x2, y2);
    x3 = XMVectorDivide(x3, y3);

    Matrix2D R;
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), x3);
    return R;
}

Matrix2D operator* (float S, const Matrix2D& M) noexcept
{
    using namespace DirectX;

    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M._31));

    x1 = XMVectorScale(x1, S);
    x2 = XMVectorScale(x2, S);
    x3 = XMVectorScale(x3, S);

    Matrix2D R;
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), x3);
    return R;
}

bool Matrix2D::Decompose(Vec2& scale, Vec2& translation) const noexcept
{
    translation.x = _31;
    translation.y = _32;

    scale.x = sqrtf(_11 * _11 + _12 * _12);
    scale.y = sqrtf(_21 * _21 + _22 * _22);

    return scale.x >= FLT_EPSILON && scale.y >= FLT_EPSILON;
}

bool Matrix2D::Decompose(Vec2& scale, float& radians, Vec2& translation) const noexcept
{
    translation.x = _31;
    translation.y = _32;

    scale.x = sqrtf(_11 * _11 + _12 * _12);
    scale.y = sqrtf(_21 * _21 + _22 * _22);

    if (scale.x < FLT_EPSILON || scale.y < FLT_EPSILON)
    {
        return false;
    }

    radians = atan2f(_12 / scale.x, _11 / scale.x);
    return true;
}

Matrix2D Matrix2D::Transpose() const noexcept
{
    Matrix2D result;
    Transpose(result);
    return result;
}

void Matrix2D::Transpose(Matrix2D& result) const noexcept
{
    using namespace DirectX;
    XMMATRIX M = XMLoadFloat3x3(this);
    XMStoreFloat3x3(&result, XMMatrixTranspose(M));
}

Matrix2D Matrix2D::Invert() const noexcept
{
    Matrix2D result;
    Invert(result);
    return result;
}

void Matrix2D::Invert(Matrix2D& result) const noexcept
{
    float det = Determinant();
    assert(fabsf(det) > FLT_EPSILON);

    float invDet = 1.0f / det;

    result._11 = (_22 * _33 - _23 * _32) * invDet;
    result._12 = (_13 * _32 - _12 * _33) * invDet;
    result._13 = (_12 * _23 - _13 * _22) * invDet;

    result._21 = (_23 * _31 - _21 * _33) * invDet;
    result._22 = (_11 * _33 - _13 * _31) * invDet;
    result._23 = (_13 * _21 - _11 * _23) * invDet;

    result._31 = (_21 * _32 - _22 * _31) * invDet;
    result._32 = (_12 * _31 - _11 * _32) * invDet;
    result._33 = (_11 * _22 - _12 * _21) * invDet;
}

float Matrix2D::Determinant() const noexcept
{
    return
        _11 * (_22 * _33 - _23 * _32) -
        _12 * (_21 * _33 - _23 * _31) +
        _13 * (_21 * _32 - _22 * _31);
}

Matrix2D Matrix2D::CreateTranslation(const Vec2& position) noexcept
{
    return Matrix2D{
         1.f, 0.f, 0.f,
         0.f, 1.f, 0.f,
         position.x, position.y, 1.f,
    };
}

Matrix2D Matrix2D::CreateTranslation(float x, float y) noexcept
{
    return Matrix2D{
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f,
        x, y, 1.f,
    };
}

Matrix2D Matrix2D::CreateScale(const Vec2& scales) noexcept
{
    return Matrix2D{
        scales.x, 0.f, 0.f,
        0.f, scales.y, 0.f,
        0.f, 0.f, 1.f,
    };
}

Matrix2D Matrix2D::CreateScale(float xs, float ys) noexcept
{
    return Matrix2D{
        xs, 0.f, 0.f,
        0.f, ys, 0.f,
        0.f, 0.f, 1.f,
    };
}

Matrix2D Matrix2D::CreateScale(float scale) noexcept
{
    return Matrix2D{
        scale, 0.f, 0.f,
        0.f, scale, 0.f,
        0.f, 0.f, 1.f,
    };
}

Matrix2D Matrix2D::CreateRotation(float radians) noexcept
{
    float cosAngle = std::cosf(radians);
    float sinAngle = std::sinf(radians);

    return Matrix2D{
        cosAngle, sinAngle, 0.f,
        -sinAngle, cosAngle, 0.f,
        0.f, 0.f, 1.f,
    };
}

void Matrix2D::Lerp(const Matrix2D& M1, const Matrix2D& M2, float t, Matrix2D& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._31));

    XMVECTOR y1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._11));
    XMVECTOR y2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._21));
    XMVECTOR y3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._31));

    x1 = XMVectorLerp(x1, y1, t);
    x2 = XMVectorLerp(x2, y2, t);
    x3 = XMVectorLerp(x3, y3, t);

    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&result._11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&result._21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&result._31), x3);
}

Matrix2D Matrix2D::Lerp(const Matrix2D& M1, const Matrix2D& M2, float t) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._11));
    XMVECTOR x2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._21));
    XMVECTOR x3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M1._31));

    XMVECTOR y1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._11));
    XMVECTOR y2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._21));
    XMVECTOR y3 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&M2._31));

    x1 = XMVectorLerp(x1, y1, t);
    x2 = XMVectorLerp(x2, y2, t);
    x3 = XMVectorLerp(x3, y3, t);

    Matrix2D result;
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&result._11), x1);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&result._21), x2);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&result._31), x3);
    return result;
}

void Matrix2D::Transform(const Matrix2D& M, float radians, Matrix2D& result) noexcept
{
    result = M * CreateRotation(radians);
}

Matrix2D Matrix2D::Transform(const Matrix2D& M, float radians) noexcept
{
    Matrix2D result;
    Transform(M, radians, result);
    return result;
}

Matrix::Matrix(const Matrix2D& other)
{
    _11 = other._11;	_12 = other._12;	_13 = 0.f;			_14 = other._13;
    _21 = other._21;	_22 = other._22;	_23 = 0.f;			_24 = other._23;
    _31 = 0.f;			_32 = 0.f;			_33 = 1.f;			_34 = 0.f;
    _41 = other._31;	_42 = other._32;	_43 = 0.f;			_44 = other._33;
}

Matrix& Matrix::operator=(const Matrix2D& other) noexcept
{
    _11 = other._11;	_12 = other._12;	_13 = 0.f;			_14 = other._13;
    _21 = other._21;	_22 = other._22;	_23 = 0.f;			_24 = other._23;
    _31 = 0.f;			_32 = 0.f;			_33 = 1.f;			_34 = 0.f;
    _41 = other._31;	_42 = other._32;	_43 = 0.f;			_44 = other._33;

    return *this;
}

Vec3 Quat::ToYawPitchRoll()
{
    Vec3 angles;

    // roll
    double sinr_cosp = 2 * (w * x + y * z);
    double cosr_cosp = 1 - 2 * (x * x + y * y);
    angles.x = static_cast<float>(std::atan2(sinr_cosp, cosr_cosp));

    // pitch
    double sinp = std::sqrt(1 + 2 * (w * y - x * z));
    double cosp = std::sqrt(1 - 2 * (w * y - x * z));
    angles.y = static_cast<float>(2 * std::atan2(sinp, cosp) - DirectX::XM_PI / 2);

    // yaw
    double siny_cosp = 2 * (w * z + x * y);
    double cosy_cosp = 1 - 2 * (y * y + z * z);
    angles.z = static_cast<float>(std::atan2(siny_cosp, cosy_cosp));

    return angles;
}

bool Plane::Contains(const Vec3& point) const noexcept
{
    return IsNearlyZero(point.Dot(Normal()) + D());
}

bool Plane::Intersects(const Triangle& triangle) const noexcept
{
    return triangle.Intersects(*this);
}

bool Plane::Intersects(const Line& line) const noexcept
{
    Vec3 dir = line.end - line.start;
    float dot = dir.Dot(Normal());
    if (IsNearlyZero(dot) == true)
    {
        return Contains(line.start);
    }
    return true;
}

bool Plane::Intersects(const Plane& plane) const noexcept
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
    return plane.Intersects(*this);
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

bool Triangle::Intersects(const Triangle& triangle) const noexcept
{
    return DirectX::TriangleTests::Intersects(a, b, c, triangle.a, triangle.b, triangle.c);
}

bool Triangle::Intersects(const Plane& plane) const noexcept
{
    return DirectX::TriangleTests::Intersects(a, b, c, plane);
}

bool BoundingAABB2D::Contains(const BoundingOBB2D& obb) const noexcept
{
    obb.Contains(*this);
    return false;
}

bool BoundingAABB2D::Intersects(const BoundingOBB2D& obb) const noexcept
{
    obb.Intersects(*this);
    return false;
}

void BoundingAABB2D::GetCorners(OUT Vec2* c) const
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

void BoundingAABB2D::GetCorners(OUT std::vector<Vec2>& c) const
{
    GetCorners(c.data());
}

Vec2 BoundingOBB2D::Location() const noexcept
{
    Vec2 hx = axisX * (float)halfWidth;
    Vec2 hy = axisY * (float)halfHeight;

    return Center() - hx - hy;
}

Vec2 BoundingOBB2D::Center() const noexcept
{
    return Vec2((float)centerX, (float)centerY);
}

void BoundingOBB2D::GetCorners(OUT Vec2* c) const
{
    Vec2 hx = axisX * (float)halfWidth;
    Vec2 hy = axisY * (float)halfHeight;
    Vec2 center = Center();

    c[0] = center - hx - hy;
    c[1] = center + hx - hy;
    c[2] = center + hx + hy;
    c[3] = center - hx + hy;
}

void BoundingOBB2D::GetCorners(OUT std::vector<Vec2>& c) const
{
    GetCorners(c.data());
}

bool BoundingOBB2D::Contains(const Vec2& point) const noexcept
{
    Vec2 local = point - Center();
    float x = local.Dot(axisX);
    float y = local.Dot(axisY);
    return (std::abs(x) <= halfWidth && std::abs(y) <= halfHeight);
}

bool BoundingOBB2D::Contains(const BoundingAABB2D& r) const noexcept
{
    Vec2 otherCorners[4];
    r.GetCorners(otherCorners);

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
    obb.GetCorners(otherCorners);

    for (int i = 0; i < 4; ++i)
    {
        if (Contains(otherCorners[i]) == false)
        {
            return false;
        }
    }
    return true;
}

void BoundingOBB2D::Inflate(long horizAmount, long vertAmount) noexcept
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

bool BoundingOBB2D::Intersects(const RECT& rct) const noexcept
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

BoundingAABB2D BoundingOBB2D::ToBoundingAABB2D() const
{
    Vec2 corners[4];
    GetCorners(corners);

    Interval<Vec2> aabbSize;
    for (int32 i = 1; i < 4; ++i)
    {
        aabbSize.mMin.x = std::min<float>(aabbSize.mMin.x, corners[i].x);
        aabbSize.mMin.y = std::min<float>(aabbSize.mMin.y, corners[i].y);
        aabbSize.mMax.x = std::max<float>(aabbSize.mMax.x, corners[i].x);
        aabbSize.mMax.y = std::max<float>(aabbSize.mMax.y, corners[i].y);
    }

    return BoundingAABB2D(static_cast<long>(aabbSize.mMin.x), static_cast<long>(aabbSize.mMin.y), static_cast<long>(aabbSize.mMax.x - aabbSize.mMin.x), static_cast<long>(aabbSize.mMax.y - aabbSize.mMin.y));
}


