#pragma once

#include "Reflection/TypeInfo.h"
#include "Utils/TypeUtils.h"

/**
 * 일반 객체 타입 정보 초기화 구조체
 * @tparam T 초기화 대상
 */
template<typename T>
struct ObjectTypeInfoInitializer : public TypeInfoInitializer<T>
{
	ObjectTypeInfoInitializer(const char* name) :
		TypeInfoInitializer<T>(),
		mName(name)
	{
		if constexpr (HasSuper<T> == true && std::same_as<typename T::Super, void> == false)
		{
			using TSuperType = typename T::Super;
			mSuperInfo = &(TSuperType::GetStaticTypeInfo());
		}
		if constexpr (IsSameSuperInterface<T> == false && std::same_as<typename T::Interfaces, void> == false)
		{
			using TInterfacesType = typename T::Interfaces;
			auto array = TInterfacesType::GetInterfaceInfos();
			mAdditionalInterfaces.insert(mAdditionalInterfaces.end(), array.begin(), array.end());
		}
	}

	const char* mName = nullptr;
	const ObjectTypeInfo* mSuperInfo = nullptr;
	std::vector<const ObjectTypeInfo*> mAdditionalInterfaces;
};

/**
 * 일반 객체 타입 정보 클래스 (unreal의 uclass와 유사)
 */
class ObjectTypeInfo : public TypeInfo
{
	friend class Method;
	friend class Property;

private:
	using MethodMap = std::unordered_map<std::string_view, const Method*>;
	using PropertyMap = std::unordered_map<std::string_view, const Property*>;

public:
	template <typename T>
	explicit ObjectTypeInfo(const ObjectTypeInfoInitializer<T>& initializer) :
		TypeInfo(initializer),
		_mName(initializer.mName),
		_mSuperInfo(initializer.mSuperInfo)
	{
		if (_mSuperInfo != nullptr)
		{
			_mInterfaces = _mSuperInfo->_mInterfaces;
		}
		for (const ObjectTypeInfo* interface : initializer.mAdditionalInterfaces)
		{
			_mInterfaces.push_back(interface);
		}
	}

public:
	const ObjectTypeInfo* GetSuper() const
	{
		return _mSuperInfo;
	}
	bool HasSuper() const
	{
		return _mSuperInfo != nullptr;
	}

public:
	/**
	 * 동적으로 동일 혹은 파생 타입 확인
	 * \param other 동일 및 부모 대상
	 * \return 동일 및 파생 여부
	 */
	bool IsChildOf(const ObjectTypeInfo& other) const;

	template<typename T>
	bool IsChildOf() const;

public:
	bool ImplementsInterface(const ObjectTypeInfo& other) const;

	template<typename T>
	bool ImplementsInterface() const;

public:
	const Method* GetMethod(const char* name) const;
	const Property* GetProperty(const char* name) const;

private:
	void AddMethod(const Method* method);
	void AddProperty(const Property* property);

private:
	const char* _mName = nullptr;

private:
	const ObjectTypeInfo* _mSuperInfo = nullptr;
	std::vector<const ObjectTypeInfo*> _mInterfaces;

private:
	MethodMap _mMethodMap;
	PropertyMap _mPropertyMap;

	std::vector<const Method*> _mMethods;
	std::vector<const Property*> _mProperties;
};

template<typename T>
inline bool ObjectTypeInfo::IsChildOf() const
{
	return IsChildOf(T::GetStaticTypeInfo());
}

template<typename T>
inline bool ObjectTypeInfo::ImplementsInterface() const
{
	return ImplementsInterface(T::GetStaticTypeInfo());
}

