#include "pch.h"
#include "ObjectTypeInfo.h"

bool ObjectTypeInfo::IsChildOf(const ObjectTypeInfo& other) const
{
	if (IsA(other) == true)
	{
		return true;
	}

	// 부모를 거쳐가면서 검증
	for (const ObjectTypeInfo* superInfo = _mSuperInfo; superInfo != nullptr; superInfo = superInfo->GetSuper())
	{
		if (superInfo->IsA(other) == true)
		{
			return true;
		}
	}

	// other이 인터페이스인 경우, 상속 검증
	return ImplementsInterface(other);
}

bool ObjectTypeInfo::ImplementsInterface(const ObjectTypeInfo& other) const
{
	if (IsA(other) == true)
	{
		return true;
	}

	// 인터페이스 부모 거쳐가면서 검증
	for (const ObjectTypeInfo* interfaceInfo : _mInterfaces)
	{
		if (interfaceInfo->ImplementsInterface(other) == true)
		{
			return true;
		}
	}
	return false;
}

const Method* ObjectTypeInfo::GetMethod(const char* name) const
{
	// 부모를 거쳐가면서 탐색
	for (const ObjectTypeInfo* currentInfo = this; currentInfo != nullptr; currentInfo = currentInfo->GetSuper())
	{
		const auto& methodMap = currentInfo->_mMethodMap;
		auto iter = methodMap.find(name);
		if (iter != methodMap.end())
		{
			return iter->second;
		}
	}
	return nullptr;
}

const Property* ObjectTypeInfo::GetProperty(const char* name) const
{
	// 부모를 거쳐가면서 탐색
	for (const ObjectTypeInfo* currentInfo = this; currentInfo != nullptr; currentInfo = currentInfo->GetSuper())
	{
		const auto& propertyMap = currentInfo->_mPropertyMap;
		auto iter = propertyMap.find(name);
		if (iter != propertyMap.end())
		{
			return iter->second;
		}
	}
	return nullptr;
}

void ObjectTypeInfo::AddMethod(const Method* method)
{
	const char* methodName = method->GetName();
	ASSERT_MSG(_mMethodMap.contains(methodName) == false, "Find same name method");

	_mMethodMap[methodName] = method;
	_mMethods.push_back(method);
}

void ObjectTypeInfo::AddProperty(const Property* property)
{
	const char* propertyName = property->GetName();
	ASSERT_MSG(_mPropertyMap.contains(propertyName) == false, "Find same name property");

	_mPropertyMap[propertyName] = property;
	_mProperties.push_back(property);
}