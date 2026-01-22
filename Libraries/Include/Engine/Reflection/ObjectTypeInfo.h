#pragma once

#include "Reflection/StructTypeInfo.h"

class Object;
class ObjectTypeInfo;
class BulkBaseTypeInfo;

/**
 * 일반 객체 타입 정보 초기화 구조체
 * @tparam T 초기화 대상
 */
template<typename T>
struct ObjectTypeInfoInitializer : public StructTypeInfoInitializer<T>
{
	ObjectTypeInfoInitializer(const char* name) :
		StructTypeInfoInitializer<T>(name)
	{
		if constexpr (std::same_as<T::Interfaces, void> == false)
		{
			using TInterfacesType = T::Interfaces;

			auto infoArray = TInterfacesType::GetInterfaceInfos();
			mAdditionalInterfaces.insert(mAdditionalInterfaces.end(), infoArray.begin(), infoArray.end());
			auto offsetArray = TInterfacesType::template GetInterfaceOffsets<T>();
			mAdditionalInterfaceOffsets.insert(mAdditionalInterfaceOffsets.end(), offsetArray.begin(), offsetArray.end());
		}
	}

	ObjectTypeInfoInitializer(const char* name, std::function<Object*()> constructor) :
		StructTypeInfoInitializer<T>(name),
		mConstructor(constructor)
	{
		if constexpr (std::same_as<T::Interfaces, void> == false)
		{
			using TInterfacesType = T::Interfaces;

			auto infoArray = TInterfacesType::GetInterfaceInfos();
			mAdditionalInterfaces.insert(mAdditionalInterfaces.end(), infoArray.begin(), infoArray.end());
			auto offsetArray = TInterfacesType::template GetInterfaceOffsets<T>();
			mAdditionalInterfaceOffsets.insert(mAdditionalInterfaceOffsets.end(), offsetArray.begin(), offsetArray.end());
		}
	}

public:
	std::vector<const ObjectTypeInfo*> mAdditionalInterfaces;
	std::vector<std::size_t> mAdditionalInterfaceOffsets;

public:
	std::function<Object*()> mConstructor = nullptr;
};

template<template<typename...> class C, typename... Args>
struct ObjectTypeInfoInitializer<C<Args...>> : public StructTypeInfoInitializer<C<Args...>>
{
	using T = C<Args...>;

	ObjectTypeInfoInitializer(const char* name) :
		StructTypeInfoInitializer<T>(typeid(T).name())
	{
		if constexpr (std::same_as<T::Interfaces, void> == false)
		{
			using TInterfacesType = T::Interfaces;

			auto infoArray = TInterfacesType::GetInterfaceInfos();
			mAdditionalInterfaces.insert(mAdditionalInterfaces.end(), infoArray.begin(), infoArray.end());
			auto offsetArray = TInterfacesType::template GetInterfaceOffsets<T>();
			mAdditionalInterfaceOffsets.insert(mAdditionalInterfaceOffsets.end(), offsetArray.begin(), offsetArray.end());
		}
	}

	ObjectTypeInfoInitializer(const char* name, std::function<Object*()> constructor) :
		TypeInfoInitializer<T>(typeid(C<Args...>).name()),
		mConstructor(constructor)
	{
		if constexpr (std::same_as<T::Interfaces, void> == false)
		{
			using TInterfacesType = T::Interfaces;

			auto infoArray = TInterfacesType::GetInterfaceInfos();
			mAdditionalInterfaces.insert(mAdditionalInterfaces.end(), infoArray.begin(), infoArray.end());
			auto offsetArray = TInterfacesType::template GetInterfaceOffsets<T>();
			mAdditionalInterfaceOffsets.insert(mAdditionalInterfaceOffsets.end(), offsetArray.begin(), offsetArray.end());
		}
	}

public:
	std::vector<const ObjectTypeInfo*> mAdditionalInterfaces;
	std::vector<std::size_t> mAdditionalInterfaceOffsets;

public:
	std::function<Object* ()> mConstructor = nullptr;
};

/**
 * 일반 객체 타입 정보 클래스 (unreal의 uclass와 유사)
 */
class ObjectTypeInfo : public StructTypeInfo
{
	GEN_STRUCT_REFLECTION(ObjectTypeInfo)

	friend class Method;
	friend class Property;

protected:
	using MethodMap = std::unordered_map<std::string_view, const Method*>;
	using PropertyMap = typename StructTypeInfo::PropertyMap;

public:
	template <typename T>
	explicit ObjectTypeInfo(const ObjectTypeInfoInitializer<T>& initializer) :
		StructTypeInfo(initializer),
		_mConstructor(initializer.mConstructor),
		_mAdditionalInterfaces(initializer.mAdditionalInterfaces),
		_mAdditionalInterfaceOffsets(initializer.mAdditionalInterfaceOffsets)
	{
	}

public:
	/**
	 * 동적으로 동일 혹은 파생 타입 확인
	 * \param other 동일 및 부모 대상
	 * \return 동일 및 파생 여부
	 */
	using StructTypeInfo::IsChildOf;
	virtual bool IsChildOf(const StructTypeInfo& other) const override;

public:
	bool ImplementsInterface(const ObjectTypeInfo& other) const;

	template<typename T>
	bool ImplementsInterface() const;

public:
	std::size_t GetInterfaceOffsetOf(const ObjectTypeInfo& other, OUT bool& isInherited) const;

	template<typename T>
	std::size_t GetInterfaceOffsetOf(OUT bool& isImplemented) const;

public:
	virtual void Serialize(OUT Archive& archive, const void* inst) const override;
	virtual void Deserialize(Archive& archive, OUT void* inst) const override;

public:
	virtual const ObjectTypeInfo* GetSuper() const override
	{
		return reinterpret_cast<const ObjectTypeInfo*>(_mSuperInfo);
	}

	const std::function<Object*()>& GetConstructor() const
	{
		return _mConstructor;
	}

	const Method* GetMethod(const char* name) const;

protected:
	void AddMethod(const Method* method);

public:
	std::shared_ptr<const Object> GetDefaultObject() const;

protected:
	std::vector<const ObjectTypeInfo*> _mAdditionalInterfaces;
	std::vector<std::size_t> _mAdditionalInterfaceOffsets;

private:
	MethodMap _mMethodMap;
	std::vector<const Method*> _mMethods;

private:
	std::function<Object*()> _mConstructor = nullptr;

private:
	mutable std::shared_ptr<Object> _mDefaultObject = nullptr;
};

template<typename T>
inline bool ObjectTypeInfo::ImplementsInterface() const
{
	return ImplementsInterface(T::GetStaticTypeInfo());
}

template<typename T>
inline std::size_t ObjectTypeInfo::GetInterfaceOffsetOf(OUT bool& isImplemented) const
{
	return GetInterfaceOffsetOf(T::GetStaticTypeInfo(), isImplemented);
}

