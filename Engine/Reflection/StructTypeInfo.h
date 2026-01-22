#pragma once

#include "Reflection/TypeInfo.h"

class Object;
class StructTypeInfo;
struct BulkData;

/**
 * 일반 객체 타입 정보 초기화 구조체
 * @tparam T 초기화 대상
 */
template<typename T>
struct StructTypeInfoInitializer : public TypeInfoInitializer<T>
{
	StructTypeInfoInitializer(const char* name) :
		TypeInfoInitializer<T>(),
		mName(name)
	{
		if constexpr (HasSuper<T> == true && std::same_as<typename T::Super, void> == false)
		{
			using TSuperType = typename T::Super;
			mSuperInfo = &(TSuperType::GetStaticTypeInfo());
		}
	}

public:
	const char* mName = nullptr;
	const StructTypeInfo* mSuperInfo = nullptr;
};

template<template<typename...> class S, typename... Args>
struct StructTypeInfoInitializer<S<Args...>> : public TypeInfoInitializer<S<Args...>>
{
	using T = S<Args...>;

	StructTypeInfoInitializer(const char* name) :
		TypeInfoInitializer<T>(),
		mName(typeid(T).name())
	{
		if constexpr (HasSuper<T> == true && std::same_as<typename T::Super, void> == false)
		{
			using TSuperType = typename T::Super;
			mSuperInfo = &(TSuperType::GetStaticTypeInfo());
		}
	}

public:
	const char* mName = nullptr;
	const StructTypeInfo* mSuperInfo = nullptr;
};

/**
 * 구조체 타입 정보 클래스 (unreal의 ustruct 유사)
 */
class StructTypeInfo : public TypeInfo
{
	GEN_STRUCT_REFLECTION(StructTypeInfo)

	friend class Property;

protected:
	using PropertyMap = std::unordered_map<std::string_view, const Property*>;

public:
	template <typename T>
	explicit StructTypeInfo(const StructTypeInfoInitializer<T>& initializer) :
		TypeInfo(initializer),
		_mName(initializer.mName),
		_mSuperInfo(initializer.mSuperInfo)
	{
	}

public:
	/**
	 * 동적으로 동일 혹은 파생 타입 확인
	 * \param other 동일 및 부모 대상
	 * \return 동일 및 파생 여부
	 */
	virtual bool IsChildOf(const StructTypeInfo& other) const;

	template<typename T>
	bool IsChildOf() const;

public:
	using TypeInfo::IsA;
	template<typename T>
	bool IsA() const;

public:
	void CollectHeaderDatas(const void* inst, OUT std::unordered_map<std::wstring, std::string> externalPackageDatas, OUT std::vector<std::shared_ptr<BulkData>>& bulkDatas) const;

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const override;
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	const char* GetName() const
	{
		return _mName;
	}

	virtual const StructTypeInfo* GetSuper() const
	{
		return _mSuperInfo;
	}
	bool HasSuper() const
	{
		return _mSuperInfo != nullptr;
	}

	const Property* GetProperty(const char* name) const;

protected:
	void AddProperty(const Property* property);

protected:
	const char* _mName = nullptr;

protected:
	const StructTypeInfo* _mSuperInfo = nullptr;

protected:
	PropertyMap _mPropertyMap;
	std::vector<const Property*> _mProperties;
};

template<typename T>
inline bool StructTypeInfo::IsChildOf() const
{
	return IsChildOf(T::GetStaticTypeInfo());
}

template<typename T>
inline bool StructTypeInfo::IsA() const
{
	return IsA(T::GetStaticTypeInfo());
}
