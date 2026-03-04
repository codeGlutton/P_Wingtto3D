#include "pch.h"
#include "PackageManager.h"

#include "Manager/ThreadManager.h"
#include "Manager/PathManager.h"

#include "Core/Archive.h"
#include "Core/Resource/Package/Package.h"

#include "Utils/ObjectUtils.h"

void AsyncPackageStorage::Load(std::shared_ptr<Archive> archive)
{
	archive->LoadPackage();
	THREAD_MANAGER->PushGameThreadJob(ObjectPool<Job>::MakeShared([archive]() {
		PACKAGE_MANAGER->ResponseToLoadPackageAsync(archive);
		}));
}

void AsyncPackageStorage::Save(std::shared_ptr<Archive> archive)
{
	archive->SavePackage();
	THREAD_MANAGER->PushGameThreadJob(ObjectPool<Job>::MakeShared([archive]() {
		PACKAGE_MANAGER->ResponseToSavePackageAsync(archive);
		}));
}

PackageManager::PackageManager()
{
}

PackageManager::~PackageManager()
{
}

void PackageManager::Init()
{
	_mAsyncStorage = std::shared_ptr<AsyncPackageStorage>(new AsyncPackageStorage, [](AsyncPackageStorage* p) {
		delete p;
		});
}

void PackageManager::Destroy()
{
}

std::shared_ptr<Package> PackageManager::GetLoadedPackage(const std::wstring& packagePath) const
{
	auto iter = _mLoadedPackages.find(packagePath);
	if (iter != _mLoadedPackages.end())
	{
		return (*iter).second.lock();
	}
	return nullptr;
}

std::shared_ptr<Package> PackageManager::LoadPackage(const std::wstring& packagePath, const ObjectTypeInfo* packageTypeinfo, PackageBuildScope scope, std::shared_ptr<ObjectLinker> linker)
{
	bool needLinkData = true;
	std::shared_ptr<Package> result = nullptr;
	if (linker == nullptr)
	{
		needLinkData = false;
		linker = std::make_shared<ObjectLinker>();
	}

	// 이미 있는 리소스면 반환
	{
		result = GetLoadedPackage(packagePath);
		if (result != nullptr)
		{
			if (needLinkData == true)
			{
				result->MakeHeader(linker);
				result->CreateLinkData(linker);
			}
			return result;
		}
	}
	// 비동기 요청 중인 리소스인 경우
	{
		auto iter = _mWaitPackages.find(packagePath);
		if (iter != _mWaitPackages.end() && (*iter).second->mType == PackageAsyncOperationType::Load)
		{
			(*iter).second->mForcedSync = true;

			(*iter).second->mOnFinishPackageLoad.BindLambda([&result](std::shared_ptr<Package> package) {
				result = package;
				});

			for (iter = _mWaitPackages.find(packagePath); iter != _mWaitPackages.end();)
			{
				THREAD_MANAGER->DoGlobalJob();
				THREAD_MANAGER->DoGameJob();
			}

			if (needLinkData == true)
			{
				result->MakeHeader(linker);
				result->CreateLinkData(linker);
			}
			return result;
		}
	}

	// 파일 로드
	Archive archive(packagePath, scope);
	archive.BindObjectLinker(linker);
	archive.LoadPackage();

	// 빈 객체 생성
	result = std::static_pointer_cast<Package>(NewObject(packageTypeinfo, ConvertUtf8ToWString(packageTypeinfo->GetName()), packagePath));
	result->_mScope = scope;
	result->CreateEmptyObjects(linker);

	// 외부 종속 패키지 확인
	const std::vector<std::string>& otherPackageClassNames = result->GetExternalPackageClassNames(linker);
	const std::vector<std::wstring>& otherPackagePaths = result->GetExternalPackagePaths(linker);
	const std::vector<PackageBuildScope>& otherPackageScopes = result->GetExternalPackageScopes(linker);
	for (std::size_t i = 0; i < otherPackageClassNames.size(); ++i)
	{
		if (linker->mLinkDataMap[otherPackagePaths[i]].mPackageHeader != nullptr)
		{
			continue;
		}

		const ObjectTypeInfo* typeInfo = BOOT_SYSTEM->GetObjectTypeInfo(otherPackageClassNames[i].c_str());
		LoadPackage(otherPackagePaths[i], typeInfo, otherPackageScopes[i], linker);
	}

	// 리소스 로드
	result->Deserialize(archive);
	return result;
}

void PackageManager::LoadPackageAsync(const std::wstring& packagePath, const ObjectTypeInfo* typeinfo, std::function<void(std::shared_ptr<Package>)> callback, PackageBuildScope scope, std::shared_ptr<ObjectLinker> linker)
{
	bool needLinkData = true;
	std::shared_ptr<Package> result = nullptr;
	if (linker == nullptr)
	{
		needLinkData = false;
		linker = std::make_shared<ObjectLinker>();
	}

	// 이미 있는 리소스면 반환
	{
		result = GetLoadedPackage(packagePath);
		if (result != nullptr)
		{
			if (needLinkData == true)
			{
				result->MakeHeader(linker);
				result->CreateLinkData(linker);
			}
			callback(result);
			return;
		}
	}
	// 비동기 요청 중인 리소스인 경우
	{
		auto iter = _mWaitPackages.find(packagePath);
		if (iter != _mWaitPackages.end() && (*iter).second->mType == PackageAsyncOperationType::Load)
		{
			if (callback != nullptr)
			{
				(*iter).second->mOnFinishPackageLoad.BindLambda(callback);
			}
			return;
		}
	}

	// 리소스 로드
	std::shared_ptr<Archive> archive = std::make_shared<Archive>(packagePath, scope);
	archive->BindObjectLinker(linker);

	// 파일 로드 비동기 처리
	std::shared_ptr<PackageAsyncContext> context = std::make_shared<PackageAsyncContext>(PackageAsyncOperationType::Load);
	context->mPackageTypeinfo = typeinfo;
	if (callback != nullptr)
	{
		context->mOnFinishPackageLoad.BindLambda(callback);
	}

	_mWaitPackages[packagePath] = context;
	_mAsyncStorage->DoAsync(&AsyncPackageStorage::Load, archive);
}

void PackageManager::SavePackage(const std::wstring& packagePath, PackageBuildScope scope, std::shared_ptr<ObjectLinker> linker)
{
	std::shared_ptr<Package> package = GetLoadedPackage(packagePath);

	// 없는 리소스면 실패
	{
		if (package == nullptr)
		{
			return;
		}
	}
	// 비동기 요청 중인 리소스인 경우
	{
		auto iter = _mWaitPackages.find(packagePath);
		if (iter != _mWaitPackages.end() && (*iter).second->mType == PackageAsyncOperationType::Save)
		{
			(*iter).second->mForcedSync = true;
			for (iter = _mWaitPackages.find(packagePath); iter != _mWaitPackages.end();)
			{
				THREAD_MANAGER->DoGlobalJob();
				THREAD_MANAGER->DoGameJob();
			}
			return;
		}
	}

	// 현 헤더 데이터 수집
	if (linker == nullptr)
	{
		linker = std::make_shared<ObjectLinker>();
	}
	package->MakeHeader(linker);

	// 외부 종속 패키지 수집
	const std::vector<std::wstring>& otherPackagePaths = package->GetExternalPackagePaths(linker);
	const std::vector<PackageBuildScope>& otherPackageScopes = package->GetExternalPackageScopes(linker);
	for (std::size_t i = 0; i < otherPackagePaths.size(); ++i)
	{
		if (linker->mLinkDataMap[otherPackagePaths[i]].mPackageHeader != nullptr)
		{
			continue;
		}

		SavePackage(otherPackagePaths[i], otherPackageScopes[i], linker);
	}

	// 리소스 저장
	Archive archive(packagePath, scope);
	archive.BindObjectLinker(linker);
	package->Serialize(archive);

	// 파일 저장
	archive.SavePackage();
}

void PackageManager::SavePackageAsync(const std::wstring& packagePath, std::function<void()> callback, PackageBuildScope scope, std::shared_ptr<ObjectLinker> linker)
{
	std::shared_ptr<Package> package = GetLoadedPackage(packagePath);

	// 없는 리소스면 실패
	{
		if (package == nullptr)
		{
			return;
		}
	}
	// 비동기 요청 중인 리소스인 경우
	{
		auto iter = _mWaitPackages.find(packagePath);
		if (iter != _mWaitPackages.end() && (*iter).second->mType == PackageAsyncOperationType::Save)
		{
			if (callback != nullptr)
			{
				(*iter).second->mOnFinishPackageSave.BindLambda(callback);
			}
			return;
		}
	}

	// 현 헤더 데이터 수집
	if (linker == nullptr)
	{
		linker = std::make_shared<ObjectLinker>();
	}
	package->MakeHeader(linker);

	// 외부 종속 패키지 수집
	const std::vector<std::wstring>& otherPackagePaths = package->GetExternalPackagePaths(linker);
	const std::vector<PackageBuildScope>& otherPackageScopes = package->GetExternalPackageScopes(linker);
	for (std::size_t i = 0; i < otherPackagePaths.size(); ++i)
	{
		if (linker->mLinkDataMap[otherPackagePaths[i]].mPackageHeader != nullptr)
		{
			continue;
		}

		SavePackage(otherPackagePaths[i], otherPackageScopes[i], linker);
	}

	// 리소스 저장
	std::shared_ptr<Archive> archive = std::make_shared<Archive>(packagePath, scope);
	archive->BindObjectLinker(linker);
	package->Serialize(*archive);

	// 파일 로드 비동기 처리
	std::shared_ptr<PackageAsyncContext> context = std::make_shared<PackageAsyncContext>(PackageAsyncOperationType::Save);
	if (callback != nullptr)
	{
		context->mOnFinishPackageSave.BindLambda(callback);
	}

	_mWaitPackages[packagePath] = context;
	_mAsyncStorage->DoAsync(&AsyncPackageStorage::Save, archive);
}

void PackageManager::NotifyToAddPackage(std::shared_ptr<Package> package)
{
	ASSERT_MSG(package != nullptr, "Can't add nullptr package");

	std::wstring packagePath = package->GetPath();
	_mLoadedPackages[packagePath] = std::move(package);
}

void PackageManager::NotifyToRemovePackage(const std::wstring& packagePath)
{
	ASSERT_MSG(_mLoadedPackages.erase(packagePath) != 0, "Can't remove same package again");
}

void PackageManager::ResponseToLoadPackageAsync(std::shared_ptr<Archive> archive)
{
	std::wstring packagePath = archive->GetPackagePath().wstring();
	PackageBuildScope scope = archive->GetScope();
	auto iter = _mWaitPackages.find(packagePath);
	ASSERT_MSG(iter != _mWaitPackages.end() && (*iter).second != nullptr, "Async package callback can't find context");
	const std::shared_ptr<PackageAsyncContext>& context = (*iter).second;
	std::shared_ptr<ObjectLinker>& linker = archive->GetObjectLinker();

	// 빈 객체 생성
	std::shared_ptr<Package> result = std::static_pointer_cast<Package>(NewObject(context->mPackageTypeinfo, ConvertUtf8ToWString(context->mPackageTypeinfo->GetName()), packagePath));
	result->_mScope = scope;
	result->CreateEmptyObjects(linker);

	CheckToLoadPackageAsync(result, archive);
}

void PackageManager::CheckToLoadPackageAsync(std::shared_ptr<Package> package, std::shared_ptr<Archive> archive)
{
	std::wstring packagePath = archive->GetPackagePath().wstring();
	PackageBuildScope scope = archive->GetScope();
	auto iter = _mWaitPackages.find(packagePath);
	ASSERT_MSG(iter != _mWaitPackages.end() && (*iter).second != nullptr, "Async package callback can't find context");
	const std::shared_ptr<PackageAsyncContext>& context = (*iter).second;
	std::shared_ptr<ObjectLinker>& linker = archive->GetObjectLinker();

	// 외부 종속 패키지 확인
	bool isLoadOtherPackageAsync = false;
	const std::vector<std::string>& otherPackageClassNames = package->GetExternalPackageClassNames(linker);
	const std::vector<std::wstring>& otherPackagePaths = package->GetExternalPackagePaths(linker);
	const std::vector<PackageBuildScope>& otherPackageScopes = package->GetExternalPackageScopes(linker);
	for (std::size_t i = 0; i < otherPackageClassNames.size(); ++i)
	{
		if (linker->mLinkDataMap[otherPackagePaths[i]].mPackageHeader != nullptr)
		{
			continue;
		}

		const ObjectTypeInfo* typeInfo = BOOT_SYSTEM->GetObjectTypeInfo(otherPackageClassNames[i].c_str());
		if (context->mForcedSync == true)
		{
			LoadPackage(otherPackagePaths[i], typeInfo, otherPackageScopes[i], linker);
		}
		else
		{
			isLoadOtherPackageAsync = true;
			LoadPackageAsync(otherPackagePaths[i], typeInfo, [package, archive](std::shared_ptr<Package> otherPackage) {
				PACKAGE_MANAGER->CheckToLoadPackageAsync(package, archive);
				}, otherPackageScopes[i], linker);
		}
	}

	// 동기적으로 모든 연관 패키지가 준비되었을 때
	if (isLoadOtherPackageAsync == false)
	{
		package->Deserialize(*archive.get());
		context->mOnFinishPackageLoad.Multicast(package);
	}
}

void PackageManager::ResponseToSavePackageAsync(std::shared_ptr<Archive> archive)
{
	std::wstring packagePath = archive->GetPackagePath().wstring();
	auto iter = _mWaitPackages.find(packagePath);
	ASSERT_MSG(iter != _mWaitPackages.end() && (*iter).second != nullptr, "Async package callback can't find context");
	const std::shared_ptr<PackageAsyncContext>& context = (*iter).second;

	context->mOnFinishPackageSave.Multicast();
}
