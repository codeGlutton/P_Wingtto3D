#pragma once

#include "Reflection/ObjectTypeInfo.h"
#include "Reflection/DefaultTypeInfo.h"
#include "Reflection/CommonTypeInfo.h"

template<typename To, typename From>
To* Cast(From* src)
{
	if (src != nullptr)
	{
		if constexpr (std::is_base_of_v<To, From> == true)
		{
			return static_cast<To*>(src);
		}
		else if constexpr (IsInterface<To> == true)
		{
			bool isInherited = false;
			std::size_t offset = src->GetTypeInfo().template GetInterfaceOffsetOf<To>(isInherited);
			if (isInherited == true)
			{
				return reinterpret_cast<To*>(reinterpret_cast<std::size_t>(src) + offset);
			}
		}
		else if (src->GetTypeInfo().template IsChildOf<To>() == true)
		{
			return reinterpret_cast<To*>(src);
		}
	}
	return nullptr;
}

template<typename To>
To* Cast(void* src, const ObjectTypeInfo& srcTypeInfo)
{
	if constexpr (IsInterface<To> == true)
	{
		bool isInherited = false;
		std::size_t offset = srcTypeInfo.template GetInterfaceOffsetOf<To>(isInherited);
		if (isInherited == true)
		{
			return reinterpret_cast<To*>(reinterpret_cast<std::size_t>(src) + offset);
		}
	}
	else if (srcTypeInfo.template IsChildOf<To>() == true)
	{
		return reinterpret_cast<To*>(src);
	}
	return nullptr;
}

template<typename To, typename From>
std::shared_ptr<To> CastSharedPointer(std::shared_ptr<From> src)
{
	if (src != nullptr)
	{
		if constexpr (std::is_base_of_v<To, From> == true)
		{
			return std::static_pointer_cast<To>(src);
		}
		else if constexpr (IsInterface<To> == true)
		{
			bool isInherited = false;
			std::size_t offset = src->GetTypeInfo().template GetInterfaceOffsetOf<To>(isInherited);
			if (isInherited == true)
			{
				return std::shared_ptr<To>(
					src, reinterpret_cast<To*>(reinterpret_cast<std::size_t>(src.get()) + offset)
				);
			}
		}
		else if (src->GetTypeInfo().template IsChildOf<To>() == true)
		{
			return std::reinterpret_pointer_cast<To>(src);
		}
	}
	return nullptr;
}

template<typename To>
std::shared_ptr<To> CastSharedPointer(std::shared_ptr<void> src, const ObjectTypeInfo& srcTypeInfo)
{
	if constexpr (IsInterface<To> == true)
	{
		bool isInherited = false;
		std::size_t offset = srcTypeInfo.template GetInterfaceOffsetOf<To>(isInherited);
		if (isInherited == true)
		{
			return std::shared_ptr<To>(
				src, reinterpret_cast<To*>(reinterpret_cast<std::size_t>(src.get()) + offset)
			);
		}
	}
	else if (srcTypeInfo.template IsChildOf<To>() == true)
	{
		return std::reinterpret_pointer_cast<To>(src);
	}
	return nullptr;
}

/**
 * 정수 및 실수 타입
 */
template<typename T, typename C>
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
struct TypeInfoResolver<void, void>
{
	static const VoidTypeInfo& Get()
	{
		return VoidTypeInfo::mStatic;
	}
};

/**
 * 열거형
 */
template<typename T>
struct TypeInfoResolver<T, std::enable_if_t<std::is_enum_v<T>>>
{
	static const EnumTypeInfo<T>& Get()
	{
		return EnumTypeInfo<T>::mStatic;
	}
};

/**
 * 클래스 및 구조체
 */
template<typename T>
struct TypeInfoResolver<T, std::enable_if_t<std::is_class_v<T> && HasSuper<T>>>
{
	static const auto& Get()
	{
		return T::GetStaticTypeInfo();
	}
};

/**
 * 포인터
 */
template<typename T> requires IsChildOfObject<T>
struct TypeInfoResolver<T*, void>
{
	static const PointerTypeInfo<T>& Get()
	{
		STATIC_ASSERT_MSG(IsChildOfObject<T> == true, "Allow only object pointer");
		return PointerTypeInfo<T>::mStatic;
	}
};

template<typename T> requires IsChildOfObject<T>
struct TypeInfoResolver<std::shared_ptr<T>, void>
{
	static const SharedPointerTypeInfo<T>& Get()
	{
		STATIC_ASSERT_MSG(IsChildOfObject<T> == true, "Allow only object pointer");
		return SharedPointerTypeInfo<T>::mStatic;
	}
};

template<typename T> requires IsChildOfObject<T>
struct TypeInfoResolver<std::weak_ptr<T>, void>
{
	static const WeakPointerTypeInfo<T>& Get()
	{
		STATIC_ASSERT_MSG(IsChildOfObject<T> == true, "Allow only object pointer");
		return WeakPointerTypeInfo<T>::mStatic;
	}
};

template<typename T> requires IsChildOfObject<T>
struct TypeInfoResolver<SubClass<T>, void>
{
	static const SubClassTypeInfo<T>& Get()
	{
		STATIC_ASSERT_MSG(IsChildOfObject<T> == true, "Allow only object pointer");
		return SubClassTypeInfo<T>::mStatic;
	}
};

template<typename T> requires IsChildOfObject<T>
struct TypeInfoResolver<SoftObjectPtr<T>, void>
{
	static const SoftRefTypeInfo<T>& Get()
	{
		return SoftRefTypeInfo<T>::mStatic;
	}
};

template<typename T> requires IsBulk<T>
struct TypeInfoResolver<std::shared_ptr<T>, void>
{
	static const BulkTypeInfo<T>& Get()
	{
		return BulkTypeInfo<T>::mStatic;
	}
};

/**
 * Pair
 */
template<typename K, typename D>
struct TypeInfoResolver<std::pair<K,D>, void>
{
	static const PairTypeInfo<K, D>& Get()
	{
		return PairTypeInfo<K, D>::mStatic;
	}
};

/**
 * 배열
 */
template<typename T, size_t N>
struct TypeInfoResolver<T[N], void>
{
	static const ArrayTypeInfo<T, N>& Get()
	{
		return ArrayTypeInfo<T, N>::mStatic;
	}
};

template<typename T, size_t N>
struct TypeInfoResolver<std::array<T, N>, void>
{
	static const ArrayContanierTypeInfo<T, N>& Get()
	{
		return ArrayContanierTypeInfo<T, N>::mStatic;
	}
};

/**
 * 어댑터를 제외한 동적 배열 및 문자열
 */
template<typename T>
struct TypeInfoResolver<T, std::enable_if_t<IsIteratorContanier<T> && UseAllocator<T>>>
{
	static const DynamicIterableContainerTypeInfo<IteratorElementType<T>, T>& Get()
	{
		return DynamicIterableContainerTypeInfo<IteratorElementType<T>, T>::mStatic;
	}
};

