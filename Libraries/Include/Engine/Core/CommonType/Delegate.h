#pragma once

#define DECLARE_DELEGATE_INTERNAL(name, ...) using name = NativeSingleDelegate<__VA_ARGS__>

#define DECLARE_DELEGATE(name)													DECLARE_DELEGATE_INTERNAL(name, void)
#define DECLARE_DELEGATE_1_PARAM(name, _1)										DECLARE_DELEGATE_INTERNAL(name, void, _1)
#define DECLARE_DELEGATE_2_PARAMS(name, _1, _2)									DECLARE_DELEGATE_INTERNAL(name, void, _1, _2)
#define DECLARE_DELEGATE_3_PARAMS(name, _1, _2, _3)								DECLARE_DELEGATE_INTERNAL(name, void, _1, _2, _3)
#define DECLARE_DELEGATE_4_PARAMS(name, _1, _2, _3, _4)							DECLARE_DELEGATE_INTERNAL(name, void, _1, _2, _3, _4)
#define DECLARE_DELEGATE_5_PARAMS(name, _1, _2, _3, _4, _5)						DECLARE_DELEGATE_INTERNAL(name, void, _1, _2, _3, _4, _5)

#define DECLARE_DELEGATE_RET(name, ret)											DECLARE_DELEGATE_INTERNAL(name, ret)
#define DECLARE_DELEGATE_RET_1_PARAM(name, ret, _1)								DECLARE_DELEGATE_INTERNAL(name, ret, _1)
#define DECLARE_DELEGATE_RET_2_PARAMS(name, ret, _1, _2)						DECLARE_DELEGATE_INTERNAL(name, ret, _1, _2)
#define DECLARE_DELEGATE_RET_3_PARAMS(name, ret, _1, _2, _3)					DECLARE_DELEGATE_INTERNAL(name, ret, _1, _2, _3)
#define DECLARE_DELEGATE_RET_4_PARAMS(name, ret, _1, _2, _3, _4)				DECLARE_DELEGATE_INTERNAL(name, ret, _1, _2, _3, _4)
#define DECLARE_DELEGATE_RET_5_PARAMS(name, ret, _1, _2, _3, _4, _5)			DECLARE_DELEGATE_INTERNAL(name, ret, _1, _2, _3, _4, _5)

#define DECLARE_MULTICAST_DELEGATE_INTERNAL(name, ...) using name = NativeMulticastDelegate<__VA_ARGS__>

#define DECLARE_MULTICAST_DELEGATE(name)										DECLARE_MULTICAST_DELEGATE_INTERNAL(name)
#define DECLARE_MULTICAST_DELEGATE_1_PARAM(name, _1)							DECLARE_MULTICAST_DELEGATE_INTERNAL(name, _1)
#define DECLARE_MULTICAST_DELEGATE_2_PARAMS(name, _1, _2)						DECLARE_MULTICAST_DELEGATE_INTERNAL(name, _1, _2)
#define DECLARE_MULTICAST_DELEGATE_3_PARAMS(name, _1, _2, _3)					DECLARE_MULTICAST_DELEGATE_INTERNAL(name, _1, _2, _3)
#define DECLARE_MULTICAST_DELEGATE_4_PARAMS(name, _1, _2, _3, _4)				DECLARE_MULTICAST_DELEGATE_INTERNAL(name, _1, _2, _3, _4)
#define DECLARE_MULTICAST_DELEGATE_5_PARAMS(name, _1, _2, _3, _4, _5)			DECLARE_MULTICAST_DELEGATE_INTERNAL(name, _1, _2, _3, _4, _5)

#define DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, ...) using name = DynamicSingleDelegate<__VA_ARGS__>

#define DECLARE_DYNAMIC_DELEGATE(name)											DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, void)
#define DECLARE_DYNAMIC_DELEGATE_1_PARAM(name, _1)								DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, void, _1)
#define DECLARE_DYNAMIC_DELEGATE_2_PARAMS(name, _1, _2)							DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, void, _1, _2)
#define DECLARE_DYNAMIC_DELEGATE_3_PARAMS(name, _1, _2, _3)						DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, void, _1, _2, _3)
#define DECLARE_DYNAMIC_DELEGATE_4_PARAMS(name, _1, _2, _3, _4)					DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, void, _1, _2, _3, _4)
#define DECLARE_DYNAMIC_DELEGATE_5_PARAMS(name, _1, _2, _3, _4, _5)				DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, void, _1, _2, _3, _4, _5)

#define DECLARE_DYNAMIC_DELEGATE_RET(name, ret)									DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, ret)
#define DECLARE_DYNAMIC_DELEGATE_RET_1_PARAM(name, ret, _1)						DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, ret, _1)
#define DECLARE_DYNAMIC_DELEGATE_RET_2_PARAMS(name, ret, _1, _2)				DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, ret, _1, _2)
#define DECLARE_DYNAMIC_DELEGATE_RET_3_PARAMS(name, ret, _1, _2, _3)			DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, ret, _1, _2, _3)
#define DECLARE_DYNAMIC_DELEGATE_RET_4_PARAMS(name, ret, _1, _2, _3, _4)		DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, ret, _1, _2, _3, _4)
#define DECLARE_DYNAMIC_DELEGATE_RET_5_PARAMS(name, ret, _1, _2, _3, _4, _5)	DECLARE_DYNAMIC_DELEGATE_INTERNAL(name, ret, _1, _2, _3, _4, _5)

class Object;
using DelegateHandle = uint32;
using DynamicDelegateHandle = std::pair<std::weak_ptr<Object>, std::string>;

#pragma region NATIVE_DELEGATE_BASE

class NativeDelegateBase
{
protected:
	DelegateHandle GetCurrentHandle() const
	{
		return _mHandle;
	}
	DelegateHandle GetNewHandle() const
	{
		return ++_mHandle;
	}

protected:
	mutable bool _mIsCalled = false;

private:
	mutable DelegateHandle _mHandle = 0ull;
};

#pragma endregion

#pragma region NATIVE_DELEGATE_BINDER

template<typename Ret, typename... Args> requires (!HasAnyReference<Ret, Args...>)
class NativeSingleDelegate;

template<typename... Args> requires (!HasAnyReference<Args...>)
class NativeMulticastDelegate;

template<typename Ret, typename... Args> requires (!HasAnyReference<Ret, Args...>)
class NativeDelegateBinder
{
	friend class NativeSingleDelegate<Ret, Args...>;
	friend class NativeMulticastDelegate<Args...>;

private:
	NativeDelegateBinder(std::function<Ret(const std::shared_ptr<Object>&, Args...)> callback, bool isWeakPtr, bool isStatic) :
		_mCallback(callback),
		_mIsWeakPtr(isWeakPtr),
		_mIsStatic(isStatic)
	{
	}

public:
	static NativeDelegateBinder BindLambda(std::function<Ret(Args...)> lambda)
	{
		return NativeDelegateBinder(
			[lambda](const std::shared_ptr<Object>& sp, Args... args) {
				if constexpr (std::is_same_v<Ret, void> == true)
				{
					lambda(std::move(args)...);
				}
				else
				{
					return lambda(std::move(args)...);
				}
			}, false, true);
	}
	static NativeDelegateBinder BindStatic(Ret(*func)(Args...))
	{
		return NativeDelegateBinder(
			[func](const std::shared_ptr<Object>& sp, Args... args) {
				if constexpr (std::is_same_v<Ret, void> == true)
				{
					*func(std::move(args)...);
				}
				else
				{
					return *func(std::move(args)...);
				}
			}, false, true);
	}

	template<typename C>
	static NativeDelegateBinder BindNativeMethod(Ret(C::* func)(Args...), bool isWeakPtr = false)
	{
		return NativeDelegateBinder(
			[func](const std::shared_ptr<Object>& sp, Args... args) {
				if constexpr (std::is_same_v<Ret, void> == true)
				{
					(static_cast<C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					return (static_cast<C*>(sp.get())->*func)(std::move(args)...);
				}
			}, isWeakPtr, false);
	}
	template<typename C>
	static NativeDelegateBinder BindNativeMethod(Ret(C::* func)(Args...) const, bool isWeakPtr = false)
	{
		return NativeDelegateBinder(
			[func](const std::shared_ptr<Object>& sp, Args... args) {
				if constexpr (std::is_same_v<Ret, void> == true)
				{
					(static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					return (static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
			}, isWeakPtr, false);
	}

	static NativeDelegateBinder BindMethod(const Method* method, bool isWeakPtr = false)
	{
		return NativeDelegateBinder(
			[method](const std::shared_ptr<Object>& sp, Args... args) {
				if constexpr (std::is_same_v<Ret, void> == true)
				{
					method->Invoke(sp.get(), std::move(args)...);
				}
				else
				{
					return method->Invoke(sp.get(), std::move(args)...);
				}
			}, isWeakPtr, false);
	}

private:
	std::function<Ret(const std::shared_ptr<Object>&, Args...)> _mCallback;
	bool _mIsWeakPtr = false;
	bool _mIsStatic = false;
};

#pragma endregion

#pragma region NATIVE_SINGLE_DELEGATE

template<typename Ret, typename... Args> requires (!HasAnyReference<Ret, Args...>)
class NativeSingleDelegate : public NativeDelegateBase
{
public:
	using Binder = NativeDelegateBinder<Ret, Args...>;

public:
	template<typename C>
	DelegateHandle Bind(const std::shared_ptr<C>& object, const Binder& binder);

	DelegateHandle BindLambda(std::function<Ret(Args...)> lambda)
	{
		_mCallback = lambda;
		return GetNewHandle();
	}
	DelegateHandle BindStatic(Ret(*func)(Args...))
	{
		_mCallback = func;
		return GetNewHandle();
	}

	template<typename C>
	DelegateHandle BindNativeMethod(const std::shared_ptr<C>& object, Ret(C::* func)(Args...), bool isWeakPtr = false);
	template<typename C>
	DelegateHandle BindNativeMethod(const std::shared_ptr<C>& object, Ret(C::* func)(Args...) const, bool isWeakPtr = false);
	template<typename C>
	DelegateHandle BindNativeMethod(const std::shared_ptr<const C>& object, Ret(C::* func)(Args...) const, bool isWeakPtr = false);

	template<typename C>
	DelegateHandle BindMethod(const std::shared_ptr<C>& object, const Method* method, bool isWeakPtr = false);

	bool Unbind(DelegateHandle handle);

public:
	bool IsBound() const
	{
		return _mCallback != nullptr;
	}

	void Clear()
	{
		_mCallback = nullptr;
	}

public:
	template<typename... InputArgs>
	Ret Execute(InputArgs&&... args) const;
	template<typename... InputArgs>
	Ret ExecuteIfBound(InputArgs&&... args) const;

protected:
	mutable std::function<Ret(Args...)> _mCallback;
};

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename C>
inline DelegateHandle NativeSingleDelegate<Ret, Args...>::Bind(const std::shared_ptr<C>& object, const Binder& binder)
{
	if (binder._mIsWeakPtr == true)
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			_mCallback = [wp = std::weak_ptr<C>(object), binder, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr || binder._mIsStatic == true)
				{
					binder._mCallback(sp, std::move(args)...);
				}
				else
				{
					Clear();
				}
				};
		}
		else
		{
			_mCallback = [wp = std::weak_ptr<C>(object), binder, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr || binder._mIsStatic == true)
				{
					return binder._mCallback(sp, std::move(args)...);
				}
				else
				{
					Clear();
					return {};
				}
				};
		}
	}
	else
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			_mCallback = [sp = object, binder](Args... args) {
				if (sp != nullptr || binder._mIsStatic == true)
				{
					binder._mCallback(sp, std::move(args)...);
				}
				};
		}
		else
		{
			_mCallback = [sp = object, binder](Args... args) {
				if (sp != nullptr || binder._mIsStatic == true)
				{
					return binder._mCallback(sp, std::move(args)...);
				}
				return {};
				};
		}
	}
	return GetNewHandle();
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename C>
inline DelegateHandle NativeSingleDelegate<Ret, Args...>::BindNativeMethod(const std::shared_ptr<C>& object, Ret(C::* func)(Args...), bool isWeakPtr)
{
	if (isWeakPtr == true)
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			_mCallback = [wp = std::weak_ptr<C>(object), func, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr)
				{
					(static_cast<C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					Clear();
				}
			};
		}
		else
		{
			_mCallback = [wp = std::weak_ptr<C>(object), func, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr)
				{
					return (static_cast<C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					Clear();
					return {};
				}
			};
		}
	}
	else
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			_mCallback = [sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					(static_cast<C*>(sp.get())->*func)(std::move(args)...);
				}
				};
		}
		else
		{
			_mCallback = [sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					return (static_cast<C*>(sp.get())->*func)(std::move(args)...);
				}
				return {};
				};
		}
	}
	return GetNewHandle();
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename C>
inline DelegateHandle NativeSingleDelegate<Ret, Args...>::BindNativeMethod(const std::shared_ptr<C>& object, Ret(C::* func)(Args...) const, bool isWeakPtr)
{
	if (isWeakPtr == true)
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			_mCallback = [wp = std::weak_ptr<C>(object), func, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr)
				{
					(static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					Clear();
				}
			};
		}
		else
		{
			_mCallback = [wp = std::weak_ptr<C>(object), func, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr)
				{
					return (static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					Clear();
					return {};
				}
			};
		}
	}
	else
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			_mCallback = [sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					(static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
			};
		}
		else
		{
			_mCallback = [sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					return (static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
				return {};
			};
		}
	}
	return GetNewHandle();
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename C>
inline DelegateHandle NativeSingleDelegate<Ret, Args...>::BindNativeMethod(const std::shared_ptr<const C>& object, Ret(C::* func)(Args...) const, bool isWeakPtr)
{
	if (isWeakPtr == true)
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			_mCallback = [wp = std::weak_ptr<const C>(object), func, this](Args... args) {
				std::shared_ptr<const C> sp = wp.lock();
				if (sp != nullptr)
				{
					(static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					Clear();
				}
				};
		}
		else
		{
			_mCallback = [wp = std::weak_ptr<const C>(object), func, this](Args... args) {
				std::shared_ptr<const C> sp = wp.lock();
				if (sp != nullptr)
				{
					return (static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					Clear();
					return {};
				}
				};
		}
	}
	else
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			_mCallback = [sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					(static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
				};
		}
		else
		{
			_mCallback = [sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					return (static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
				return {};
				};
		}
	}
	return GetNewHandle();
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename C>
inline DelegateHandle NativeSingleDelegate<Ret, Args...>::BindMethod(const std::shared_ptr<C>& object, const Method* method, bool isWeakPtr)
{
	if (isWeakPtr == true)
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			_mCallback = [wp = std::weak_ptr<C>(object), method, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr)
				{
					method->Invoke(sp.get(), args...);
				}
				else
				{
					Clear();
				}
			};
		}
		else
		{
			_mCallback = [wp = std::weak_ptr<C>(object), method, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr)
				{
					return method->Invoke(sp.get(), args...);
				}
				else
				{
					Clear();
					return {};
				}
			};
		}
	}
	else
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			_mCallback = [sp = object, method](Args... args) {
				if (sp != nullptr)
				{
					method->Invoke(sp.get(), args...);
				}
			};
		}
		else
		{
			_mCallback = [sp = object, method](Args... args) {
				if (sp != nullptr)
				{
					return method->Invoke(sp.get(), args...);
				}
				return {};
			};
		}
	}
	return GetNewHandle();
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
inline bool NativeSingleDelegate<Ret, Args...>::Unbind(DelegateHandle handle)
{
	if (GetCurrentHandle() == handle && IsBound() == true)
	{
		Clear();
		return true;
	}
	return false;
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename ...InputArgs>
inline Ret NativeSingleDelegate<Ret, Args...>::Execute(InputArgs && ...args) const
{
	ASSERT_MSG(_mIsCalled == false, "Not safe delegate was called circularly");
	_mIsCalled = true;

	if constexpr (std::is_same_v<Ret, void> == true)
	{
		_mCallback(std::forward<InputArgs>(args)...);
		_mIsCalled = false;
	}
	else
	{
		Ret ret = _mCallback(std::forward<InputArgs>(args)...);
		_mIsCalled = false;
		return ret;
	}
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename ...InputArgs>
inline Ret NativeSingleDelegate<Ret, Args...>::ExecuteIfBound(InputArgs && ...args) const
{
	if (_mCallback == nullptr)
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			return;
		}
		else
		{
			return {};
		}
	}

	ASSERT_MSG(_mIsCalled == false, "Not safe delegate was called circularly");
	_mIsCalled = true;

	if constexpr (std::is_same_v<Ret, void> == true)
	{
		_mCallback(std::forward<InputArgs>(args)...);
		_mIsCalled = false;
	}
	else
	{
		Ret ret = _mCallback(std::forward<InputArgs>(args)...);
		_mIsCalled = false;
		return ret;
	}
}

#pragma endregion

#pragma region NATIVE_MULTICAST_DELEGATE

template<typename... Args> requires (!HasAnyReference<Args...>)
class NativeMulticastDelegate : public NativeDelegateBase
{
public:
	using Binder = NativeDelegateBinder<void, Args...>;

public:
	struct CallbackDesc
	{
		DelegateHandle mHandle;
		std::function<void(Args...)> mCallback;
	};

public:
	template<typename C>
	DelegateHandle Bind(const std::shared_ptr<C>& object, const Binder& binder);

	DelegateHandle BindLambda(std::function<void(Args...)> lambda)
	{
		_mCallbackDescs.push_back(CallbackDesc{ GetNewHandle(), lambda });
		return GetCurrentHandle();
	}
	DelegateHandle BindStatic(void(*func)(Args...))
	{
		_mCallbackDescs.push_back(CallbackDesc{ GetNewHandle(), func });
		return GetCurrentHandle();
	}

	template<typename C>
	DelegateHandle BindNativeMethod(const std::shared_ptr<C>& object, void(C::* func)(Args...), bool isWeakPtr = false);
	template<typename C>
	DelegateHandle BindNativeMethod(const std::shared_ptr<C>& object, void(C::* func)(Args...) const, bool isWeakPtr = false);
	template<typename C>
	DelegateHandle BindNativeMethod(const std::shared_ptr<const C>& object, void(C::* func)(Args...) const, bool isWeakPtr = false);

	template<typename C>
	DelegateHandle BindMethod(const std::shared_ptr<C>& object, const Method* method, bool isWeakPtr = false);

	bool Unbind(DelegateHandle handle);

public:
	bool IsBound() const
	{
		return _mCallbackDescs.empty() == false;
	}

	void Clear()
	{
		_mCallbackDescs.clear();
	}

public:
	template<typename... InputArgs>
	void Multicast(InputArgs&&... args) const;

protected:
	mutable std::vector<CallbackDesc> _mCallbackDescs;
	mutable bool _mIsDirty = false;
};

template<typename ...Args> requires (!HasAnyReference<Args...>)
template<typename C>
inline DelegateHandle NativeMulticastDelegate<Args...>::Bind(const std::shared_ptr<C>& object, const Binder& binder)
{
	if (binder._mIsWeakPtr == true)
	{
		_mCallbackDescs.push_back({
			GetNewHandle(),
			[wp = std::weak_ptr<C>(object), binder, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr || binder._mIsStatic == true)
				{
					binder._mCallback(sp, std::move(args)...);
				}
				else
				{
					_mIsDirty = true;
				}
			}
			});
	}
	else
	{
		_mCallbackDescs.push_back({
			GetNewHandle(),
			[sp = object, binder](Args... args) {
				if (sp != nullptr || binder._mIsStatic == true)
				{
					binder._mCallback(sp, std::move(args)...);
				}
			}
			});
	}
	return GetCurrentHandle();
}

template<typename ...Args> requires (!HasAnyReference<Args...>)
template<typename C>
inline DelegateHandle NativeMulticastDelegate<Args...>::BindNativeMethod(const std::shared_ptr<C>& object, void(C::* func)(Args...), bool isWeakPtr)
{
	if (isWeakPtr == true)
	{
		_mCallbackDescs.push_back({
			GetNewHandle(),
			[wp = std::weak_ptr<C>(object), func, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr)
				{
					(static_cast<C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					_mIsDirty = true;
				}
			}
		});
	}
	else
	{
		_mCallbackDescs.push_back({
			GetNewHandle(),
			[sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					(static_cast<C*>(sp.get())->*func)(std::move(args)...);
				}
			}
		});
	}
	return GetCurrentHandle();
}

template<typename ...Args> requires (!HasAnyReference<Args...>)
template<typename C>
inline DelegateHandle NativeMulticastDelegate<Args...>::BindNativeMethod(const std::shared_ptr<C>& object, void(C::* func)(Args...) const, bool isWeakPtr)
{
	if (isWeakPtr == true)
	{
		_mCallbackDescs.push_back({
			GetNewHandle(),
			[wp = std::weak_ptr<C>(object), func, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr)
				{
					(static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					_mIsDirty = true;
				}
			}
		});
	}
	else
	{
		_mCallbackDescs.push_back({
			GetNewHandle(),
			[sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					(static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
			}
		});
	}
	return GetCurrentHandle();
}

template<typename ...Args> requires (!HasAnyReference<Args...>)
template<typename C>
inline DelegateHandle NativeMulticastDelegate<Args...>::BindNativeMethod(const std::shared_ptr<const C>& object, void(C::* func)(Args...) const, bool isWeakPtr)
{
	if (isWeakPtr == true)
	{
		_mCallbackDescs.push_back({
			GetNewHandle(),
			[wp = std::weak_ptr<const C>(object), func, this](Args... args) {
				std::shared_ptr<const C> sp = wp.lock();
				if (sp != nullptr)
				{
					(static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
				else
				{
					_mIsDirty = true;
				}
			}
			});
	}
	else
	{
		_mCallbackDescs.push_back({
			GetNewHandle(),
			[sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					(static_cast<const C*>(sp.get())->*func)(std::move(args)...);
				}
			}
			});
	}
	return GetCurrentHandle();
}

template<typename ...Args> requires (!HasAnyReference<Args...>)
template<typename C>
inline DelegateHandle NativeMulticastDelegate<Args...>::BindMethod(const std::shared_ptr<C>& object, const Method* method, bool isWeakPtr)
{
	if (isWeakPtr == true)
	{
		_mCallbackDescs.push_back({
			GetNewHandle(),
			[wp = std::weak_ptr<C>(object), method, this](Args... args) {
				std::shared_ptr<C> sp = wp.lock();
				if (sp != nullptr)
				{
					method->Invoke(sp.get(), args...);
				}
				else
				{
					_mIsDirty = true;
				}
			}
		});
	}
	else
	{
		_mCallbackDescs.push_back({
			GetNewHandle(),
			[sp = object, method](Args... args) {
				if (sp != nullptr)
				{
					method->Invoke(sp.get(), args...);
				}
			}
		});
	}
	return GetCurrentHandle();
}

template<typename ...Args> requires (!HasAnyReference<Args...>)
inline bool NativeMulticastDelegate<Args...>::Unbind(DelegateHandle handle)
{
	for (auto iter = _mCallbackDescs.begin(); iter != _mCallbackDescs.end(); ++iter)
	{
		if (handle == iter->mHandle)
		{
			_mCallbackDescs.erase(iter);
			return true;
		}
	}
	return false;
}

template<typename... Args> requires (!HasAnyReference<Args...>)
template<typename... InputArgs>
inline void NativeMulticastDelegate<Args...>::Multicast(InputArgs&& ...args) const
{
	ASSERT_MSG(_mIsCalled == false, "Not safe delegate was called circularly");
	_mIsCalled = true;

	_mIsDirty = false;
	for (auto iter = _mCallbackDescs.begin(); iter != _mCallbackDescs.end();)
	{
		iter->mCallback(std::forward<InputArgs>(args)...);
		if (_mIsDirty == true)
		{
			iter = _mCallbackDescs.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	_mIsCalled = false;
}

#pragma endregion


#pragma region DYNAMIC_DELEGATE_BASE

class DynamicDelegateBase
{
protected:
	mutable bool _mIsCalled = false;
};

#pragma endregion

#pragma region DYNAMIC_DELEGATE_BINDER

#define BIND_DYNAMIC(object, func) BindMethod(object, #func)

template<typename Ret, typename... Args> requires (!HasAnyReference<Ret, Args...>)
class DynamicSingleDelegate;

template<typename Ret, typename... Args> requires (!HasAnyReference<Ret, Args...>)
class DynamicDelegateBinder
{
	friend class DynamicSingleDelegate<Ret, Args...>;

private:
	DynamicDelegateBinder(std::shared_ptr<Object> object, const std::string& methodName) :
		_mBinding({object, methodName})
	{
	}

public:
	static DynamicDelegateBinder BindMethod(std::shared_ptr<Object> object, const std::string& methodName)
	{
		return DynamicDelegateBinder(object, methodName);
	}
	static DynamicDelegateBinder BindMethod(std::shared_ptr<Object> object, const Method* method)
	{
		return DynamicDelegateBinder(object, method->GetName());
	}

private:
	mutable std::pair<std::shared_ptr<Object>, std::string> _mBinding;
};

#pragma endregion

#pragma region DYNAMIC_SINGLE_DELEGATE

template<typename Ret, typename... Args> requires (!HasAnyReference<Ret, Args...>)
class DynamicSingleDelegate : public DynamicDelegateBase
{
	GEN_MINIMUM_STRUCT_REFLECTION(DynamicSingleDelegate<Ret, Args...>)

public:
	using Binder = DynamicDelegateBinder<Ret, Args...>;

public:
	DynamicDelegateHandle Bind(const Binder& binder);

	template<typename C>
	DynamicDelegateHandle BindMethod(const std::shared_ptr<C>& object, const Method* method);
	template<typename C>
	DynamicDelegateHandle BindMethod(const std::shared_ptr<C>& object, const char* methodName);

	bool Unbind(DynamicDelegateHandle handle);
	bool Unbind(std::shared_ptr<Object> object);

public:
	bool IsBound() const
	{
		return _mBinding.first != nullptr;
	}

	void Clear()
	{
		_mBinding.first.reset();
		_mBinding.second.clear();
	}

public:
	template<typename... InputArgs>
	Ret Execute(InputArgs&&... args) const;
	template<typename... InputArgs>
	Ret ExecuteIfBound(InputArgs&&... args) const;

protected:
	PROPERTY(_mBinding)
	mutable std::pair<std::shared_ptr<Object>, std::string> _mBinding;
};

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
inline DynamicDelegateHandle DynamicSingleDelegate<Ret, Args...>::Bind(const Binder& binder)
{
	_mBinding = binder._mBinding;

	return DynamicDelegateHandle{ _mBinding.first, _mBinding.second };
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename C>
inline DynamicDelegateHandle DynamicSingleDelegate<Ret, Args...>::BindMethod(const std::shared_ptr<C>& object, const Method* method)
{
	_mBinding.first = object;
	_mBinding.second = method->GetName();

	return DynamicDelegateHandle{ _mBinding.first, _mBinding.second };
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename C>
inline DynamicDelegateHandle DynamicSingleDelegate<Ret, Args...>::BindMethod(const std::shared_ptr<C>& object, const char* methodName)
{
	_mBinding.first = object;
	_mBinding.second = methodName;

	return DynamicDelegateHandle{ _mBinding.first, _mBinding.second };
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
inline bool DynamicSingleDelegate<Ret, Args...>::Unbind(DynamicDelegateHandle handle)
{
	if (_mBinding.first == handle.first.lock() && _mBinding.second == handle.second && IsBound() == true)
	{
		Clear();
		return true;
	}
	return false;
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
inline bool DynamicSingleDelegate<Ret, Args...>::Unbind(std::shared_ptr<Object> object)
{
	if (_mBinding.first == object && IsBound() == true)
	{
		Clear();
		return true;
	}
	return false;
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename ...InputArgs>
inline Ret DynamicSingleDelegate<Ret, Args...>::Execute(InputArgs && ...args) const
{
	ASSERT_MSG(_mIsCalled == false, "Not safe delegate was called circularly");
	_mIsCalled = true;

	const Method* method = _mBinding.first->GetTypeInfo().GetMethod(_mBinding.second.c_str());
	if constexpr (std::is_same_v<Ret, void> == true)
	{
		method->Invoke(_mBinding.first.get(), std::forward<InputArgs>(args)...);
		_mIsCalled = false;
	}
	else
	{
		Ret ret = method->Invoke<Object, Ret>(_mBinding.first.get(), std::forward<InputArgs>(args)...);
		_mIsCalled = false;
		return ret;
	}
}

template<typename Ret, typename ...Args> requires (!HasAnyReference<Ret, Args...>)
template<typename ...InputArgs>
inline Ret DynamicSingleDelegate<Ret, Args...>::ExecuteIfBound(InputArgs && ...args) const
{
	if (IsBound() == false)
	{
		if constexpr (std::is_same_v<Ret, void> == true)
		{
			return;
		}
		else
		{
			return {};
		}
	}

	ASSERT_MSG(_mIsCalled == false, "Not safe delegate was called circularly");
	_mIsCalled = true;

	const Method* method = _mBinding.first->GetTypeInfo().GetMethod(_mBinding.second.c_str());
	if constexpr (std::is_same_v<Ret, void> == true)
	{
		method->Invoke(_mBinding.first.get(), std::forward<InputArgs>(args)...);
		_mIsCalled = false;
	}
	else
	{
		Ret ret = method->Invoke<Object, Ret>(_mBinding.first.get(), std::forward<InputArgs>(args)...);
		_mIsCalled = false;
		return ret;
	}
}

#pragma endregion


//template<typename... Args>
//class DynamicMulticastDelegate {};