#pragma once

#include "Reflection/TypeInfo.h"

#include "Core/Archive.h"

#pragma region VOID

/**
 * void
 */
class VoidTypeInfo : public TypeInfo
{
protected:
	VoidTypeInfo() :
		TypeInfo(TypeInfoInitializer<void>())
	{
	}

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
class NumericTypeInfo : public TypeInfo
{
protected:
	NumericTypeInfo() :
		TypeInfo(TypeInfoInitializer<T>(sizeof(T)))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
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
class EnumTypeInfo : public TypeInfo
{
protected:
	EnumTypeInfo() :
		TypeInfo(TypeInfoInitializer<T>(sizeof(T)))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const EnumTypeInfo<T> mStatic;
};

#pragma endregion


#pragma region ARRAY

/**
 * 배열
 */
template<typename E, size_t N, typename C>
class ArrayTypeInfo : public TypeInfo
{
	using Element = E;

protected:
	ArrayTypeInfo() :
		TypeInfo(TypeInfoInitializer<C>(sizeof(E) * N))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const ArrayTypeInfo<E, N, C> mStatic;
};

#pragma endregion


#pragma region DYNAMIC_CONTAINER

/**
 * 동적 크기 타입
 */
template<typename E, typename C>
class DynamicContainerTypeInfo abstract : public TypeInfo
{
	using Element = E;

protected:
	DynamicContainerTypeInfo() :
		TypeInfo(TypeInfoInitializer<C>(sizeof(C)))
	{
	}
};

template<typename E, typename C>
class DynamicSequenceContainerTypeInfo : public DynamicContainerTypeInfo<E, C>
{
public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const DynamicSequenceContainerTypeInfo<E, C> mStatic;
};

#pragma endregion


#pragma region POINTER

/**
 * 포인터
 */
template<typename T> requires IsChildOfObject<T>
class PointerTypeInfo : public TypeInfo
{
protected:
	PointerTypeInfo() :
		TypeInfo(TypeInfoInitializer<T*>(sizeof(T*)))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const PointerTypeInfo<T> mStatic;
};

template<typename T> requires IsChildOfObject<T>
class SharedPointerTypeInfo : public TypeInfo
{
protected:
	SharedPointerTypeInfo() :
		TypeInfo(TypeInfoInitializer<std::shared_ptr<T>>(sizeof(std::shared_ptr<T>)))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const SharedPointerTypeInfo<T> mStatic;
};

template<typename T> requires IsChildOfObject<T>
class WeakPointerTypeInfo : public TypeInfo
{
protected:
	WeakPointerTypeInfo() :
		TypeInfo(TypeInfoInitializer<std::weak_ptr<T>>(sizeof(std::weak_ptr<T>)))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const WeakPointerTypeInfo<T> mStatic;
};

#pragma endregion

#include "DefaultTypeInfo.inl"
