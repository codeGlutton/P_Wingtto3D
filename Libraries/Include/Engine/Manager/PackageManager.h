#pragma once

#include "Utils/Thread/JobQueue.h"
#include "Core/CommonType/Delegate.h"
#include "Core/ObjectLinker.h"

#define PACKAGE_MANAGER PackageManager::GetInst()

class Package;

DECLARE_MULTICAST_DELEGATE_1_PARAM(OnFinishPackageLoad, std::shared_ptr<Package>);
DECLARE_MULTICAST_DELEGATE(OnFinishPackageSave);

enum class PackageAsyncOperationType : uint8
{
	Save,
	Load
};

struct PackageAsyncContext
{
public:
	PackageAsyncContext(PackageAsyncOperationType type) :
		mType(type)
	{
	}

public:
	const ObjectTypeInfo* mPackageTypeinfo = nullptr;

public:
	// 현재 비동기 로드 상태
	PackageAsyncOperationType mType;
	bool mForcedSync = false;

public:
	// 비동기 로드 후 콜백 이벤트들
	OnFinishPackageLoad mOnFinishPackageLoad;
	OnFinishPackageSave mOnFinishPackageSave;
};

/**
 * 패키징 비동기 로드용 Job Queue
 */
class AsyncPackageStorage : public ConcurrentJobQueue
{
	friend class PackageManager;

private:
	AsyncPackageStorage()
	{
	}

public:
	void Load(std::shared_ptr<Archive> archive);
	void Save(std::shared_ptr<Archive> archive);
};

class PackageManager
{
	friend class AsyncPackageStorage;

private:
	PackageManager();
	~PackageManager();

public:
	static PackageManager* GetInst()
	{
		static PackageManager inst;
		return &inst;
	}

public:
	void Init();
	void Destroy();

public:
	std::shared_ptr<Package> GetLoadedPackage(const std::wstring& packagePath) const;

public:
	template<typename T> requires std::is_base_of_v<Package, T>
	std::shared_ptr<T> LoadPackage(const std::wstring& packagePath, PackageBuildScope scope = PackageBuildScope::Shared, std::shared_ptr<ObjectLinker> linker = nullptr);
	std::shared_ptr<Package> LoadPackage(const std::wstring& packagePath, const ObjectTypeInfo* packageTypeinfo, PackageBuildScope scope = PackageBuildScope::Shared, std::shared_ptr<ObjectLinker> linker = nullptr);
	template<typename T> requires std::is_base_of_v<Package, T>
	void LoadPackageAsync(const std::wstring& packagePath, std::function<void(std::shared_ptr<Package>)> callback, PackageBuildScope scope = PackageBuildScope::Shared, std::shared_ptr<ObjectLinker> linker = nullptr);
	void LoadPackageAsync(const std::wstring& packagePath, const ObjectTypeInfo* typeinfo, std::function<void(std::shared_ptr<Package>)> callback, PackageBuildScope scope = PackageBuildScope::Shared, std::shared_ptr<ObjectLinker> linker = nullptr);

	void SavePackage(const std::wstring& packagePath, PackageBuildScope scope = PackageBuildScope::Shared, std::shared_ptr<ObjectLinker> linker = nullptr);
	void SavePackageAsync(const std::wstring& packagePath, std::function<void()> callback, PackageBuildScope scope = PackageBuildScope::Shared, std::shared_ptr<ObjectLinker> linker = nullptr);

public:
	void NotifyToAddPackage(std::shared_ptr<Package> package);
	void NotifyToRemovePackage(const std::wstring& packagePath);

private:
	void ResponseToLoadPackageAsync(std::shared_ptr<Archive> archive);
	void CheckToLoadPackageAsync(std::shared_ptr<Package> package, std::shared_ptr<Archive> archive);

	void ResponseToSavePackageAsync(std::shared_ptr<Archive> archive);

private:
	// 로드된 패키지
	std::unordered_map<std::wstring, std::shared_ptr<PackageAsyncContext>> _mWaitPackages;
	std::unordered_map<std::wstring, std::weak_ptr<Package>> _mLoadedPackages;

private:
	std::shared_ptr<AsyncPackageStorage> _mAsyncStorage;
};

template<typename T> requires std::is_base_of_v<Package, T>
std::shared_ptr<T> PackageManager::LoadPackage(const std::wstring& packagePath, PackageBuildScope scope, std::shared_ptr<ObjectLinker> linker)
{
	return CastSharedPointer<T>(LoadPackage(packagePath, &T::GetStaticTypeInfo(), scope, linker));
}

template<typename T> requires std::is_base_of_v<Package, T>
void PackageManager::LoadPackageAsync(const std::wstring& packagePath, std::function<void(std::shared_ptr<Package>)> callback, PackageBuildScope scope, std::shared_ptr<ObjectLinker> linker)
{
	LoadPackageAsync(packagePath, &T::GetStaticTypeInfo(), callback, scope, linker);
}