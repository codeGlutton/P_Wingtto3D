#pragma once

#include <variant>
#include "Input/InputInclude.h"

enum class KeyValueType
{
	Bool,
	Float,
	Vec2,
};

struct InputValue
{
	friend class InputManager;

public:
	using InputVariant = std::variant<bool, float, Vec2>;

public:
	InputValue() :
		InputValue(false)
	{
	}
	InputValue(bool value) :
		_mValue(value == true ? Vec2(1.f, 0.f) : Vec2::Zero),
		_mType(KeyValueType::Bool)
	{
	}
	InputValue(float value) :
		_mValue(Vec2(value, 0.f)),
		_mType(KeyValueType::Float)
	{
	}
	InputValue(const Vec2& value) :
		_mValue(value),
		_mType(KeyValueType::Vec2)
	{
	}
	InputValue(Vec2&& value) :
		_mValue(value),
		_mType(KeyValueType::Vec2)
	{
	}

public:
	template<typename T>
	void Convert()
	{
		T value = Get<T>();
		Set(value);
	}

public:
	template<typename T>
	T Get() const 
	{ 
		STATIC_ASSERT_MSG(sizeof(T) == 0, "Unsupported value type");
	}

	KeyValueType GetValueType() const
	{
		return _mType;
	}

private:
	template<typename T>
	void Set(const T& value)
	{
		STATIC_ASSERT_MSG(sizeof(T) == 0, "Unsupported value type");
	}

private:
	Vec2 _mValue;
	KeyValueType _mType;
};

template<>
inline bool InputValue::Get() const
{
	return _mValue.LengthSquared() > std::numeric_limits<float>::epsilon();
}
template<>
inline float InputValue::Get() const
{
	return _mValue.x;
}
template<>
inline Vec2 InputValue::Get() const
{
	return _mValue;
}

template<>
inline void InputValue::Set(const bool& value)
{
	_mValue = value == true ? Vec2(1.f, 0.f) : Vec2::Zero;
	_mType = KeyValueType::Bool;
}
template<>
inline void InputValue::Set(const float& value)
{
	_mValue.x = value;
	_mValue.y = 0.f;
	_mType = KeyValueType::Float;
}
template<>
inline void InputValue::Set(const Vec2& value)
{
	_mValue = value;
	_mType = KeyValueType::Vec2;
}

