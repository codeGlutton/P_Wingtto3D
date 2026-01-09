#pragma once

#include "Reflection/ObjectTypeInfo.h"
#include "Utils/TypeUtils.h"

/**
 * Template 회피를 위한 베이스 PropertyHandler 인터페이스
 */
class IPropertyHandlerBase abstract
{ 
	GEN_INTERFACE_REFLECTION(IPropertyHandlerBase)
};

/**
 * 조작 가능한 PropertyHandler 인터페이스
 */
template<typename T>
class IPropertyHandler abstract : public InterfaceReflector<IPropertyHandlerBase>
{
	GEN_INTERFACE_REFLECTION(IPropertyHandler<T>)

public:
	virtual const T& Get(void* object) const = 0;
	virtual void Set(void* object, T& value) const = 0;
	virtual void Set(void* object, T&& value) const = 0;
};

/**
 * 멤버 변수에 대한 PropertyHandler
 */
template<class C, typename P>
class PropertyHandler : public InterfaceReflector<IPropertyHandler<P>>
{
	GEN_REFLECTION(PropertyHandler<C,P>)

	// 멤버 변수 포인터
	using MemPtr = P C::*;

public:
	explicit PropertyHandler(MemPtr ptr) :
		_mPtr(ptr)
	{
	}

public:
	virtual const P& Get(void* object) const override
	{
		return static_cast<C*>(object)->*_mPtr;
	}
	virtual void Set(void* object, P& value) const override
	{
		static_cast<C*>(object)->*_mPtr = value;
	}
	virtual void Set(void* object, P&& value) const override
	{
		static_cast<C*>(object)->*_mPtr = std::move(value);
	}

private:
	MemPtr _mPtr = nullptr;
};

/**
 * 정적 변수에 대한 PropertyHandler
 */
template<class C, typename P>
class StaticPropertyHandler : public InterfaceReflector<IPropertyHandler<P>>
{
	GEN_REFLECTION(StaticPropertyHandler<C, P>)

	// 정적 변수 포인터
	using Ptr = P*;

public:
	explicit StaticPropertyHandler(Ptr ptr) :
		_mPtr(ptr)
	{
	}

public:
	virtual const P& Get([[maybe_unused]] void* object) const override
	{
		return *_mPtr;
	}
	virtual void Set([[maybe_unused]] void* object, P& value) const override
	{
		*_mPtr = value;
	}
	virtual void Set([[maybe_unused]] void* object, P&& value) const override
	{
		*_mPtr = std::move(value);
	}

private:
	Ptr _mPtr = nullptr;
};

/**
 * Property 등록 주체
 */
template<class C, typename P, typename Ptr, Ptr ptr>
class PropertyRegister
{
public:
	PropertyRegister(const char* name, ObjectTypeInfo& ownerTypeInfo);
};

/**
 * Property 생성 시 초기화 인자 구조체
 */
struct PropertyInitializer
{
	const char* mName = nullptr;
	const TypeInfo& mTypeInfo;
	const IPropertyHandlerBase& mHandler;
};

template <typename T>
class ReturnPropertyWrapper
{
public:
	ReturnPropertyWrapper() :
		_mValue(nullptr)
	{
		FAIL_MSG("Invalid return value");
	}

	ReturnPropertyWrapper(const T& value) :
		_mValue(&value)
	{
	}

public:
	const T& Get() const
	{
		return *_mValue;
	}

	operator const T& () const
	{
		return Get();
	}

private:
	const T* _mValue;
};

/**
 * Property 리플렉션 타입
 */
class Property
{
public:
	Property(ObjectTypeInfo& ownerTypeInfo, const PropertyInitializer& initializer) :
		_mName(initializer.mName),
		_mTypeInfo(initializer.mTypeInfo),
		_mHandler(initializer.mHandler)
	{
		ownerTypeInfo.AddProperty(this);
	}

public:
	template<typename T>
	ReturnPropertyWrapper<T> Get(void* object) const;

	template<typename T>
	void Set(void* object, T&& value) const;

public:
	const char* GetName() const
	{
		return _mName;
	}

private:
	const char* _mName = nullptr;
	const TypeInfo& _mTypeInfo;
	const IPropertyHandlerBase& _mHandler;
};

template<class C, typename P, typename Ptr, Ptr ptr>
inline PropertyRegister<C, P, Ptr, ptr>::PropertyRegister(const char* name, ObjectTypeInfo& ownerTypeInfo)
{
	STATIC_ASSERT_MSG(std::is_reference_v<P> == false, "Not allow ref value");

	// 멤버 변수
	if constexpr (std::is_member_pointer_v<Ptr> == true)
	{
		static PropertyHandler<C, P> handler(ptr);
		static PropertyInitializer initializer =
		{
			.mName = name,
			.mTypeInfo = TypeInfoResolver<P>::Get(),
			.mHandler = handler
		};
		static Property property(ownerTypeInfo, initializer);
	}
	// 정적 변수
	else
	{
		static StaticPropertyHandler<C, P> handler(ptr);
		static PropertyInitializer initializer =
		{
			.mName = name,
			.mTypeInfo = TypeInfoResolver<P>::Get(),
			.mHandler = handler
		};
		static Property property(ownerTypeInfo, initializer);
	}
}

template<typename T>
inline ReturnPropertyWrapper<T> Property::Get(void* object) const
{
	if (_mHandler.GetTypeInfo().IsChildOf<IPropertyHandler<T>>() == true)
	{
		auto handler = static_cast<const IPropertyHandler<T>*>(&_mHandler);
		return handler->Get(object);
	}
	else
	{
		FAIL_MSG("InValid casting for getting property value");
		return {};
	}
}

template<typename T>
inline void Property::Set(void* object, T&& value) const
{
	if (_mHandler.GetTypeInfo().IsChildOf<IPropertyHandler<std::remove_reference_t<T>>>() == true)
	{
		auto handler = static_cast<const IPropertyHandler<std::remove_reference_t<T>>*>(&_mHandler);
		handler->Set(object, std::forward<T>(value));
	}
	else
	{
		FAIL_MSG("InValid casting for setting property value");
	}
}
