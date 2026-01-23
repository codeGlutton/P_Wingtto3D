#include "pch.h"
#include "StructTypeInfo.h"

#include "Core/Resource/BulkData.h"
#include "Core/Resource/Package/Package.h"

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

void StructTypeInfo::CollectHeaderDatas(const void* inst, OUT std::unordered_map<std::wstring, std::string> externalPackageDatas, OUT std::vector<std::shared_ptr<BulkData>>& bulkDatas) const
{
	// 부모를 거쳐가면서 패키징 헤더에 필요한 데이터 추출
	for (const StructTypeInfo* currentInfo = this; currentInfo != nullptr; currentInfo = currentInfo->GetSuper())
	{
		const PropertyMap& propertyMap = currentInfo->_mPropertyMap;
		for (const auto& propertyPair : propertyMap)
		{
			const HardRefTypeInfo* hardRefTypeInfo = Cast<const HardRefTypeInfo>(&propertyPair.second->GetTypeInfo());
			if (hardRefTypeInfo != nullptr)
			{
				std::shared_ptr<Package> instPackage = hardRefTypeInfo->GetInstancePackage(propertyPair.second->GetRawPtr(inst));
				if (instPackage != nullptr)
				{
					externalPackageDatas[instPackage->GetPath()] = instPackage->GetTypeInfo().GetName();
				}
			}
			else
			{
				const BulkBaseTypeInfo* bulkTypeInfo = Cast<const BulkBaseTypeInfo>(&propertyPair.second->GetTypeInfo());
				if (bulkTypeInfo != nullptr)
				{
					const std::shared_ptr<BulkData>& bulkData = *reinterpret_cast<const std::shared_ptr<BulkData>*>(propertyPair.second->GetRawPtr(inst));
					bulkDatas.push_back(bulkData);
				}
				else
				{
					const StructTypeInfo* structTypeInfo = Cast<const StructTypeInfo>(&propertyPair.second->GetTypeInfo());
					if (structTypeInfo != nullptr)
					{
						structTypeInfo->CollectHeaderDatas(propertyPair.second->GetRawPtr(inst), externalPackageDatas, bulkDatas);
					}
				}
			}
		}
	}
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
	// 부모를 거쳐가면서 전부 포함
	std::vector<const std::pair<const std::string_view, const Property*>*> changes;
	for (const StructTypeInfo* currentInfo = this; currentInfo != nullptr; currentInfo = currentInfo->GetSuper())
	{
		const PropertyMap& propertyMap = currentInfo->_mPropertyMap;
		for (const auto& propertyPair : propertyMap)
		{
			changes.push_back(&propertyPair);
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

void StructTypeInfo::Deserialize(Archive& archive, OUT void* inst) const
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
		for (const StructTypeInfo* currentInfo = this; currentInfo != nullptr; currentInfo = currentInfo->GetSuper())
		{
			const PropertyMap& propertyMap = currentInfo->_mPropertyMap;
			auto iter = propertyMap.find(propertyName);
			if (iter != propertyMap.end())
			{
				void* propertyPtr = iter->second->GetRawPtr(inst);
				iter->second->GetTypeInfo().Deserialize(archive, propertyPtr);
			}
		}
	}
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