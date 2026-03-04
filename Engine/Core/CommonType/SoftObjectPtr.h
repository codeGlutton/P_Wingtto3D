#pragma once

template<typename T>
class SoftObjectPtr
{
public:
	SoftObjectPtr() = default;
	SoftObjectPtr(const std::wstring& path) :
		_mFullPath(path),
		_mObject()
	{
	}
	SoftObjectPtr(std::wstring&& path) :
		_mFullPath(std::move(path)),
		_mObject()
	{
	}
	// 다른 상속된 타입의 SubClass 객체로 초기화
	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
	SoftObjectPtr(const SoftObjectPtr<U>& other) :
		_mFullPath(other.path),
		_mObject(other._mObject)
	{
	}
	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
	SoftObjectPtr(SoftObjectPtr<U>&& other) :
		_mFullPath(std::move(other.path)),
		_mObject(std::move(other._mObject))
	{
	}

public:
	SoftObjectPtr& operator=(SoftObjectPtr&&) = default;
	SoftObjectPtr& operator=(const SoftObjectPtr&) = default;

	SoftObjectPtr& operator=(const std::wstring& path)
	{
		_mFullPath = path;
		_mObject.reset();
		return *this;
	}
	SoftObjectPtr& operator=(std::wstring&& path)
	{
		_mFullPath = std::move(path);
		_mObject.reset();
		return *this;
	}
	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
	SoftObjectPtr& operator=(const SoftObjectPtr<U>& other)
	{
		_mFullPath = other._mFullPath;
		_mObject = other._mObject;
		return *this;
	}
	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
	SoftObjectPtr& operator=(SoftObjectPtr<U>&& other)
	{
		_mFullPath = std::move(other._mFullPath);
		_mObject = std::move(other._mObject);
		return *this;
	}

	explicit operator std::wstring() const noexcept
	{
		return _mFullPath;
	}

public:
	bool operator==(const SoftObjectPtr& other) const
	{
		return _mFullPath == other._mFullPath;
	}
	bool operator==(SoftObjectPtr&& other) const
	{
		return _mFullPath == other._mFullPath;
	}

public:
	const std::wstring& GetFullPath() const
	{
		return _mFullPath;
	}
	std::shared_ptr<T> Get() const
	{
		return _mObject.lock();
	}

private:
	std::wstring _mFullPath;
	std::weak_ptr<T> _mObject;
};

