#pragma once

#define OBJECT_MANAGER ObjectManager::GetInst()

class Object;

namespace ObjectCreateFlag
{
	enum Type : uint8
	{
		None = 0,

		CDO = 1 << 0,			// 초기 CDO 생성 시
		DeferredLoad = 1 << 1,	// Object Index를 통해 Pointer 초기화 필요
	};
}

struct ObjectInitializeParameters
{
public:
	ObjectInitializeParameters(const ObjectTypeInfo* typeInfo, const std::wstring& name) :
		mTypeInfo(typeInfo),
		mName(name)
	{
	}

public:
	const ObjectTypeInfo* mTypeInfo;
	std::wstring mName;
	std::wstring mPath = L"";
	std::shared_ptr<Package> mOuter = nullptr;
	ObjectCreateFlag::Type mFlags = ObjectCreateFlag::None;
};

class ObjectManager
{
private:
	ObjectManager();
	~ObjectManager();

public:
	static ObjectManager* GetInst()
	{
		static ObjectManager inst;
		return &inst;
	}

public:
	void Init();
	void Destroy();

public:
	std::shared_ptr<Object> CreateObject(ObjectInitializeParameters params) const;

public:
	void RequestToRename(const std::wstring& path, OUT std::wstring& rename) const;

	void NotifyToAddObject(const std::wstring& fullPath);
	void NotifyToRemoveObject(const std::wstring& fullPath);

private:
	inline bool ShouldLoadProperties(ObjectCreateFlag::Type flags) const
	{
		return !(flags & (ObjectCreateFlag::CDO | ObjectCreateFlag::DeferredLoad));
	}

private:
	std::unordered_set<std::wstring> _mFullPathNameSet;
};

