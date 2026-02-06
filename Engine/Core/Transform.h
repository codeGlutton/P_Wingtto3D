#pragma once

struct Transform2D
{
	GEN_MINIMUM_STRUCT_REFLECTION(Transform2D)

public:
	Transform2D() : 
		_mLocalScale(1.f)
	{
	}
	Transform2D(Vec2 localPos, float rotation, Vec2 localScale) :
		_mLocalPosition(localPos),
		_mLocalRotation(rotation),
		_mLocalScale(localScale)
	{
	}
	Transform2D(Matrix2D mat)
	{
		mat.CreateTranslation(_mLocalPosition);
		mat.CreateRotation(_mLocalRotation);
		mat.CreateScale(_mLocalScale);

		_mLocalRotation = _mLocalRotation / DirectX::XM_PI * 180.f;
	}

public:
	Vec2 GetLocalScale() const
	{
		return _mLocalScale;
	}
	void SetLocalScale(const Vec2& localScale)
	{
		_mLocalScale = localScale;
	}
	float GetLocalRotation() const
	{
		return _mLocalRotation;
	}
	void SetLocalRotation(float localRotation)
	{
		_mLocalRotation = localRotation;
	}
	Vec2 GetLocalPosition() const
	{
		return _mLocalPosition;
	}
	void SetLocalPosition(const Vec2& localPosition)
	{
		_mLocalPosition = localPosition;
	}

public:
	Matrix2D MakeLocalMatrix() const;
	Matrix2D MakeLocalSTMatrix() const;

private:
	PROPERTY(_mLocalPosition)
	Vec2 _mLocalPosition = { 0.f, 0.f };
	PROPERTY(_mLocalRotation)
	float _mLocalRotation = 0.f;
	PROPERTY(_mLocalScale)
	Vec2 _mLocalScale = { 1.f, 1.f };
};

struct Transform
{
	GEN_MINIMUM_STRUCT_REFLECTION(Transform)

public:
	Transform() :
		_mLocalScale(1.f)
	{
	}
	Transform(Vec3 localPos, Vec3 rotation, Vec3 localScale) :
		_mLocalPosition(localPos),
		_mLocalRotation(rotation),
		_mLocalScale(localScale)
	{
	}
	Transform(Vec3 localPos, Quat quat, Vec3 localScale) :
		_mLocalPosition(localPos),
		_mLocalRotation(quat.ToYawPitchRoll()),
		_mLocalScale(localScale)
	{
	}
	Transform(Matrix mat)
	{
		mat.CreateTranslation(_mLocalPosition);
		mat.CreateRotationX(_mLocalRotation.x);
		mat.CreateRotationY(_mLocalRotation.y);
		mat.CreateRotationZ(_mLocalRotation.z);
		mat.CreateScale(_mLocalScale);
	}

public:
	Vec3 GetLocalScale() const 
	{
		return _mLocalScale;
	}
	void SetLocalScale(const Vec3& localScale) 
	{ 
		_mLocalScale = localScale;
	}
	Vec3 GetLocalRotation() const 
	{ 
		return _mLocalRotation;
	}
	void SetLocalRotation(const Vec3& localRotation) 
	{ 
		_mLocalRotation = localRotation;
	}
	Vec3 GetLocalPosition() const 
	{ 
		return _mLocalPosition;
	}
	void SetLocalPosition(const Vec3& localPosition) 
	{ 
		_mLocalPosition = localPosition;
	}

public:
	Matrix MakeLocalMatrix() const;

private:
	PROPERTY(_mLocalPosition)
	Vec3 _mLocalPosition = { 0.f, 0.f, 0.f };
	PROPERTY(_mLocalRotation)
	Vec3 _mLocalRotation = { 0.f, 0.f, 0.f };
	PROPERTY(_mLocalScale)
	Vec3 _mLocalScale = { 1.f, 1.f, 1.f };
};

