#pragma once

#include <chrono>

template<typename Ret>
class Tester
{
	using CallbackType = std::function<Ret()>;

public:
	template<typename... Args>
	Tester(Ret(*func)(Args...), Args... args)
	{
		_mCallback = [func, args...]() {
			return (*func)(args...);
			};
	}

	template<typename C, typename... Args>
	Tester(C* owner, Ret(C::*memFunc)(Args...), Args... args)
	{
		_mCallback = [owner, memFunc, args...]() {
			return (owner->*memFunc)(args...);
			};
	}

public:
	std::pair<std::chrono::microseconds, Ret> operator() () const
	{
		auto start = std::chrono::steady_clock::now();
		std::pair<std::chrono::microseconds, Ret> result = std::make_pair(typename std::chrono::microseconds{ 0 }, _mCallback());
		auto end = std::chrono::steady_clock::now();

		result.first = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		return result;
	}

protected:
	CallbackType _mCallback;
};

template<>
class Tester<void>
{
	using CallbackType = std::function<void()>;

public:
	template<typename... Args>
	Tester(void(*func)(Args...), Args... args)
	{
		_mCallback = [func, args...]() {
			(*func)(args...);
			};
	}

	template<typename C, typename... Args>
	Tester(C* owner, void(C::* memFunc)(Args...), Args... args)
	{
		_mCallback = [owner, memFunc, args...]() {
			(owner->*memFunc)(args...);
			};
	}

public:
	std::chrono::microseconds operator() () const
	{
		auto start = std::chrono::steady_clock::now();
		_mCallback();
		auto end = std::chrono::steady_clock::now();

		return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	}

protected:
	CallbackType _mCallback;
};