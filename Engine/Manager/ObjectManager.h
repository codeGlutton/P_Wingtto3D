#pragma once

#define OBJECT_MANAGER ObjectManager::GetInst()

class Object;

namespace ObjectFlag
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
	ObjectInitializeParameters(const ObjectTypeInfo* typeInfo) :
		mTypeInfo(typeInfo)
	{
	}

public:
	const ObjectTypeInfo* mTypeInfo;
	Object* mOuter = nullptr;
	ObjectFlag::Type mFlags = ObjectFlag::None;
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
	std::shared_ptr<Object> CreateObject(ObjectInitializeParameters params) const;
	void LoadObjects(const std::vector<std::shared_ptr<Object>>& objects) const;

private:
	inline bool ShouldLoadProperties(ObjectFlag::Type flags) const
	{
		return !(flags & (ObjectFlag::CDO | ObjectFlag::DeferredLoad));
	}
};

