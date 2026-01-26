#pragma once

#include "Utils/TypeUtils.h"
#include "DefaultTypeInfo.h"

#include "Core/Archive.h"
#include "Core/ObjectLinker.h"

#include "Utils/ObjectUtils.h"

template<typename T>
inline bool ComparableDefaultTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	return *reinterpret_cast<const T*>(lhsInst) == *reinterpret_cast<const T*>(rhsInst);
}

#pragma region NUMERIC

/**
 * 정수 및 실수
 */
template<typename T>
const NumericTypeInfo<T> NumericTypeInfo<T>::mStatic;

template<typename T>
void NumericTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	archive << *reinterpret_cast<const T*>(inst);
}

template<typename T>
void NumericTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	archive >> *reinterpret_cast<T*>(inst);
}

#pragma endregion


#pragma region ENUM

/**
 * 열거형
 */
template<typename T>
const EnumTypeInfo<T> EnumTypeInfo<T>::mStatic;

template<typename T>
void EnumTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	archive << *reinterpret_cast<const T*>(inst);
}

template<typename T>
void EnumTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	archive >> *reinterpret_cast<T*>(inst);
}

#pragma endregion


#pragma region ARRAY

/**
 * Pair
 */
template<typename K, typename D> requires (IsChildOfObject<K> == false && IsChildOfObject<D> == false)
const PairTypeInfo<K, D> PairTypeInfo< K, D>::mStatic;

template<typename K, typename D> requires (IsChildOfObject<K> == false && IsChildOfObject<D> == false)
inline void PairTypeInfo<K, D>::Serialize(OUT Archive& archive, const void* inst) const
{
	const TypeInfo& keyTypeInfo = TypeInfoResolver<std::remove_const_t<K>>::Get();
	const TypeInfo& dataTypeInfo = TypeInfoResolver<D>::Get();

	const C* instPtr = reinterpret_cast<const C*>(inst);
	keyTypeInfo.Serialize(archive, &instPtr->first);
	dataTypeInfo.Serialize(archive, &instPtr->second);
}

template<typename K, typename D> requires (IsChildOfObject<K> == false && IsChildOfObject<D> == false)
inline void PairTypeInfo<K, D>::Deserialize(Archive& archive, OUT void* inst) const
{
	const TypeInfo& keyTypeInfo = TypeInfoResolver<std::remove_const_t<K>>::Get();
	const TypeInfo& dataTypeInfo = TypeInfoResolver<D>::Get();

	C* instPtr = reinterpret_cast<C*>(inst);
	keyTypeInfo.Deserialize(archive, const_cast<std::remove_const_t<K>*>(&instPtr->first));
	dataTypeInfo.Deserialize(archive, &instPtr->second);
}

/**
 * 배열
 */
template<typename E, size_t N> requires (IsChildOfObject<E> == false)
const ArrayTypeInfo<E, N> ArrayTypeInfo<E, N>::mStatic;

template<typename E, size_t N> requires (IsChildOfObject<E> == false)
bool ArrayTypeInfo<E, N>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const E* lhsInstPtr = reinterpret_cast<const E*>(lhsInst);
	const E* rhsInstPtr = reinterpret_cast<const E*>(rhsInst);
	for (int32 i = 0; i < N; ++i)
	{
		if (*lhsInstPtr != *rhsInstPtr)
		{
			return false;
		}
		++lhsInstPtr;
		++rhsInstPtr;
	}
	return true;
}

template<typename E, size_t N> requires (IsChildOfObject<E> == false)
void ArrayTypeInfo<E, N>::Serialize(OUT Archive& archive, const void* inst) const
{
	const TypeInfo& typeInfo = TypeInfoResolver<E>::Get();

	const E* instPtr = reinterpret_cast<const E*>(inst);
	for (int32 i = 0; i < N; ++i)
	{
		typeInfo.Serialize(archive, instPtr);
		++instPtr;
	}
}

template<typename E, size_t N> requires (IsChildOfObject<E> == false)
void ArrayTypeInfo<E, N>::Deserialize(Archive& archive, OUT void* inst) const
{
	const TypeInfo& typeInfo = TypeInfoResolver<E>::Get();

	E* instPtr = reinterpret_cast<E*>(inst);
	for (int32 i = 0; i < N; ++i)
	{
		typeInfo.Deserialize(archive, instPtr);
		++instPtr;
	}
}

template<typename E, size_t N> requires (IsChildOfObject<E> == false)
const ArrayContanierTypeInfo<E, N> ArrayContanierTypeInfo<E, N>::mStatic;

template<typename E, size_t N> requires (IsChildOfObject<E> == false)
inline void ArrayContanierTypeInfo<E, N>::Serialize(OUT Archive& archive, const void* inst) const
{
	const TypeInfo& typeInfo = TypeInfoResolver<E>::Get();

	const C& instRef = *reinterpret_cast<const C*>(inst);
	for (const E& element : instRef)
	{
		typeInfo.Serialize(archive, &element);
	}
}

template<typename E, size_t N> requires (IsChildOfObject<E> == false)
inline void ArrayContanierTypeInfo<E, N>::Deserialize(Archive& archive, OUT void* inst) const
{
	const TypeInfo& typeInfo = TypeInfoResolver<E>::Get();

	C& instRef = *reinterpret_cast<C*>(inst);
	for (E& element : instRef)
	{
		typeInfo.Deserialize(archive, &element);
	}
}

#pragma endregion


#pragma region DYNAMIC_CONTAINER

/**
 * 동적 크기 타입
 */
template<typename E, typename C> requires (IsChildOfObject<E> == false)
const DynamicIterableContainerTypeInfo<E, C> DynamicIterableContainerTypeInfo<E, C>::mStatic;

template<typename E, typename C> requires (IsChildOfObject<E> == false)
void DynamicIterableContainerTypeInfo<E, C>::Serialize(OUT Archive& archive, const void* inst) const
{
	const TypeInfo& typeInfo = TypeInfoResolver<E>::Get();

	const C& containerRef = *reinterpret_cast<const C*>(inst);
	
	std::size_t containerSize = std::size(containerRef);
	TypeInfoResolver<std::size_t>::Get().Serialize(archive, &containerSize);

	for (const E& element : containerRef)
	{
		typeInfo.Serialize(archive, &element);
	}
}

template<typename E, typename C> requires (IsChildOfObject<E> == false)
void DynamicIterableContainerTypeInfo<E, C>::Deserialize(Archive& archive, OUT void* inst) const
{
	const TypeInfo& typeInfo = TypeInfoResolver<E>::Get();

	C& containerRef = *reinterpret_cast<C*>(inst);

	std::size_t containerSize;
	TypeInfoResolver<std::size_t>::Get().Deserialize(archive, &containerSize);

	containerRef.clear();
	auto inserter = std::inserter(containerRef, std::end(containerRef));
	for (std::size_t i = 0; i < containerSize; ++i)
	{
		E element{};
		typeInfo.Deserialize(archive, &element);
		*inserter++ = std::move(element);
	}
}

#pragma endregion


#pragma region POINTER

/**
 * 포인터
 */

template<typename T> requires IsChildOfObject<T>
const PointerTypeInfo<T> PointerTypeInfo<T>::mStatic;

template<typename T> requires IsChildOfObject<T>
inline bool PointerTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	return *reinterpret_cast<const T* const*>(lhsInst) == *reinterpret_cast<const T* const*>(rhsInst);
}

template<typename T> requires IsChildOfObject<T>
void PointerTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	const T* const& instRef = *reinterpret_cast<const T* const*>(inst);

	// 패키징 명 쓰기
	const std::wstring* packagePath = GetObjectPath(reinterpret_cast<Object*>(GetInstancePackage(instRef).get()));
	TypeInfoResolver<std::wstring>::Get().Serialize(archive, packagePath);

	// 풀 주소 쓰기
	const std::wstring* objectFullPath = GetObjectFullPath(reinterpret_cast<const Object*>(instRef));
	TypeInfoResolver<std::wstring>::Get().Serialize(archive, objectFullPath);
}

template<typename T> requires IsChildOfObject<T>
void PointerTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	T*& instRef = *reinterpret_cast<T**>(inst);

	// 패키징 명 읽기
	std::wstring packagePath;
	TypeInfoResolver<std::wstring>::Get().Deserialize(archive, &packagePath);

	// 풀 주소 읽기
	std::wstring objectFullPath;
	TypeInfoResolver<std::wstring>::Get().Deserialize(archive, &objectFullPath);

	// 리소스에 대입
	std::shared_ptr<Object> objectPtr = archive.GetObjectLinker()->mLinkDataMap[packagePath].mObjectPtrMap[objectFullPath];
	instRef = objectPtr.get();
}

template<typename T> requires IsChildOfObject<T>
inline std::shared_ptr<Package> PointerTypeInfo<T>::GetInstancePackage(const void* inst) const
{
	const Object* const& instRef = *reinterpret_cast<const T* const*>(inst);
	if (instRef == nullptr)
	{
		return std::shared_ptr<Package>();
	}
	return GetOuter(instRef);
}

template<typename T> requires IsChildOfObject<T>
const SharedPointerTypeInfo<T> SharedPointerTypeInfo<T>::mStatic;

template<typename T> requires IsChildOfObject<T>
inline bool SharedPointerTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const std::shared_ptr<T>& lhsInstRef = *reinterpret_cast<const std::shared_ptr<T>*>(lhsInst);
	const std::shared_ptr<T>& rhsInstRef = *reinterpret_cast<const std::shared_ptr<T>*>(rhsInst);
	return lhsInstRef == rhsInstRef;
}

template<typename T> requires IsChildOfObject<T>
void SharedPointerTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	const std::shared_ptr<T>& instRef = *reinterpret_cast<const std::shared_ptr<T>*>(inst);

	// 패키징 명 쓰기
	const std::wstring* packagePath = GetObjectPath(reinterpret_cast<Object*>(GetInstancePackage(instRef.get()).get()));
	TypeInfoResolver<std::wstring>::Get().Serialize(archive, packagePath);

	// 풀 주소 쓰기
	const std::wstring* objectFullPath = GetObjectFullPath(reinterpret_cast<Object*>(instRef.get()));
	TypeInfoResolver<std::wstring>::Get().Serialize(archive, objectFullPath);
}

template<typename T> requires IsChildOfObject<T>
void SharedPointerTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	std::shared_ptr<T>& instRef = *reinterpret_cast<std::shared_ptr<T>*>(inst);

	// 패키징 명 읽기
	std::wstring packagePath;
	TypeInfoResolver<std::wstring>::Get().Deserialize(archive, &packagePath);

	// 풀 주소 읽기
	std::wstring objectFullPath;
	TypeInfoResolver<std::wstring>::Get().Deserialize(archive, &objectFullPath);

	// 리소스에 대입
	std::shared_ptr<Object> objectPtr = archive.GetObjectLinker()->mLinkDataMap[packagePath].mObjectPtrMap[objectFullPath];
	instRef = std::reinterpret_pointer_cast<T>(objectPtr);
}

template<typename T> requires IsChildOfObject<T>
inline std::shared_ptr<Package> SharedPointerTypeInfo<T>::GetInstancePackage(const void* inst) const
{
	const std::shared_ptr<Object> instRef = *reinterpret_cast<const std::shared_ptr<T>*>(inst);
	if (instRef == nullptr)
	{
		return std::shared_ptr<Package>();
	}
	return GetOuter(instRef.get());
}

template<typename T> requires IsChildOfObject<T>
const WeakPointerTypeInfo<T> WeakPointerTypeInfo<T>::mStatic;

template<typename T> requires IsChildOfObject<T>
inline bool WeakPointerTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const std::weak_ptr<T>& lhsInstRef = *reinterpret_cast<const std::weak_ptr<T>*>(lhsInst);
	const std::weak_ptr<T>& rhsInstRef = *reinterpret_cast<const std::weak_ptr<T>*>(rhsInst);
	return lhsInstRef.lock() == rhsInstRef.lock();
}

template<typename T> requires IsChildOfObject<T>
void WeakPointerTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	const std::shared_ptr<T> sharedInstRef = (*reinterpret_cast<const std::weak_ptr<T>*>(inst)).lock();

	// 패키징 명 쓰기
	const std::wstring* packagePath = GetObjectPath(reinterpret_cast<Object*>(GetInstancePackage(sharedInstRef.get()).get()));
	TypeInfoResolver<std::wstring>::Get().Serialize(archive, packagePath);

	// 풀 주소 쓰기
	const std::wstring* objectFullPath = GetObjectFullPath(reinterpret_cast<Object*>(sharedInstRef.get()));
	TypeInfoResolver<std::wstring>::Get().Serialize(archive, objectFullPath);
}

template<typename T> requires IsChildOfObject<T>
void WeakPointerTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	std::weak_ptr<T>& instRef = *reinterpret_cast<std::weak_ptr<T>*>(inst);

	// 패키징 명 읽기
	std::wstring packagePath;
	TypeInfoResolver<std::wstring>::Get().Deserialize(archive, &packagePath);

	// 풀 주소 읽기
	std::wstring objectFullPath;
	TypeInfoResolver<std::wstring>::Get().Deserialize(archive, &objectFullPath);

	// 리소스에 대입
	std::shared_ptr<Object> objectPtr = archive.GetObjectLinker()->mLinkDataMap[packagePath].mObjectPtrMap[objectFullPath];
	instRef = std::reinterpret_pointer_cast<T>(objectPtr);
}

template<typename T> requires IsChildOfObject<T>
inline std::shared_ptr<Package> WeakPointerTypeInfo<T>::GetInstancePackage(const void* inst) const
{
	const std::shared_ptr<Object> instRef = (*reinterpret_cast<const std::weak_ptr<T>*>(inst)).lock();
	if (instRef == nullptr)
	{
		return std::shared_ptr<Package>();
	}
	return GetOuter(instRef.get());
}

#pragma endregion

