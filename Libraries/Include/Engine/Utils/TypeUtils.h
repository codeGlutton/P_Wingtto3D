#pragma once

#include "Reflection/TypeInfo.h"
#include "Reflection/DefaultTypeInfo.h"

template<typename To, typename From>
To* DynamicCast(From* src)
{
	if (src != nullptr && src->GetTypeInfo().template IsChildOf<To>() == true)
	{
		return (To*)src;
	}
	return nullptr;
}

template<typename To, typename From>
To* Cast(From* src)
{
	if (src != nullptr)
	{
		if constexpr (std::is_base_of_v<To, From> == true)
		{
			return (To*)src;
		}
		else if (src->GetTypeInfo().template IsChildOf<To>() == true)
		{
			return (To*)src;
		}
	}
	return nullptr;
}

/**
 * 정수 및 실수 타입
 */
template<typename T, typename C = void>
struct TypeInfoResolver
{
	static const NumericTypeInfo<T>& Get()
	{
		STATIC_ASSERT_MSG(std::is_integral_v<T> == true || std::is_floating_point_v<T> == true, "Invalid type");
		return NumericTypeInfo<T>::mStatic;
	}
};

/**
 * void 타입
 */
template<>
struct TypeInfoResolver<void>
{
	static const VoidTypeInfo& Get()
	{
		return VoidTypeInfo::mStatic;
	}
};

/**
 * 클래스 및 구조체
 */
template<typename T>
struct TypeInfoResolver<T, std::enable_if_t<std::is_class_v<T>>>
{
	static const ObjectTypeInfo& Get()
	{
		return T::GetStaticTypeInfo();
	}
};

/**
 * 포인터
 */
template<typename T>
struct TypeInfoResolver<T*>
{
	static const PointerTypeInfo<T>& Get()
	{
		STATIC_ASSERT_MSG(IsChildOfObject<T> == true, "Allow only object pointer");
		return PointerTypeInfo<T>::mStatic;
	}
};

/**
 * 배열
 */
template<typename T, size_t N>
struct TypeInfoResolver<T[N]>
{
	static const ArrayTypeInfo<T, N>& Get()
	{
		return ArrayTypeInfo<T, N>::mStatic;
	}
};

