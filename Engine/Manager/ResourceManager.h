#pragma once

#include "Core/Resource/Resource.h"
#include "Core/Resource/Package.h"

#define RESOURCE_MANAGER ResourceManager::GetInst()

class ResourceHeader;

class ResourceManager : public IPackageRuntimeOwner
{
private:
	ResourceManager();
	~ResourceManager();

public:
	static ResourceManager* GetInst()
	{
		static ResourceManager inst;
		return &inst;
	}

public:
	void Init();
	void Destroy();

public:
	void RegisterPackage(std::shared_ptr<Package> package) override;
	void Save() override;
	void Load() override;

public:
	std::shared_ptr<ResourceHeader> CreateResourceHeader(std::shared_ptr<Resource> target = nullptr, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);

public:
	bool IsLoadedResource(const std::wstring& resourcePath) const
	{
		return _mResources.find(resourcePath) != _mResources.end();
	}

	void NotifyToAddResourceHeader(std::shared_ptr<ResourceHeader> resourceHeader);
	void NotifyToAddResource(std::shared_ptr<Resource> resource);
	void NotifyToRemoveResource(const std::wstring& resourcePath);

private:
	std::weak_ptr<ResourcePreviewPackage> _mPackage;

	// 상시 존재할 헤더
	std::unordered_map<std::wstring, std::shared_ptr<ResourceHeader>> _mHeaders;
	std::unordered_map<const ObjectTypeInfo*, std::unordered_set<std::wstring>> _mClassFilter;

private:
	// 참조에 의해 로드될 리소스
	std::unordered_map<std::wstring, std::weak_ptr<Resource>> _mResources;
};


