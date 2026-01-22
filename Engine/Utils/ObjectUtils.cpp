#include "pch.h"
#include "ObjectUtils.h"

#include "Core/Object.h"
#include "Core/Resource/Package.h"

bool IsValid(const Object* object)
{
    return object != nullptr && object->IsAlive() == true;
}

std::shared_ptr<Package> GetOuter(const Object* object)
{
    return object->GetOuter().lock();
}

const std::wstring& GetObjectName(const Object* object)
{
	return object->GetName();
}

const std::wstring& GetObjectPath(const Object* object)
{
	return object->GetPath();
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
