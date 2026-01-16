#pragma once

#include "Utils/TypeUtils.h"

#pragma region NUMERIC

/**
 * 정수 및 실수
 */
template<typename T>
const NumericTypeInfo<T> NumericTypeInfo<T>::mStatic;

template<typename T>
bool NumericTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	return *reinterpret_cast<const T*>(lhsInst) == *reinterpret_cast<const T*>(rhsInst);
}

template<typename T>
void NumericTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	archive << *reinterpret_cast<const T*>(inst);
}

template<typename T>
void NumericTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	//archive->Read(*reinterpret_cast<const T*>(inst));
}

#pragma endregion


#pragma region ENUM

/**
 * 열거형
 */
template<typename T>
const EnumTypeInfo<T> EnumTypeInfo<T>::mStatic;

template<typename T>
bool EnumTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	return *reinterpret_cast<const T*>(lhsInst) == *reinterpret_cast<const T*>(rhsInst);
}

template<typename T>
void EnumTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	archive << *reinterpret_cast<const T*>(inst);
}

template<typename T>
void EnumTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	//archive->Read(*reinterpret_cast<const T*>(inst));
}

#pragma endregion


#pragma region ARRAY

/**
 * 배열
 */
template<typename E, size_t N, typename C>
const ArrayTypeInfo<E, N, C> ArrayTypeInfo<E, N, C>::mStatic;

template<typename E, size_t N, typename C>
bool ArrayTypeInfo<E, N, C>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const TypeInfo& typeInfo = TypeInfoResolver<E>::Get();

	const E* lhsInstPtr = reinterpret_cast<const E*>(lhsInst);
	const E* rhsInstPtr = reinterpret_cast<const E*>(rhsInst);
	for (int32 i = 0; i < N; ++i)
	{
		if (typeInfo.IsInstanceValueEqual(lhsInstPtr, rhsInstPtr) == false)
		{
			return false;
		}
		++lhsInstPtr;
		++rhsInstPtr;
	}
	return true;
}

template<typename E, size_t N, typename C>
void ArrayTypeInfo<E, N, C>::Serialize(OUT Archive& archive, const void* inst) const
{
	const TypeInfo& typeInfo = TypeInfoResolver<E>::Get();

	const E* instPtr = reinterpret_cast<const E*>(inst);
	for (int32 i = 0; i < N; ++i)
	{
		typeInfo.Serialize(archive, instPtr);
		++instPtr;
	}
}

template<typename E, size_t N, typename C>
void ArrayTypeInfo<E, N, C>::Deserialize(Archive& archive, OUT void* inst) const
{
	// TODO
}

#pragma endregion


#pragma region DYNAMIC_CONTAINER

/**
 * 동적 크기 타입
 */
template<typename E, typename C>
const DynamicSequenceContainerTypeInfo<E, C> DynamicSequenceContainerTypeInfo<E, C>::mStatic;

template<typename E, typename C>
bool DynamicSequenceContainerTypeInfo<E, C>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const TypeInfo& typeInfo = TypeInfoResolver<E>::Get();

	const C& lhsContainerPtr = *reinterpret_cast<const C*>(lhsInst);
	const C& rhsContainerPtr = *reinterpret_cast<const C*>(rhsInst);

	if (std::size(lhsContainerPtr) != std::size(rhsContainerPtr))
	{
		return false;
	}

	auto lhsIter = std::begin(lhsContainerPtr);
	auto rhsIter = std::begin(rhsContainerPtr);
	auto lhsIterEnd = std::end(lhsContainerPtr);
	while (lhsIter != lhsIterEnd)
	{
		if (typeInfo.IsInstanceValueEqual(&(*lhsIter), &(*rhsIter)) == false)
		{
			return false;
		}
		++lhsIter;
		++rhsIter;
	}
	return true;
}

template<typename E, typename C>
void DynamicSequenceContainerTypeInfo<E, C>::Serialize(OUT Archive& archive, const void* inst) const
{
	const TypeInfo& typeInfo = TypeInfoResolver<E>::Get();

	const C& containerPtr = *reinterpret_cast<const C*>(inst);
	archive << std::size(containerPtr);
	for (const E& element : containerPtr)
	{
		typeInfo.Serialize(archive, &element);
	}
}

template<typename E, typename C>
void DynamicSequenceContainerTypeInfo<E, C>::Deserialize(Archive& archive, OUT void* inst) const
{
	// TODO
}

#pragma endregion


#pragma region POINTER

/**
 * 포인터
 */

template<typename T> requires IsChildOfObject<T>
const PointerTypeInfo<T> PointerTypeInfo<T>::mStatic;

template<typename T> requires IsChildOfObject<T>
bool PointerTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const T* lhs = *reinterpret_cast<T* const*>(lhsInst);
	const T* rhs = *reinterpret_cast<T* const*>(rhsInst);

	return lhs == rhs;
}

template<typename T> requires IsChildOfObject<T>
void PointerTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	ObjectGUID id = archive.ConvertGUID(*reinterpret_cast<T* const*>(inst));
	archive << id;
}

template<typename T> requires IsChildOfObject<T>
void PointerTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	// TODO
}

template<typename T> requires IsChildOfObject<T>
const SharedPointerTypeInfo<T> SharedPointerTypeInfo<T>::mStatic;

template<typename T> requires IsChildOfObject<T>
bool SharedPointerTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const std::shared_ptr<T>& lhs = *reinterpret_cast<const std::shared_ptr<T>*>(lhsInst);
	const std::shared_ptr<T>& rhs = *reinterpret_cast<const std::shared_ptr<T>*>(rhsInst);

	return lhs == rhs;
}

template<typename T> requires IsChildOfObject<T>
void SharedPointerTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	const std::shared_ptr<T>& instPtr = *reinterpret_cast<const std::shared_ptr<T>*>(inst);
	ObjectGUID id = archive.ConvertGUID(instPtr.get());
	archive << id;
}

template<typename T> requires IsChildOfObject<T>
void SharedPointerTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	// TODO
}

template<typename T> requires IsChildOfObject<T>
const WeakPointerTypeInfo<T> WeakPointerTypeInfo<T>::mStatic;

template<typename T> requires IsChildOfObject<T>
bool WeakPointerTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const std::shared_ptr<T> lhs = (*reinterpret_cast<const std::weak_ptr<T>*>(lhsInst)).lock();
	const std::shared_ptr<T> rhs = (*reinterpret_cast<const std::weak_ptr<T>*>(rhsInst)).lock();

	return lhs == rhs;
}

template<typename T> requires IsChildOfObject<T>
void WeakPointerTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	const std::shared_ptr<T> instPtr = (*reinterpret_cast<const std::weak_ptr<T>*>(inst)).lock();
	ObjectGUID id = archive.ConvertGUID(instPtr.get());
	archive << id;
}

template<typename T> requires IsChildOfObject<T>
void WeakPointerTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	// TODO
}

#pragma endregion

