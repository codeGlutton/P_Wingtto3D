#pragma once

#define OBJECT_MANAGER ObjectManager::GetInst()

class Object;

namespace ObjectFlag
{
	enum Type : uint8
	{
		None = 0,
		CDO = 1 << 0,
		WorldActor = 1 << 1,
		Asset = 1 << 2,

		Instancing = WorldActor | Asset,
	};
}

struct ObjectInitializeParameters
{
public:
	ObjectInitializeParameters(ObjectTypeInfo* typeInfo) :
		mTypeInfo(typeInfo)
	{
	}

public:
	ObjectTypeInfo* mTypeInfo;
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
	std::shared_ptr<Object> CreateObject(ObjectInitializeParameters params);

private:
	inline bool ShouldLoadProperties(ObjectFlag::Type flags)
	{
		return !(flags & ObjectFlag::CDO) && (flags & ObjectFlag::Instancing);
	}
};

