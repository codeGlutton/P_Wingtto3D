#pragma once

#define DECLARE_DELEGATE_INTERNAL(name, ...) using name = NativeSingleDelegate<__VA_ARGS__>

#define DECLARE_DELEGATE(name)											DECLARE_DELEGATE_INTERNAL(name, void)
#define DECLARE_DELEGATE_1_PARAM(name, _1)								DECLARE_DELEGATE_INTERNAL(name, void, _1)
#define DECLARE_DELEGATE_2_PARAMS(name, _1, _2)							DECLARE_DELEGATE_INTERNAL(name, void, _1, _2)
#define DECLARE_DELEGATE_3_PARAMS(name, _1, _2, _3)						DECLARE_DELEGATE_INTERNAL(name, void, _1, _2, _3)
#define DECLARE_DELEGATE_4_PARAMS(name, _1, _2, _3, _4)					DECLARE_DELEGATE_INTERNAL(name, void, _1, _2, _3, _4)
#define DECLARE_DELEGATE_5_PARAMS(name, _1, _2, _3, _4, _5)				DECLARE_DELEGATE_INTERNAL(name, void, _1, _2, _3, _4, _5)

#define DECLARE_DELEGATE_RET(name, ret)									DECLARE_DELEGATE_INTERNAL(name, ret)
#define DECLARE_DELEGATE_RET_1_PARAM(name, ret, _1)						DECLARE_DELEGATE_INTERNAL(name, ret, _1)
#define DECLARE_DELEGATE_RET_2_PARAMS(name, ret, _1, _2)				DECLARE_DELEGATE_INTERNAL(name, ret, _1, _2)
#define DECLARE_DELEGATE_RET_3_PARAMS(name, ret, _1, _2, _3)			DECLARE_DELEGATE_INTERNAL(name, ret, _1, _2, _3)
#define DECLARE_DELEGATE_RET_4_PARAMS(name, ret, _1, _2, _3, _4)		DECLARE_DELEGATE_INTERNAL(name, ret, _1, _2, _3, _4)
#define DECLARE_DELEGATE_RET_5_PARAMS(name, ret, _1, _2, _3, _4, _5)	DECLARE_DELEGATE_INTERNAL(name, ret, _1, _2, _3, _4, _5)

#define DECLARE_MULTICAST_DELEGATE_INTERNAL(name, ...) using name = NativeMulticastDelegate<__VA_ARGS__>

#define DECLARE_MULTICAST_DELEGATE(name)								DECLARE_MULTICAST_DELEGATE_INTERNAL(name)
#define DECLARE_MULTICAST_DELEGATE_1_PARAM(name, _1)					DECLARE_MULTICAST_DELEGATE_INTERNAL(name, _1)
#define DECLARE_MULTICAST_DELEGATE_2_PARAMS(name, _1, _2)				DECLARE_MULTICAST_DELEGATE_INTERNAL(name, _1, _2)
#define DECLARE_MULTICAST_DELEGATE_3_PARAMS(name, _1, _2, _3)			DECLARE_MULTICAST_DELEGATE_INTERNAL(name, _1, _2, _3)
#define DECLARE_MULTICAST_DELEGATE_4_PARAMS(name, _1, _2, _3, _4)		DECLARE_MULTICAST_DELEGATE_INTERNAL(name, _1, _2, _3, _4)
#define DECLARE_MULTICAST_DELEGATE_5_PARAMS(name, _1, _2, _3, _4, _5)	DECLARE_MULTICAST_DELEGATE_INTERNAL(name, _1, _2, _3, _4, _5)

#pragma region NATIVE_DELEGATE_BASE

using DelegateHandle = uint32;

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


#pragma region NATIVE_SINGLE_DELEGATE

template<typename Ret, typename... Args> requires (!HasAnyReference<Ret, Args...>)
class NativeSingleDelegate : public NativeDelegateBase
{
public:
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
					(sp.get()->*func)(std::move(args)...);
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
					return (sp.get()->*func)(std::move(args)...);
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
					(sp.get()->*func)(std::move(args)...);
				}
				};
		}
		else
		{
			_mCallback = [sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					return (sp.get()->*func)(std::move(args)...);
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
					(sp.get()->*func)(std::move(args)...);
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
					return (sp.get()->*func)(std::move(args)...);
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
					(sp.get()->*func)(std::move(args)...);
				}
			};
		}
		else
		{
			_mCallback = [sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					return (sp.get()->*func)(std::move(args)...);
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
					(sp.get()->*func)(std::move(args)...);
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
					return (sp.get()->*func)(std::move(args)...);
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
					(sp.get()->*func)(std::move(args)...);
				}
				};
		}
		else
		{
			_mCallback = [sp = object, func](Args... args) {
				if (sp != nullptr)
				{
					return (sp.get()->*func)(std::move(args)...);
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
	struct CallbackDesc
	{
		DelegateHandle mHandle;
		std::function<void(Args...)> mCallback;
	};

public:
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
					(sp.get()->*func)(std::move(args)...);
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
					(sp.get()->*func)(std::move(args)...);
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
					(sp.get()->*func)(std::move(args)...);
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
					(sp.get()->*func)(std::move(args)...);
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
					(sp.get()->*func)(std::move(args)...);
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
					(sp.get()->*func)(std::move(args)...);
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
					_mIsDirty == true;
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


//template<typename Ret, typename... Args>
//class NativeMulticastDelegate
//{
//public:
//	void BindStatic(Ret(*func)(Args...));
//	template<typename C>
//	void BindNativeMethod(const C* object, Ret(C::* func)(Args...));
//	template<typename C>
//	void BindNativeMethod(const C* object, Ret(C::* func)(Args...) const);
//
//	template<typename C>
//	void BindMethod(const C* object, const Method* method);
//
//public:
//	bool IsBound();
//
//public:
//	template<typename... InputArgs>
//	void
//
//protected:
//	std::vector<std::function<void()>> _mCallbacks;
//};
//
//struct CallbackDesc
//{
//	std::weak_ptr<Object> mObject;
//	std::string mMethodName;
//};
//

