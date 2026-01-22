#pragma once

template<typename T> requires std::is_base_of_v<Object, T>
class SoftObjectPtr
{
public:
	SoftObjectPtr() = default;
	SoftObjectPtr(const std::wstring& path) :
		_mPath(path),
		_mObject()
	{
	}
	SoftObjectPtr(std::wstring&& path) :
		_mPath(std::move(path)),
		_mObject()
	{
	}
	// 다른 상속된 타입의 SubClass 객체로 초기화
	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
	SoftObjectPtr(const SoftObjectPtr<U>& other) :
		_mPath(other.path),
		_mObject(other._mObject)
	{
	}
	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
	SoftObjectPtr(SoftObjectPtr<U>&& other) :
		_mPath(std::move(other.path)),
		_mObject(std::move(other._mObject))
	{
	}

public:
	SoftObjectPtr& operator=(SoftObjectPtr&&) = default;
	SoftObjectPtr& operator=(const SoftObjectPtr&) = default;

	SoftObjectPtr& operator=(const std::wstring& path)
	{
		_mPath = path;
		_mObject.reset();
		return *this;
	}
	SoftObjectPtr& operator=(std::wstring&& path)
	{
		_mPath = std::move(path);
		_mObject.reset();
		return *this;
	}
	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
	SoftObjectPtr& operator=(const SoftObjectPtr<U>& other)
	{
		_mPath = other._mPath;
		_mObject = other._mPath;
		return *this;
	}
	template <typename U> requires(
		std::is_convertible_v<U*, T*> == true
		)
	SoftObjectPtr& operator=(SoftObjectPtr<U>&& other)
	{
		_mPath = std::move(other._mPath);
		_mObject = std::move(other._mPath);
		return *this;
	}

	explicit operator std::wstring() const noexcept
	{
		return _mPath;
	}

public:
	bool operator==(const SoftObjectPtr& other) const
	{
		return _mPath == other._mPath;
	}
	bool operator==(SoftObjectPtr&& other) const
	{
		return _mPath == other._mPath;
	}

public:
	const std::wstring& GetPath() const
	{
		return _mPath;
	}
	std::shared_ptr<T> Get() const
	{
		return _mObject.lock();
	}

	void LoadSync();
	void LoadASync();

private:
	std::wstring _mPath;
	std::weak_ptr<T> _mObject;
};

template<typename T> requires std::is_base_of_v<Object, T>
void SoftObjectPtr<T>::LoadSync()
{
}

template<typename T> requires std::is_base_of_v<Object, T>
void SoftObjectPtr<T>::LoadASync()
{
}