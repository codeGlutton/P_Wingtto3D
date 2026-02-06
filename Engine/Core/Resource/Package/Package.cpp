#include "pch.h"
#include "Package.h"

#include "Utils/Thread/JobQueue.h"

#include "Core/Object.h"
#include "Core/ObjectLinker.h"
#include "Core/Resource/BulkData.h"
#include "Core/Resource/Package/PackageHeader.h"

#include "Manager/PackageManager.h"
#include "Manager/ThreadManager.h"

#include "Manager/AppWindowManager.h"

#include "Manager/ResourceManager.h"
#include "Core/Resource/ResourceHeader.h"

void Package::PostLoad()
{
	Super::PostLoad();
	RegisterPackage();
}

void Package::RegisterPackage()
{
	PACKAGE_MANAGER->NotifyToAddPackage(std::static_pointer_cast<Package>(shared_from_this()));
}

void Package::UnregisterPackage()
{
	PACKAGE_MANAGER->NotifyToRemovePackage(GetPath());
}

void Package::BeginDestroy()
{
	UnregisterPackage();
	Super::BeginDestroy();
}

void Package::Serialize(Archive& archive) const
{
	ASSERT_THREAD(MainThreadType::Game);

	PackageLinkData& linkData = archive.GetLinkData();
	
	archive.ChangeTarget(ArchiveMemoryTarget::Data);

	// 모든 객체부터 저장
	for (const std::wstring& objFullPath : linkData.mPackageHeader->mObjectFullPaths)
	{
		std::shared_ptr<Object> object = _mChildSharedObjects.find(objFullPath)->second.lock();
		linkData.mObjectFullPathMap[object.get()] = objFullPath;
		linkData.mObjectPtrMap[objFullPath] = object;
	}
	// 벌크 데이터 해석해 인덱스 저장
	std::size_t bulkIndex = 0ull;
	for (std::shared_ptr<BulkData>& bulkData : linkData.mPackageHeader->mBulkDatas)
	{
		linkData.mBulkDataIndexMap[bulkData.get()] = bulkIndex++;
	}
	// 헤더의 순서대로 모든 객체 프로퍼티 저장 시작
	for (const std::wstring& objFullPath : linkData.mPackageHeader->mObjectFullPaths)
	{
		linkData.mObjectPtrMap[objFullPath]->Serialize(archive);
	}
}

void Package::Deserialize(Archive& archive)
{
	ASSERT_THREAD(MainThreadType::Game);

	PackageLinkData& linkData = archive.GetLinkData();

	archive.ChangeTarget(ArchiveMemoryTarget::Data);

	// 헤더의 순서대로 프로퍼티 로드
	for (const std::wstring& objFullPath : linkData.mPackageHeader->mObjectFullPaths)
	{
		linkData.mObjectPtrMap[objFullPath]->Deserialize(archive);
	}
	// 로드 완료 알림
	for (const std::wstring& objFullPath : linkData.mPackageHeader->mObjectFullPaths)
	{
		linkData.mObjectPtrMap[objFullPath]->PostLoad();
	}
}

void Package::NotifyToAddChild(std::shared_ptr<Object> object)
{
	ASSERT_MSG(_mChildSharedObjects.find(object->GetFullPath()) == _mChildSharedObjects.end(), "Overlapped object register to package");
	_mChildSharedObjects[object->GetFullPath()] = object;
}

void Package::NotifyToRemoveChild(const std::wstring& objectFullPath)
{
	ASSERT_MSG(_mChildSharedObjects.find(objectFullPath) != _mChildSharedObjects.end(), "Invalid object can't unregister");
	_mChildSharedObjects.erase(objectFullPath);
}

const std::vector<std::string>& Package::GetExternalPackageClassNames(std::shared_ptr<ObjectLinker> linker) const
{
	return linker->mLinkDataMap[GetPath()].mPackageHeader->mLinkedExternalPackageClassNames;
}

const std::vector<std::wstring>& Package::GetExternalPackagePaths(std::shared_ptr<ObjectLinker> linker) const
{
	return linker->mLinkDataMap[GetPath()].mPackageHeader->mLinkedExternalPackagePaths;
}

void Package::MakeHeader(std::shared_ptr<ObjectLinker> linker) const
{
	ASSERT_THREAD(MainThreadType::Game);

	if (linker == nullptr)
	{
		return;
	}

	PackageLinkData& linkData = linker->mLinkDataMap[GetPath()];
	if (linkData.mPackageHeader != nullptr)
	{
		return;
	}

	// 외부 종속 패키지와 벌크 데이터 저장해두기
	std::shared_ptr<PackageHeader> header = std::make_shared<PackageHeader>();
	std::unordered_map<std::wstring, std::string> externalPackageDatas;
	for (auto& childPair : _mChildSharedObjects)
	{
		std::shared_ptr<Object> child = childPair.second.lock();

		header->mObjectClassNames.push_back(child->GetTypeInfo().GetName());
		header->mObjectFullPaths.push_back(child->GetFullPath());
		if (child != nullptr)
		{
			child->GetTypeInfo().CollectHeaderDatas(
				child.get(), externalPackageDatas,
				linkData.mPackageHeader->mBulkDatas
			);
		}
	}
	for (auto& externalPackageDataPair : externalPackageDatas)
	{
		header->mLinkedExternalPackagePaths.push_back(externalPackageDataPair.first);
		header->mLinkedExternalPackageClassNames.push_back(externalPackageDataPair.second);
	}
	for (auto& bulkData : header->mBulkDatas)
	{
		header->mBulkClassNames.push_back(bulkData->GetTypeInfo().GetName());
	}
	linkData.mPackageHeader = header;
}

void Package::CreateEmptyObjects(std::shared_ptr<ObjectLinker> linker)
{
	ASSERT_THREAD(MainThreadType::Game);

	if (linker == nullptr)
	{
		return;
	}

	PackageLinkData& linkData = linker->mLinkDataMap[GetPath()];

	// 모든 객체 타입부터 해석해 생성
	std::size_t objectSize = linkData.mPackageHeader->mObjectClassNames.size();
	for (std::size_t i = 0; i < objectSize; ++i)
	{
		const std::string& name = linkData.mPackageHeader->mObjectClassNames[i];
		const std::wstring& fullPath = linkData.mPackageHeader->mObjectFullPaths[i];

		const ObjectTypeInfo* typeInfo = BOOT_SYSTEM->GetObjectTypeInfo(name.c_str());
		std::shared_ptr<Object> object = RequestToCreateObject(typeInfo, ObjectCreateFlag::DeferredLoad);

		linkData.mObjectFullPathMap[object.get()] = fullPath;
		linkData.mObjectPtrMap[fullPath] = object;
	}
	// 벌크 데이터 해석해 인덱스 저장
	std::size_t bulkIndex = 0ull;
	for (std::shared_ptr<BulkData>& bulkData : linkData.mPackageHeader->mBulkDatas)
	{
		linkData.mBulkDataIndexMap[bulkData.get()] = bulkIndex++;
	}
}

std::shared_ptr<Object> Package::RequestToCreateObject(const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	std::wstring typeName = ConvertUtf8ToWString(typeInfo->GetName());
	return NewObject(std::static_pointer_cast<Package>(shared_from_this()), typeInfo, typeName, flags);
}

void AppWindowPackage::RegisterPackage()
{
	Super::RegisterPackage();
	APP_WIN_MANAGER->RegisterPackage(std::static_pointer_cast<Package>(shared_from_this()));
}

std::shared_ptr<Object> AppWindowPackage::RequestToCreateObject(const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	if (typeInfo->IsChildOf<AppWindow>() == true)
	{
		return APP_WIN_MANAGER->CreateAppWindow(nullptr, typeInfo, flags);
	}
	else if (typeInfo->IsChildOf<Widget>() == true)
	{
		return APP_WIN_MANAGER->CreateAppWindowWidget(nullptr, typeInfo, flags);
	}
	return Super::RequestToCreateObject(typeInfo, flags);
}

void ResourcePreviewPackage::RegisterPackage()
{
	Super::RegisterPackage();
	RESOURCE_MANAGER->RegisterPackage(std::static_pointer_cast<Package>(shared_from_this()));
}

std::shared_ptr<Object> ResourcePreviewPackage::RequestToCreateObject(const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	return RESOURCE_MANAGER->CreateResourceHeader(nullptr, flags);
}

std::shared_ptr<Object> ResourcePackage::RequestToCreateObject(const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	// TODO
	return std::shared_ptr<Object>();
}
