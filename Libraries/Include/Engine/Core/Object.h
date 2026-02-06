#pragma once

class Archive;
class Package;

class Object abstract : public std::enable_shared_from_this<Object>
{
	GEN_ABSTRACT_REFLECTION(Object)

protected:
	Object() {}
	virtual ~Object() {}

protected:
	/**
	 * 실제 생성 시점
	 */
	virtual void PostCreate();
	/**
	 * 인스턴스 객체 로드 시점
	 */
	virtual void PostLoad();
	/**
	 * 실제 제거 시점
	 */
	virtual void BeginDestroy();

public:
	std::shared_ptr<Package> GetOuter() const
	{
		return _mOuter;
	}
	const std::wstring& GetName() const
	{
		return _mName;
	}
	const std::wstring& GetPath() const
	{
		return _mPath;
	}
	const std::wstring& GetFullPath() const
	{
		return _mFullPath;
	}

public:
	bool IsAlive() const
	{
		return _mIsAlive;
	}
	bool IsReplicated() const
	{
		return _mIsReplicated;
	}

	virtual bool IsValid() const
	{
		return _mIsAlive;
	}

public:
	void SetName(const std::wstring& name);

public:
	bool IsA(const TypeInfo* typeInfo) const
	{
		if (typeInfo == nullptr)
		{
			return false;
		}
		return typeInfo->IsA(GetTypeInfo());
	}

	template<typename T>
	bool IsA() const
	{
		return IsA(T::GetStaticTypeInfo());
	}

protected:
	virtual void Serialize(Archive& archive) const;
	virtual void Deserialize(Archive& archive);

private:
	// 저장 주체
	std::shared_ptr<Package> _mOuter;

private:
	PROPERTY(_mName)
	std::wstring _mName;
	PROPERTY(_mPath)
	std::wstring _mPath;
	std::wstring _mFullPath;
	
private:
	bool _mIsAlive = true;
	PROPERTY(_mIsReplicated)
	bool _mIsReplicated = false;

	
};

