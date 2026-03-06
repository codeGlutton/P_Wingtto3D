#include "pch.h"
#include "ObjectTypeInfo.h"

#include "Core/Object.h"
#include "Utils/ObjectUtils.h"

#include "Core/Archive.h"

bool ObjectTypeInfo::IsChildOf(const StructTypeInfo& other) const
{
	if (StructTypeInfo::IsChildOf(other) == true)
	{
		return true;
	}
	
	// other이 인터페이스인 경우, 상속 검증
	return ImplementsInterface(reinterpret_cast<const ObjectTypeInfo&>(other));
}

bool ObjectTypeInfo::ImplementsInterface(const ObjectTypeInfo& other) const
{
	if (IsA(other) == true)
	{
		return true;
	}

	// 새로운 인터페이스들의 부모 거쳐가면서 검증
	for (const ObjectTypeInfo* interfaceInfo : _mAdditionalInterfaces)
	{
		if (interfaceInfo->ImplementsInterface(other) == true)
		{
			return true;
		}
	}

	// 부모 거쳐가면서 검증
	if (_mSuperInfo != nullptr)
	{
		return GetSuper()->ImplementsInterface(other);
	}
	return false;
}

std::size_t ObjectTypeInfo::GetInterfaceOffsetOf(const ObjectTypeInfo& other, OUT bool& isInherited) const
{
	if (IsA(other) == true)
	{
		isInherited = true;
		return 0ull;
	}

	// 새로운 인터페이스들의 부모 거쳐가면서 검증
	const std::size_t size = _mAdditionalInterfaces.size();
	for (size_t i = 0; i < size; ++i)
	{
		const std::size_t offset = _mAdditionalInterfaces[i]->GetInterfaceOffsetOf(other, isInherited);
		if (isInherited == true)
		{
			return offset + _mAdditionalInterfaceOffsets[i];
		}
	}

	// 부모 거쳐가면서 검증
	if (_mSuperInfo != nullptr)
	{
		return GetSuper()->GetInterfaceOffsetOf(other, isInherited);
	}
	isInherited = false;
	return 0ull;
}

void ObjectTypeInfo::Serialize(OUT Archive& archive, const void* inst) const
{
	// 부모를 거쳐가면서 검증
	std::vector<const std::pair<const std::string_view, const Property*>*> changes;
	for (const ObjectTypeInfo* currentInfo = this; currentInfo != nullptr; currentInfo = currentInfo->GetSuper())
	{
		const PropertyMap& propertyMap = currentInfo->_mPropertyMap;
		for (const auto& propertyPair : propertyMap)
		{
			// 기본값과 틀린 항목만 기록
			if (propertyPair.second->IsEqual(inst, GetDefaultObject().get()) == false)
			{
				changes.push_back(&propertyPair);
			}
		}
	}

	// 프로퍼티 갯수 저장
	std::size_t propertyCount = changes.size();
	TypeInfoResolver<std::size_t>::Get().Serialize(archive, &propertyCount);

	// 각 프로퍼티 저장
	for (const auto& changePair : changes)
	{
		// 프로퍼티 이름 저장
		std::string propertyName = changePair->first.data();
		TypeInfoResolver<std::string>::Get().Serialize(archive, &propertyName);

		// 값 저장
		const void* propertyPtr = changePair->second->GetRawPtr(inst);
		changePair->second->GetTypeInfo().Serialize(archive, propertyPtr);
	}
}

void ObjectTypeInfo::Deserialize(Archive& archive, OUT void* inst) const
{
	// 프로퍼티 갯수 저장
	std::size_t propertyCount;
	TypeInfoResolver<std::size_t>::Get().Deserialize(archive, &propertyCount);

	// 로드
	for (std::size_t i = 0; i < propertyCount; ++i)
	{
		// 프로퍼티 이름 로드
		std::string propertyName;
		TypeInfoResolver<std::string>::Get().Deserialize(archive, &propertyName);

		// 부모를 거쳐가면서 값 찾아 로드
		for (const ObjectTypeInfo* currentInfo = this; currentInfo != nullptr; currentInfo = currentInfo->GetSuper())
		{
			const PropertyMap& propertyMap = currentInfo->_mPropertyMap;
			auto iter = propertyMap.find(propertyName);
			if (iter != propertyMap.end())
			{
				void* propertyPtr = iter->second->GetRawPtr(inst);
				iter->second->GetTypeInfo().Deserialize(archive, propertyPtr);
				break;
			}
		}
	}
}

const Method* ObjectTypeInfo::GetMethod(const char* name) const
{
	auto iter = _mMethodMap.find(name);
	if (iter != _mMethodMap.end())
	{
		return iter->second;
	}

	// 인터페이스 거쳐가면서 탐색
	for (const ObjectTypeInfo* interfaceInfo : _mAdditionalInterfaces)
	{
		const Method* interfaceMethod = interfaceInfo->GetMethod(name);
		if (interfaceMethod != nullptr)
		{
			return interfaceMethod;
		}
	}

	// 부모를 거쳐가면서 탐색
	if (_mSuperInfo != nullptr)
	{
		return GetSuper()->GetMethod(name);
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

std::shared_ptr<const Object> ObjectTypeInfo::GetDefaultObject() const
{
	if (_mDefaultObject == nullptr)
	{
		_mDefaultObject = NewObject(nullptr, this, ConvertUtf8ToWString(_mName), ObjectCreateFlag::CDO);
	}
	return _mDefaultObject;
}

