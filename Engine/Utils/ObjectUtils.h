#pragma once

#include "Manager/ObjectManager.h"

class Object;
class Package;

bool IsValid(const Object* object);
std::shared_ptr<Package> GetOuter(const Object* object);
const std::wstring* GetObjectName(const Object* object);
const std::wstring* GetObjectPath(const Object* object);
const std::wstring* GetObjectFullPath(const Object* object);

template<typename T>
inline const T* GetDefault()
{
	return static_cast<const T*>(T::GetStaticTypeInfo().GetDefaultObject());
}

template<class T> requires std::is_base_of_v<Object, T>
std::shared_ptr<T> NewObject(std::shared_ptr<Package> outer, const std::wstring& name, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
std::shared_ptr<Object> NewObject(std::shared_ptr<Package> outer, const ObjectTypeInfo* typeInfo, const std::wstring& name, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
template<class T> requires std::is_base_of_v<Object, T>
std::shared_ptr<T> NewObject(const std::wstring& name, const std::wstring& path, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
std::shared_ptr<Object> NewObject(const ObjectTypeInfo* typeInfo, const std::wstring& name, const std::wstring& path, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);

template<class T> requires std::is_base_of_v<Object, T>
std::shared_ptr<T> NewObject(std::shared_ptr<Package> outer, const std::wstring& name, ObjectCreateFlag::Type flags)
{
	return std::static_pointer_cast<T>(NewObject(outer, &T::GetStaticTypeInfo(), name, flags));
}

template<class T>
inline std::shared_ptr<T> NewObject(const std::wstring& name, const std::wstring& path, ObjectCreateFlag::Type flags)
{
	return std::static_pointer_cast<T>(NewObject(&T::GetStaticTypeInfo(), name, path, flags));
}



