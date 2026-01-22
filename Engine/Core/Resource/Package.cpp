#include "pch.h"
#include "Package.h"

#include "Utils/Thread/JobQueue.h"

#include "Manager/PackageManager.h"
#include "Manager/ThreadManager.h"

#include "Core/Object.h"
#include "Core/ObjectLinker.h"
#include "Core/Resource/BulkWrapper.h"

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
	PACKAGE_MANAGER->NotifyToRemovePackage(std::static_pointer_cast<Package>(shared_from_this()));
}

void Package::BeginDestroy()
{
	for (Object* p : _mChildObjects)
	{
		delete p;
	}

	Super::BeginDestroy();
}

void Package::Serialize(Archive& archive) const
{
	ASSERT_THREAD(MainThreadType::Game);

	PackageLinkData& linkData = archive.GetLinkData();

	archive.ChangeTarget(ArchiveMemoryTarget::Data);

	// 모든 객체부터 저장
	std::size_t objIndex = 0ull;
	for (std::weak_ptr<Object> weakObject : _mChildSharedObjects)
	{
		std::shared_ptr<Object> object = weakObject.lock();
		linkData.mObjectIndexMap[object.get()] = objIndex++;
		linkData.mObjects.push_back(object);
	}
	// 벌크 데이터 해석해 인덱스 저장
	std::size_t bulkIndex = 0ull;
	for (std::shared_ptr<BulkData>& bulkData : linkData.mPackageHeader->mBulkDatas)
	{
		linkData.mBulkDataIndexMap[bulkData.get()] = bulkIndex++;
	}
	// 모든 객체 프로퍼티 저장 시작
	for (Object const* object : _mChildObjects)
	{
		object->Serialize(archive);
	}
}

void Package::Deserialize(Archive& archive)
{
	ASSERT_THREAD(MainThreadType::Game);

	PackageLinkData& linkData = archive.GetLinkData();

	archive.ChangeTarget(ArchiveMemoryTarget::Data);

	// 프로퍼티 로드
	for (std::shared_ptr<Object>& object : linkData.mObjects)
	{
		object->Deserialize(archive);
	}
	// 로드 완료 알림
	for (std::shared_ptr<Object>& object : linkData.mObjects)
	{
		object->PostLoad();
	}
}

const std::vector<std::string>& Package::GetExternalPackageClassNames(std::shared_ptr<ObjectLinker> linker) const
{
	return linker->mLinkDataMap[GetPath()].mPackageHeader->mLinkedExternalPackageClassNames;
}

const std::vector<std::wstring>& Package::GetExternalPackagePaths(std::shared_ptr<ObjectLinker> linker) const
{
	return linker->mLinkDataMap[GetPath()].mPackageHeader->mLinkedExternalPackages;
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
	for (std::size_t i = 0; i < _mChildObjects.size(); ++i)
	{
		header->mObjectClassNames.push_back(_mChildObjects[i]->GetTypeInfo().GetName());
		if (_mChildSharedObjects[i].lock() != nullptr)
		{
			_mChildObjects[i]->GetTypeInfo().CollectHeaderDatas(
				_mChildObjects[i], externalPackageDatas,
				linkData.mPackageHeader->mBulkDatas
			);
		}
	}
	for (auto& externalPackageDataPair : externalPackageDatas)
	{
		header->mLinkedExternalPackages.push_back(externalPackageDataPair.first);
		header->mLinkedExternalPackageClassNames.push_back(externalPackageDataPair.second);
	}
	for (auto& bulkData : header->mBulkDatas)
	{
		header->mBulkClassNames.push_back(bulkData->GetTypeInfo().GetName());
	}
	linkData.mPackageHeader = header;
}

void Package::UpRefCount()
{
	WRITE_LOCK(_mLock);
	if (_mIsDeleted == false)
	{
		++_mRefCount;
	}
}

void Package::DownRefCount()
{
	WRITE_LOCK(_mLock);
	if (_mIsDeleted == false)
	{
		--_mRefCount;
		if (_mRefCount == 0)
		{
			_mIsDeleted = true;

			std::shared_ptr<Package> selfPackage = std::static_pointer_cast<Package>(shared_from_this());
			THREAD_MANAGER->PushGameThreadJob(ObjectPool<Job>::MakeShared([selfPackage]() {
				selfPackage->UnregisterPackage();
				}));
		}
	}
}

void Package::CreateEmptyObjects(std::shared_ptr<ObjectLinker> linker)
{
	ASSERT_THREAD(MainThreadType::Game);

	if (linker == nullptr)
	{
		return;
	}

	PackageLinkData& linkData = linker->mLinkDataMap[GetPath()];
	std::size_t objectSize = linkData.mPackageHeader->mObjectClassNames.size();
	{
		WRITE_LOCK(_mLock);
		_mRefCount += objectSize;
	}

	APP_WIN_MANAGER->RegisterPackage(std::static_pointer_cast<Package>(shared_from_this()));

	// 모든 객체 타입부터 해석해 생성
	std::size_t objIndex = 0ull;
	for (std::string& name : linkData.mPackageHeader->mObjectClassNames)
	{
		const ObjectTypeInfo* typeInfo = BOOT_SYSTEM->GetObjectTypeInfo(name.c_str());
		std::shared_ptr<Object> object = RequestToCreateObject(typeInfo, ObjectCreateFlag::DeferredLoad);
		_mChildObjects.push_back(object.get());
		_mChildSharedObjects.push_back(object);

		linkData.mObjectIndexMap[object.get()] = objIndex++;
		linkData.mObjects.push_back(object);
	}
	// 벌크 데이터 해석해 인덱스 저장
	std::size_t bulkIndex = 0ull;
	for (std::shared_ptr<BulkData>& bulkData : linkData.mPackageHeader->mBulkDatas)
	{
		linkData.mBulkDataIndexMap[bulkData.get()] = bulkIndex++;
	}
}

std::shared_ptr<Object> AppWindowPackage::RequestToCreateObject(const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	return APP_WIN_MANAGER->CreateAppWindow(nullptr, typeInfo, flags);
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
