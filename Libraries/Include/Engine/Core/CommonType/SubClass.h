#pragma once

template <typename T>
class SubClass;

/* 템플릿 객체인 SubClass<T>를 같은 서브클래스 타입인지를 알기 위해 */

template <typename T>
struct SubClassTypeTrait
{
	enum { Value = false };
};
template <typename T> 
struct SubClassTypeTrait<SubClass<T>>
{ 
	enum { Value = true }; 
};

template<typename T>
constexpr bool IsSubClassType = SubClassTypeTrait<std::remove_cv_t<T>>::Value;

template<typename T>
class SubClass
{
	template <typename U>
	friend class SubClass;

public:
	using Element = T;

public:
	SubClass() = default;
	SubClass(const SubClass&) = default;
	SubClass(SubClass&&) = default;
	~SubClass() = default;

public:
	SubClass(const ObjectTypeInfo* typeInfo) :
		_mTypeInfo(typeInfo)
	{
		ASSERT_MSG(typeInfo->IsChildOf<T>(), "Input type is not child of subclass T");
	}

	// ObjectTypeInfo 객체로 초기화
	template <typename U> requires (
		IsSubClassType<std::decay_t<U>> == false &&
		std::is_convertible_v<U, const ObjectTypeInfo*> == true
		)
	SubClass(U&& typeInfo) :
		_mTypeInfo(std::forward<U>(typeInfo))
	{
	}

	// 다른 상속된 타입의 SubClass 객체로 초기화
	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
	SubClass(const SubClass<U>& other) :
		_mTypeInfo(other._mTypeInfo)
	{
	}
	// 다른 상속된 타입의 SubClass 객체로 초기화
	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
		SubClass(SubClass<U>&& other) :
		_mTypeInfo(other._mTypeInfo)
	{
	}

public:
	SubClass& operator=(SubClass&&) = default;
	SubClass& operator=(const SubClass&) = default;

	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
	SubClass& operator=(const SubClass<U>& other)
	{
		_mTypeInfo = other._mTypeInfo;
		return *this;
	}

	SubClass& operator=(const ObjectTypeInfo* typeInfo)
	{
		_mTypeInfo = typeInfo;
		return *this;
	}

	template <typename U> requires(
		IsSubClassType<std::decay_t<U>> == false &&
		std::is_convertible_v<U, const ObjectTypeInfo*> == true
		)
	SubClass& operator=(U&& typeInfo)
	{
		_mTypeInfo = std::forward<U>(typeInfo);
		return *this;
	}

public:
	const ObjectTypeInfo* operator*() const
	{
		if (_mTypeInfo == nullptr || _mTypeInfo->IsChildOf(T::GetStaticTypeInfo()) == false)
		{
			return nullptr;
		}
		return _mTypeInfo;
	}

	const ObjectTypeInfo* Get() const
	{
		return **this;
	}

	const ObjectTypeInfo* operator->() const
	{
		return **this;
	}

	operator const ObjectTypeInfo* () const
	{
		return **this;
	}

private:
	const ObjectTypeInfo* _mTypeInfo = nullptr;
};
