#pragma once

#include "Reflection/TypeInfo.h"

class Archive;
class Package;

template<typename T>
class DefaultTypeInfo abstract : public TypeInfo
{
	GEN_STRUCT_REFLECTION(DefaultTypeInfo<T>)

protected:
	DefaultTypeInfo() :
		TypeInfo(TypeInfoInitializer<T>(sizeof(T)))
	{
	}
};

template<>
class DefaultTypeInfo<void> abstract : public TypeInfo
{
	GEN_STRUCT_REFLECTION(DefaultTypeInfo<void>)

protected:
	DefaultTypeInfo() :
		TypeInfo(TypeInfoInitializer<void>(sizeof(0)))
	{
	}
};

template<typename T>
class ComparableDefaultTypeInfo abstract : public DefaultTypeInfo<T>
{
	GEN_STRUCT_REFLECTION(ComparableDefaultTypeInfo<T>)

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
};

#pragma region VOID

/**
 * void
 */
class VoidTypeInfo : public DefaultTypeInfo<void>
{
	GEN_STRUCT_REFLECTION(VoidTypeInfo)

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override
	{
		FAIL_MSG("Void type cannot be compared");
		return false;
	}

	virtual void Serialize(OUT Archive& archive, const void* inst) const override
	{
		FAIL_MSG("Void type cannot be serialized");
	}

	virtual void Deserialize(Archive& archive, OUT void* inst) const override
	{
		FAIL_MSG("Void type cannot be deserialized");
	}

public:
	static const VoidTypeInfo mStatic;
};

#pragma endregion


#pragma region NUMERIC

/**
 * 정수 및 실수
 */
template<typename T>
class NumericTypeInfo : public ComparableDefaultTypeInfo<T>
{
	GEN_STRUCT_REFLECTION(NumericTypeInfo)

public:
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const NumericTypeInfo<T> mStatic;
};

#pragma endregion


#pragma region ENUM

/**
 * 열거형
 */
template<typename T>
class EnumTypeInfo : public ComparableDefaultTypeInfo<T>
{
	GEN_STRUCT_REFLECTION(EnumTypeInfo<T>)

public:
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const EnumTypeInfo<T> mStatic;
};

#pragma endregion


#pragma region ARRAY

/**
 * Pair
 */
template<typename K, typename D>
class PairTypeInfo : public ComparableDefaultTypeInfo<std::pair<K, D>>
{
	GEN_STRUCT_REFLECTION(PairTypeInfo<K, D>)

private:
	using C = std::pair<K,D>;

public:
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const PairTypeInfo<K, D> mStatic;
};

/**
 * 배열
 */
template<typename E, size_t N>
class ArrayTypeInfo : public DefaultTypeInfo<E[N]>
{
	GEN_STRUCT_REFLECTION(ArrayTypeInfo<E, N>)

private:
	using C = E[N];
	using Element = E;

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const ArrayTypeInfo<E, N> mStatic;
};

template<typename E, size_t N>
class ArrayContanierTypeInfo : public ComparableDefaultTypeInfo<std::array<E, N>>
{
	GEN_STRUCT_REFLECTION(ArrayContanierTypeInfo<E, N>)

private:
	using C = std::array<E, N>;
	using Element = E;

public:
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const ArrayContanierTypeInfo<E, N> mStatic;
};

#pragma endregion


#pragma region DYNAMIC_CONTAINER

/**
 * 동적 크기 타입
 */
template<typename E, typename C>
class DynamicIterableContainerTypeInfo : public ComparableDefaultTypeInfo<C>
{
	GEN_STRUCT_REFLECTION(DynamicIterableContainerTypeInfo<E, C>)

public:
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const DynamicIterableContainerTypeInfo<E, C> mStatic;
};

#pragma endregion


#pragma region POINTER

class HardRefTypeInfo abstract : public TypeInfo
{
	GEN_STRUCT_REFLECTION(HardRefTypeInfo)

protected:
	template<typename T>
	HardRefTypeInfo(const TypeInfoInitializer<T>& initializer) :
		TypeInfo(initializer)
	{
	}

public:
	virtual std::shared_ptr<Package> GetInstancePackage(const void* inst) const = 0;
};

/**
 * 포인터
 */
template<typename T>
class PointerTypeInfo : public HardRefTypeInfo
{
	GEN_STRUCT_REFLECTION(PointerTypeInfo<T>)

protected:
	PointerTypeInfo() :
		HardRefTypeInfo(TypeInfoInitializer<T*>(sizeof(T*)))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	virtual std::shared_ptr<Package> GetInstancePackage(const void* inst) const override;

public:
	static const PointerTypeInfo<T> mStatic;
};

template<typename T>
class SharedPointerTypeInfo : public HardRefTypeInfo
{
	GEN_STRUCT_REFLECTION(SharedPointerTypeInfo<T>)

protected:
	SharedPointerTypeInfo() :
		HardRefTypeInfo(TypeInfoInitializer<std::shared_ptr<T>>(sizeof(std::shared_ptr<T>)))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	virtual std::shared_ptr<Package> GetInstancePackage(const void* inst) const override;

public:
	static const SharedPointerTypeInfo<T> mStatic;
};

template<typename T>
class WeakPointerTypeInfo : public HardRefTypeInfo
{
	GEN_STRUCT_REFLECTION(WeakPointerTypeInfo<T>)

protected:
	WeakPointerTypeInfo() :
		HardRefTypeInfo(TypeInfoInitializer<std::weak_ptr<T>>(sizeof(std::weak_ptr<T>)))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	virtual std::shared_ptr<Package> GetInstancePackage(const void* inst) const override;

public:
	static const WeakPointerTypeInfo<T> mStatic;
};

#pragma endregion

#include "DefaultTypeInfo.inl"

