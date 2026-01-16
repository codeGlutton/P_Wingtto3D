#include "pch.h"
#include "StructTypeInfo.h"

bool StructTypeInfo::IsChildOf(const StructTypeInfo& other) const
{
	if (IsA(other) == true)
	{
		return true;
	}

	// 부모를 거쳐가면서 검증
	for (const StructTypeInfo* superInfo = _mSuperInfo; superInfo != nullptr; superInfo = superInfo->GetSuper())
	{
		if (superInfo->IsA(other) == true)
		{
			return true;
		}
	}
	return false;
}

bool StructTypeInfo::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	// 부모를 거쳐가면서 검증
	for (const StructTypeInfo* currentInfo = this; currentInfo != nullptr; currentInfo = currentInfo->GetSuper())
	{
		const PropertyMap& propertyMap = currentInfo->_mPropertyMap;
		for (auto& propertyPair : propertyMap)
		{
			if (propertyPair.second->IsEqual(lhsInst, rhsInst) == false)
			{
				return false;
			}
		}
	}
	return true;
}

void StructTypeInfo::Serialize(OUT Archive& archive, const void* inst) const
{
	std::string name = _mName;
	TypeInfoResolver<std::string>::Get().Serialize(archive, &name);

	// 부모를 거쳐가면서 검증
	std::vector<const std::pair<const std::string_view, const Property*>*> changes;
	for (const StructTypeInfo* currentInfo = this; currentInfo != nullptr; currentInfo = currentInfo->GetSuper())
	{
		const PropertyMap& propertyMap = currentInfo->_mPropertyMap;
		for (const auto& propertyPair : propertyMap)
		{
			changes.push_back(&propertyPair);
		}
	}

	archive << changes.size();

	// 저장
	for (const auto& changePair : changes)
	{
		archive << changePair->first;
		changePair->second->GetTypeInfo().Serialize(archive, inst);
	}
}

void StructTypeInfo::Deserialize(Archive& archive, OUT void* inst) const
{
}

const Property* StructTypeInfo::GetProperty(const char* name) const
{
	// 부모를 거쳐가면서 탐색
	for (const StructTypeInfo* currentInfo = this; currentInfo != nullptr; currentInfo = currentInfo->GetSuper())
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

void StructTypeInfo::AddProperty(const Property* property)
{
	const char* propertyName = property->GetName();
	ASSERT_MSG(_mPropertyMap.contains(propertyName) == false, "Find same name property");

	_mPropertyMap[propertyName] = property;
	_mProperties.push_back(property);
}