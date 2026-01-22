#pragma once

// 람다와 Function을 활용한 Job 클래스
class Job
{
public:
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
	template<typename T, typename Ret, typename... Args, typename... Params>
	Job(std::shared_ptr<T> owner, Ret(* staticFunc)(Args...), Params&&... params)
	{
		_mCallback = [owner, staticFunc, params...]()
			{
				(*staticFunc)(params...);
			};
	}
	template<typename T, typename Ret, typename... Args, typename... Params>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Params&&... params)
	{
		_mCallback = [owner, memFunc, params...]()
			{
				(owner.get()->*memFunc)(params...);
			};
	}
	template<typename T, typename Ret, typename... Args, typename... Params>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...) const, Params&&... params)
	{
		_mCallback = [owner, memFunc, params...]()
			{
				(owner.get()->*memFunc)(params...);
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

template<typename MetaData>
class JobContext : public Job
{
public:
	// 이미 생성된 콜백 클로저를 활용할 경우 생성자
	JobContext(const MetaData& metaData, CallBackType callback) :
		Job(std::move(callback)),
		_mMetaData(metaData)
	{
	}
	JobContext(MetaData&& metaData, CallBackType callback) :
		Job(std::move(callback)),
		_mMetaData(std::move(metaData))
	{
	}

public:
	// 메소드 콜백을 활용하는 Job 생성자
	template<typename Meta, typename T, typename Ret, typename... Args, typename... Params>
	JobContext(Meta&& metaData, std::shared_ptr<T> owner, Ret(*staticFunc)(Args...), Params&&... params) :
		Job(owner, staticFunc, std::forward<Params>(params)...),
		_mMetaData(std::forward<Meta>(metaData))
	{
	}
	template<typename Meta, typename T, typename Ret, typename... Args, typename... Params>
	JobContext(Meta&& metaData, std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Params&&... params) :
		Job(owner, memFunc, std::forward<Params>(params)...),
		_mMetaData(std::forward<Meta>(metaData))
	{
	}
	template<typename Meta, typename T, typename Ret, typename... Args, typename... Params>
	JobContext(Meta&& metaData, std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...) const, Params&&... params) :
		Job(owner, memFunc, std::forward<Params>(params)...),
		_mMetaData(std::forward<Meta>(metaData))
	{
	}

public:
	const MetaData& GetMetaData() const
	{
		return _mMetaData;
	}

private:
	MetaData _mMetaData;
};