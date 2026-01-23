#include "pch.h"
#include "ObjectUtils.h"

#include "Core/Object.h"
#include "Core/Resource/Package/Package.h"

bool IsValid(const Object* object)
{
    return object != nullptr && object->IsAlive() == true;
}

std::shared_ptr<Package> GetOuter(const Object* object)
{
	if (object == nullptr)
	{
		return std::shared_ptr<Package>();
	}
    return object->GetOuter();
}

const std::wstring* GetObjectName(const Object* object)
{
	if (object == nullptr)
	{
		return nullptr;
	}
	return &object->GetName();
}

const std::wstring* GetObjectPath(const Object* object)
{
	if (object == nullptr)
	{
		return nullptr;
	}
	return &object->GetPath();
}

const std::wstring* GetObjectFullPath(const Object* object)
{
	if (object == nullptr)
	{
		return nullptr;
	}
	return &object->GetFullPath();
}

std::shared_ptr<Object> NewObject(std::shared_ptr<Package> outer, const ObjectTypeInfo* typeInfo, const std::wstring& name, ObjectCreateFlag::Type flags)
{
	ASSERT_MSG(typeInfo->IsChildOf<Object>() == true, "NewObject func is not allowed to create non object class");

	ObjectInitializeParameters params(typeInfo, name);
	params.mOuter = outer;
	params.mFlags = flags;

	return OBJECT_MANAGER->CreateObject(params);
}

std::shared_ptr<Object> NewObject(const ObjectTypeInfo* typeInfo, const std::wstring& name, const std::wstring& path, ObjectCreateFlag::Type flags)
{
	ASSERT_MSG(typeInfo->IsChildOf<Object>() == true, "NewObject func is not allowed to create non object class");

	ObjectInitializeParameters params(typeInfo, name);
	params.mPath = path;
	params.mFlags = flags;

	return OBJECT_MANAGER->CreateObject(params);
}
