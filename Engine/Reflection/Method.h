#pragma once

#include "Reflection/ObjectTypeInfo.h"
#include "Utils/TypeUtils.h"

/**
 * Template 회피를 위한 베이스 MethodCaller 인터페이스
 */
class IMethodCallerBase abstract
{
	GEN_INTERFACE_REFLECTION(IMethodCallerBase)
};

/**
 * 호출 가능한 MethodCaller 인터페이스
 */
template<typename Ret, typename... Args>
class IMethodCaller abstract : public InterfaceReflector<IMethodCallerBase>
{
	GEN_INTERFACE_REFLECTION(IMethodCaller<Ret, Args...>)

public:
	virtual Ret Invoke(void* object, Args... args) const = 0;
};

/**
 * 멤버 함수에 대한 MethodCaller
 */
template<class C, typename Ret, typename... Args>
class MethodCaller : public InterfaceReflector<IMethodCaller<Ret, Args...>>
{
	GEN_ABSTRACT_REFLECTION(MethodCaller<C, Ret, Args...>)

	// 멤버 변수 포인터
	using MemFunc = Ret(C::*)(Args...);

public:
	MethodCaller() = default;
	explicit MethodCaller(MemFunc func) :
		_mFunc(func)
	{
	}

public:
	virtual Ret Invoke(void* object, Args... args) const override
	{
		if constexpr (std::same_as<Ret, void> == true)
		{
			(static_cast<C*>(object)->*_mFunc)(std::move(args)...);
		}
		else
		{
			return (static_cast<C*>(object)->*_mFunc)(std::move(args)...);
		}
	}

private:
	MemFunc _mFunc = nullptr;
};

/**
 * 멤버 상수 함수에 대한 MethodCaller
 */
template<class C, typename Ret, typename... Args>
class ConstMethodCaller : public InterfaceReflector<IMethodCaller<Ret, Args...>>
{
	GEN_ABSTRACT_REFLECTION(ConstMethodCaller<C, Ret, Args...>)

	// 멤버 변수 포인터
	using MemFuncConst = Ret(C::*)(Args...) const;

public:
	ConstMethodCaller() = default;
	explicit ConstMethodCaller(MemFuncConst func) :
		_mFunc(func)
	{
	}

public:
	virtual Ret Invoke(void* object, Args... args) const override
	{
		if constexpr (std::same_as<Ret, void> == true)
		{
			(static_cast<C*>(object)->*_mFunc)(std::move(args)...);
		}
		else
		{
			return (static_cast<C*>(object)->*_mFunc)(std::move(args)...);
		}
	}

private:
	MemFuncConst _mFunc = nullptr;
};

/**
 * 정적 변수에 대한 MethodCaller
 */
template<class C, typename Ret, typename... Args>
class StaticMethodCaller : public InterfaceReflector<IMethodCaller<Ret, Args...>>
{
	GEN_ABSTRACT_REFLECTION(StaticMethodCaller<C, Ret, Args...>)

	// 정적 변수 포인터
	using Func = Ret(*)(Args...);

public:
	StaticMethodCaller() = default;
	explicit StaticMethodCaller(Func func) :
		_mFunc(func)
	{
	}

public:
	virtual Ret Invoke([[maybe_unused]] void* object, Args... args) const override
	{
		if constexpr (std::same_as<Ret, void> == true)
		{
			(*_mFunc)(std::move(args)...);
		}
		else
		{
			return (*_mFunc)(std::move(args)...);
		}
	}

private:
	Func _mFunc = nullptr;
};

/**
 * Method 등록 주체
 */
template<class C>
class MethodRegister
{
public:
	template<typename Ret, typename... Args>
	MethodRegister(const char* name, ObjectTypeInfo& ownerTypeInfo, Ret(C::* func)(Args...));
	template<typename Ret, typename... Args>
	MethodRegister(const char* name, ObjectTypeInfo& ownerTypeInfo, Ret(C::* func)(Args...) const);
	template<typename Ret, typename... Args>
	MethodRegister(const char* name, ObjectTypeInfo& ownerTypeInfo, Ret(* func)(Args...));
};

/**
 * Method 생성 시 초기화 인자 구조체
 */
struct MethodInitializer
{
	const char* mName = nullptr;
	const TypeInfo& mRetTypeInfo;
	std::vector<const TypeInfo*> mParamTypeInfos;
	const IMethodCallerBase& mCaller;
};

/**
 * Method 리플렉션 타입
 */
class Method
{
public:
	Method(ObjectTypeInfo& ownerTypeInfo, const MethodInitializer& initializer) :
		_mName(initializer.mName),
		_mRetTypeInfo(initializer.mRetTypeInfo),
		_mParamTypeInfos(initializer.mParamTypeInfos),
		_mCaller(initializer.mCaller)
	{
		ownerTypeInfo.AddMethod(this);
	}

public:
	template<typename Ret = void, typename... InputArgs>
	Ret Invoke(void* object, InputArgs&&... args) const;

public:
	const char* GetName() const
	{
		return _mName;
	}

private:
	const char* _mName = nullptr;
	const TypeInfo& _mRetTypeInfo;
	std::vector<const TypeInfo*> _mParamTypeInfos;
	const IMethodCallerBase& _mCaller;
};

template<class C>
template<typename Ret, typename ...Args>
inline MethodRegister<C>::MethodRegister(const char* name, ObjectTypeInfo& ownerTypeInfo, Ret(C::* func)(Args...))
{
	STATIC_ASSERT_MSG(HasAnyReference<Args...> == false, "Not allow ref value");

	std::vector<const TypeInfo*> paramTypeInfos;
	paramTypeInfos.reserve(sizeof...(Args));
	(paramTypeInfos.push_back(&TypeInfoResolver<Args>::Get()), ...);

	static MethodCaller<C, Ret, Args...> caller(func);
	static MethodInitializer initializer =
	{
		.mName = name,
		.mRetTypeInfo = TypeInfoResolver<Ret>::Get(),
		.mParamTypeInfos = std::move(paramTypeInfos),
		.mCaller = caller
	};
	static Method method(ownerTypeInfo, initializer);
}

template<class C>
template<typename Ret, typename ...Args>
inline MethodRegister<C>::MethodRegister(const char* name, ObjectTypeInfo& ownerTypeInfo, Ret(C::* func)(Args...) const)
{
	STATIC_ASSERT_MSG(HasAnyReference<Args...> == false, "Not allow ref value");

	std::vector<const TypeInfo*> paramTypeInfos;
	paramTypeInfos.reserve(sizeof...(Args));
	(paramTypeInfos.push_back(&TypeInfoResolver<Args>::Get()), ...);

	static ConstMethodCaller<C, Ret, Args...> caller(func);
	static MethodInitializer initializer =
	{
		.mName = name,
		.mRetTypeInfo = TypeInfoResolver<Ret>::Get(),
		.mParamTypeInfos = std::move(paramTypeInfos),
		.mCaller = caller
	};
	static Method method(ownerTypeInfo, initializer);
}

template<class C>
template<typename Ret, typename ...Args>
inline MethodRegister<C>::MethodRegister(const char* name, ObjectTypeInfo& ownerTypeInfo, Ret(* func)(Args...))
{
	STATIC_ASSERT_MSG(HasAnyReference<Args...> == false, "Not allow ref value");

	std::vector<const TypeInfo*> paramTypeInfos;
	paramTypeInfos.reserve(sizeof...(Args));
	(paramTypeInfos.push_back(&TypeInfoResolver<Args>::Get()), ...);

	static StaticMethodCaller<C, Ret, Args...> caller(func);
	static MethodInitializer initializer =
	{
		.mName = name,
		.mRetTypeInfo = TypeInfoResolver<Ret>::Get(),
		.mParamTypeInfos = std::move(paramTypeInfos),
		.mCaller = caller
	};
	static Method method(ownerTypeInfo, initializer);
}

template<typename Ret, typename... InputArgs>
inline Ret Method::Invoke(void* object, InputArgs&&... args) const
{
	if (_mCaller.GetTypeInfo().IsChildOf<IMethodCaller<Ret, std::remove_reference_t<InputArgs>...>>() == true)
	{
		auto caller = static_cast<const IMethodCaller<Ret, std::remove_reference_t<InputArgs>...>*>(&_mCaller);
		if constexpr (std::same_as<Ret, void> == true)
		{
			caller->Invoke(object, std::forward<InputArgs>(args)...);
		}
		else
		{
			return caller->Invoke(object, std::forward<InputArgs>(args)...);
		}
	}
	else
	{
		FAIL_MSG("InValid casting for invoking property value");
		if constexpr (std::same_as<Ret, void> == true)
		{
			return;
		}
		else
		{
			return {};
		}
	}
}

