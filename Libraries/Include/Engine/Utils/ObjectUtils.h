#pragma once

#include "Manager/ObjectManager.h"

template<typename T>
const T* GetDefalt()
{
	return static_cast<const T*>(T::GetStaticTypeInfo().GetDefaultObject());
}

template<class T>
std::shared_ptr<T> NewObject(Object* outer, ObjectFlag::Type flags = ObjectFlag::None)
{
	ObjectInitializeParameters params(T::GetStaticTypeInfo());
	params.mOuter = outer;
	params.mFlags = flags;

	return std::static_pointer_cast<T>(OBJECT_MANAGER->CreateObject(params));
}

template<class T>
std::shared_ptr<T> NewObject(Object* outer, const ObjectTypeInfo* typeInfo, ObjectFlag::Type flags = ObjectFlag::None)
{
	ObjectInitializeParameters params(typeInfo);
	params.mOuter = outer;
	params.mFlags = flags;

	return std::static_pointer_cast<T>(OBJECT_MANAGER->CreateObject(params));
}
