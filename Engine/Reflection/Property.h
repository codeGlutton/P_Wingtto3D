#pragma once

#include "Reflection/StructTypeInfo.h"
#include "Utils/TypeUtils.h"

/**
 * Template 회피를 위한 베이스 PropertyHandler 인터페이스
 */
class IPropertyHandlerBase abstract
{ 
	GEN_INTERFACE_REFLECTION(IPropertyHandlerBase)

public:
	virtual const void* GetRawPtr(const void* object) const = 0;
	virtual void* GetRawPtr(void* object) const = 0;

	virtual void SetRawPtr(void* object, const void* value) const = 0;
};

/**
 * 조작 가능한 PropertyHandler 인터페이스
 */
template<typename T>
class IPropertyHandler abstract : public InterfaceReflector<IPropertyHandlerBase>
{
	GEN_INTERFACE_REFLECTION(IPropertyHandler<T>)

public:
	virtual const T& Get(const void* object) const = 0;
	virtual void Set(void* object, T& value) const = 0;
	virtual void Set(void* object, T&& value) const = 0;
};

/**
 * 멤버 변수에 대한 PropertyHandler
 */
template<class C, typename P>
class PropertyHandler : public InterfaceReflector<IPropertyHandler<P>>
{
	GEN_ABSTRACT_REFLECTION(PropertyHandler<C, P>)

	// 멤버 변수 포인터
	using MemPtr = P C::*;

public:
	PropertyHandler() = default;
	explicit PropertyHandler(MemPtr ptr) :
		_mPtr(ptr)
	{
	}

public:
	virtual const void* GetRawPtr(const void* object) const override
	{
		return &(static_cast<const C*>(object)->*_mPtr);
	}
	virtual void* GetRawPtr(void* object) const override
	{
		return &(static_cast<C*>(object)->*_mPtr);
	}
	virtual void SetRawPtr(void* object, const void* value) const override
	{
		static_cast<C*>(object)->*_mPtr = *reinterpret_cast<const P*>(value);
	}

public:
	virtual const P& Get(const void* object) const override
	{
		return static_cast<const C*>(object)->*_mPtr;
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

template<class C, typename M, std::size_t N>
class PropertyHandler<C, M[N]> : public InterfaceReflector<IPropertyHandler<M[N]>>
{
	GEN_ABSTRACT_REFLECTION(PropertyHandler<C, M[N]>)

	using P = M[N];
	using MemPtr = P C::*;

public:
	PropertyHandler() = default;
	explicit PropertyHandler(MemPtr ptr) :
		_mPtr(ptr)
	{
	}

public:
	virtual const void* GetRawPtr(const void* object) const override
	{
		return &(static_cast<const C*>(object)->*_mPtr);
	}
	virtual void* GetRawPtr(void* object) const override
	{
		return &(static_cast<C*>(object)->*_mPtr);
	}
	virtual void SetRawPtr(void* object, const void* value) const override
	{
		P& memRef = static_cast<C*>(object)->*_mPtr;
		const P& valueRef = *reinterpret_cast<const P*>(value);
		std::copy(std::begin(valueRef), std::end(valueRef), std::begin(memRef));
	}

public:
	virtual const P& Get(const void* object) const override
	{
		return static_cast<const C*>(object)->*_mPtr;
	}
	virtual void Set(void* object, P& value) const override
	{
		P& memRef = static_cast<C*>(object)->*_mPtr;
		std::copy(std::begin(value), std::end(value), std::begin(memRef));
	}
	virtual void Set(void* object, P&& value) const override
	{
		P& memRef = static_cast<C*>(object)->*_mPtr;
		std::copy(std::begin(value), std::end(value), std::begin(memRef));
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
	GEN_ABSTRACT_REFLECTION(StaticPropertyHandler<C, P>)

	// 정적 변수 포인터
	using Ptr = P*;

public:
	StaticPropertyHandler() = default;
	explicit StaticPropertyHandler(Ptr ptr) :
		_mPtr(ptr)
	{
	}

public:
	virtual const void* GetRawPtr([[maybe_unused]] const void* object) const override
	{
		return _mPtr;
	}
	virtual void* GetRawPtr([[maybe_unused]] void* object) const override
	{
		return _mPtr;
	}
	virtual void SetRawPtr([[maybe_unused]] void* object, const void* value) const override
	{
		*_mPtr = *reinterpret_cast<const P*>(value);
	}

public:
	virtual const P& Get([[maybe_unused]] void* object) const override
	{
		return *_mPtr;
	}
	virtual void Set([[maybe_unused]] const void* object, P& value) const override
	{
		*_mPtr = value;
	}
	virtual void Set([[maybe_unused]] const void* object, P&& value) const override
	{
		*_mPtr = std::move(value);
	}

private:
	Ptr _mPtr = nullptr;
};

template<class C, typename M, std::size_t N>
class StaticPropertyHandler<C, M[N]> : public InterfaceReflector<IPropertyHandler<M[N]>>
{
	GEN_ABSTRACT_REFLECTION(StaticPropertyHandler<C, M[N]>)

	using P = M[N];
	using Ptr = P*;

public:
	StaticPropertyHandler() = default;
	explicit StaticPropertyHandler(Ptr ptr) :
		_mPtr(ptr)
	{
	}

public:
	virtual const void* GetRawPtr([[maybe_unused]] const void* object) const override
	{
		return _mPtr;
	}
	virtual void* GetRawPtr([[maybe_unused]] void* object) const override
	{
		return _mPtr;
	}
	virtual void SetRawPtr([[maybe_unused]] void* object, const void* value) const override
	{
		const P& valueRef = *reinterpret_cast<const P*>(value);
		std::copy(std::begin(valueRef), std::end(valueRef), std::begin(_mPtr));
	}

public:
	virtual const P& Get([[maybe_unused]] void* object) const override
	{
		return *_mPtr;
	}
	virtual void Set([[maybe_unused]] const void* object, P& value) const override
	{
		std::copy(std::begin(value), std::end(value), std::begin(_mPtr));
	}
	virtual void Set([[maybe_unused]] const void* object, P&& value) const override
	{
		std::copy(std::begin(value), std::end(value), std::begin(_mPtr));
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
	PropertyRegister(const char* name, StructTypeInfo& ownerTypeInfo);
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
	Property(StructTypeInfo& ownerTypeInfo, const PropertyInitializer& initializer) :
		_mName(initializer.mName),
		_mTypeInfo(initializer.mTypeInfo),
		_mHandler(initializer.mHandler)
	{
		ownerTypeInfo.AddProperty(this);
	}

public:
	bool IsEqual(const void* lhsObject, const void* rhsObject) const
	{
		return _mTypeInfo.IsInstanceValueEqual(GetRawPtr(lhsObject), GetRawPtr(rhsObject));
	}

	const void* GetRawPtr(const void* object) const
	{
		return _mHandler.GetRawPtr(object);
	}
	void* GetRawPtr(void* object) const
	{
		return _mHandler.GetRawPtr(object);
	}
	void SetRawPtr(void* object, const void* value) const
	{
		_mHandler.SetRawPtr(object, value);
	}

public:
	template<typename T>
	ReturnPropertyWrapper<T> Get(const void* object) const;

	template<typename T>
	void Set(void* object, T&& value) const;

public:
	const char* GetName() const
	{
		return _mName;
	}
	const TypeInfo& GetTypeInfo() const
	{
		return _mTypeInfo;
	}

private:
	const char* _mName = nullptr;
	const TypeInfo& _mTypeInfo;
	const IPropertyHandlerBase& _mHandler;
};

template<class C, typename P, typename Ptr, Ptr ptr>
inline PropertyRegister<C, P, Ptr, ptr>::PropertyRegister(const char* name, StructTypeInfo& ownerTypeInfo)
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
inline ReturnPropertyWrapper<T> Property::Get(const void* object) const
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
