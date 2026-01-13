#pragma once


// 람다와 Funtion을 활용한 Job 클래스
class Job
{
	using CallBackType = std::function<void()>;

public:
	// 이미 생성된 콜백 클로저를 활용할 경우 생성자
	Job(const CallBackType& callback) : _mCallback(callback)
	{
	}
	Job(CallBackType&& callback) : _mCallback(std::move(callback))
	{
	}

	Job& operator=(const CallBackType& callback)
	{
		_mCallback = callback;
		return *this;
	}

	Job& operator=(CallBackType&& callback)
	{
		_mCallback = std::move(callback);
		return *this;
	}

public:
	explicit operator CallBackType()
	{
		return _mCallback;
	}

	CallBackType operator*()
	{
		return _mCallback;
	}

	void operator()() const
	{
		if (_mCallback != nullptr)
		{
			_mCallback();
		}
	}

	explicit operator bool() const
	{
		return _mCallback != nullptr;
	}

public:
	// 메소드 콜백을 활용하는 Job 생성자
	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> owner, Ret(* staticFunc)(Args...), Args&&... args)
	{
		_mCallback = [owner, staticFunc, args...]()
			{
				(*staticFunc)(args...);
			};
	}
	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		_mCallback = [owner, memFunc, args...]()
			{
				(owner.get()->*memFunc)(args...);
			};
	}
	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...) const, Args&&... args)
	{
		_mCallback = [owner, memFunc, args...]()
			{
				(owner.get()->*memFunc)(args...);
			};
	}

public:
	void Execute() const
	{
		_mCallback();
	}

private:
	CallBackType _mCallback;
};
