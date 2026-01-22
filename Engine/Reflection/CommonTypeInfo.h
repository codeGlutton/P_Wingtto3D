#pragma once

#include "Reflection/TypeInfo.h"

#include "Core/Archive.h"

#include "Core/CommonType/SubClass.h"
#include "Core/CommonType/SoftObjectPtr.h"

// TODO : 
// 동적으로 에디터에서 바인딩하고 저장할 수 있도록
// 시그니처에 대한 타입 정보들 요구
//template<typename Ret, typename... Args>
//class DelegateTypeInfo : public TypeInfo

/**
 * 클래스 타입 정보 데이터
 */
template<typename T> requires IsChildOfObject<T>
class SubClassTypeInfo : public TypeInfo
{
	GEN_STRUCT_REFLECTION(SubClassTypeInfo<T>)

protected:
	SubClassTypeInfo() :
		TypeInfo(TypeInfoInitializer<SubClass<T>>(sizeof(SubClass<T>)))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const SubClassTypeInfo<T> mStatic;
};

/**
 * 약한 참조
 */
template<typename T> requires IsChildOfObject<T>
class SoftRefTypeInfo : public TypeInfo
{
	GEN_STRUCT_REFLECTION(SoftRefTypeInfo<T>)

protected:
	SoftRefTypeInfo() :
		TypeInfo(TypeInfoInitializer<SoftObjectPtr<T>>(sizeof(SoftObjectPtr<T>)))
	{
	}

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const SoftRefTypeInfo<T> mStatic;
};

/**
 * 벌크 타입
 */
class BulkBaseTypeInfo abstract : public TypeInfo
{
	GEN_STRUCT_REFLECTION(BulkBaseTypeInfo)

protected:
	template<typename T>
	BulkBaseTypeInfo(TypeInfoInitializer<T> initializer) :
		TypeInfo(initializer)
	{
	}

public:
	virtual bool SerializeBulkData(OUT Archive& archive, const void* inst) const = 0;
};

template<typename T> requires IsBulk<T>
class BulkTypeInfo : public BulkBaseTypeInfo
{
	GEN_STRUCT_REFLECTION(BulkTypeInfo<T>)

protected:
	BulkTypeInfo() :
		BulkBaseTypeInfo(TypeInfoInitializer<std::shared_ptr<T>>(sizeof(std::shared_ptr<T>)))
	{
	}

public:
	virtual bool SerializeBulkData(OUT Archive& archive, const void* inst) const override;

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	static const BulkTypeInfo<T> mStatic;
};

#include "CommonTypeInfo.inl"


