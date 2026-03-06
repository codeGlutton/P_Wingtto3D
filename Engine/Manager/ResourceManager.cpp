#include "pch.h"
#include "ResourceManager.h"

#include "Manager/PathManager.h"
#include "Manager/PackageManager.h"

#include "Core/Resource/Package/Package.h"
#include "Core/Resource/Package/PackageHeader.h"

#include "Core/Resource/ResourceHeader.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::Init()
{
#ifdef _EDITOR
	Load();
#endif
}

void ResourceManager::Destroy()
{
#ifdef _EDITOR
	Save();
#endif
	mOnAddResourceHeader.Clear();
	
	_mHeaders.clear();
	_mPackage = nullptr;
}

void ResourceManager::RegisterPackage(std::shared_ptr<Package> package)
{
	_mPackage = CastSharedPointer<ResourcePreviewPackage>(package);
}

void ResourceManager::Save() const
{
	std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();
	packagePath.append(L"\\Preview");
	PACKAGE_MANAGER->SavePackage(packagePath);
}

void ResourceManager::Load()
{
	std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();
	packagePath.append(L"\\Preview");
	PACKAGE_MANAGER->LoadPackage<ResourcePreviewPackage>(packagePath);
}

std::shared_ptr<ResourceHeader> ResourceManager::CreateResourceHeader(std::shared_ptr<Resource> target, ObjectCreateFlag::Type flags)
{
	std::wstring typeName = ConvertUtf8ToWString(ResourceHeader::GetStaticTypeInfo().GetName());
	return CreateResourceHeader(typeName, target, flags);
}

std::shared_ptr<ResourceHeader> ResourceManager::CreateResourceHeader(const std::wstring& objectName, std::shared_ptr<Resource> target, ObjectCreateFlag::Type flags)
{
	bool isDeferredRequest = (flags & ObjectCreateFlag::DeferredLoad);
	std::shared_ptr<ResourceHeader> resourceHeader = NewObject<ResourceHeader>(_mPackage, objectName, ObjectCreateFlag::Type(flags | ObjectCreateFlag::DeferredLoad));
	if (target != nullptr)
	{
		resourceHeader->mResourceName = target->GetName();
		resourceHeader->mResourcePath = target;
		resourceHeader->mResourceTypeInfo = &target->GetTypeInfo();
	}
	if (isDeferredRequest == false)
	{
		resourceHeader->PostLoad();
	}
	return resourceHeader;
}

std::shared_ptr<Resource> ResourceManager::LoadOrGetResource(const std::wstring& resourcePath, const ObjectTypeInfo* typeInfo)
{
	if (_mGameThreadResources.find(resourcePath) != _mGameThreadResources.end())
	{
		return _mGameThreadResources[resourcePath].lock();
	}

	ASSERT_MSG(typeInfo->IsChildOf<Resource>() == true, "GetResource func is not allowed to create non Resource class");
	
	std::shared_ptr<ResourcePackage> package = PACKAGE_MANAGER->LoadPackage<ResourcePackage>(resourcePath);
	std::shared_ptr<Resource> resource = package->GetResource();
#ifdef _EDITOR
	if (resource == nullptr)
	{
		DEBUG_LOG("Try to load non existent resource");
	}
#else
	ASSERT_MSG(resource != nullptr, "Try to load non existent resource");
#endif
	return resource;
}

void ResourceManager::LoadOrGetResourceAsync(const std::wstring& resourcePath, const ObjectTypeInfo* typeInfo, std::function<void(std::shared_ptr<Resource>)> callback)
{
	if (_mGameThreadResources.find(resourcePath) != _mGameThreadResources.end())
	{
		callback(_mGameThreadResources[resourcePath].lock());
		return;
	}

	ASSERT_MSG(typeInfo->IsChildOf<Resource>() == true, "GetResource func is not allowed to create non Resource class");

	PACKAGE_MANAGER->LoadPackageAsync<ResourcePackage>(resourcePath, [callback, typeInfo](std::shared_ptr<Package> package) {
		std::shared_ptr<Resource> resource = std::static_pointer_cast<ResourcePackage>(package)->GetResource();
#ifdef _EDITOR
		if (resource == nullptr)
		{
			DEBUG_LOG("Try to load non existent resource");
		}
#else
		ASSERT_MSG(resource != nullptr, "Try to load non existent resource");
#endif
		callback(resource);
		});
}

std::shared_ptr<Resource> ResourceManager::CreateResource(std::shared_ptr<ResourcePackage> outer, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	std::wstring typeName = ConvertUtf8ToWString(typeInfo->GetName());
	return CreateResource(typeName, outer, typeInfo, flags);
}

std::shared_ptr<Resource> ResourceManager::CreateResource(const std::wstring& objectName, std::shared_ptr<ResourcePackage> outer, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	ASSERT_MSG(typeInfo->IsChildOf<Resource>() == true, "GetResource func is not allowed to create non Resource class");

	std::wstring typeName = ConvertUtf8ToWString(typeInfo->GetName());
	std::shared_ptr<Resource> resource = std::static_pointer_cast<Resource>(NewObject(outer, typeInfo, typeName, flags));
	if (resource == nullptr)
	{
		return nullptr;
	}
	return resource;
}

void ResourceManager::NotifyToAddResourceHeader(std::shared_ptr<ResourceHeader> resourceHeader)
{
	ASSERT_MSG(resourceHeader != nullptr && resourceHeader->mResourceTypeInfo != nullptr, "Can't add nullptr resource header");
	std::wstring resourcePath = resourceHeader->mResourcePath.GetFullPath();

	// 이미 있는 리소스 헤더면 반환
	auto headerIter = _mHeaders.find(resourcePath);
	if (headerIter != _mHeaders.end())
	{
		return;
	}
	_mHeaders[resourcePath] = resourceHeader;
	_mClassFilter[resourceHeader->mResourceTypeInfo.Get()].insert(resourcePath);

	mOnAddResourceHeader.Multicast(resourceHeader);
}

void ResourceManager::NotifyToAddResource(std::shared_ptr<Resource> resource)
{
	ASSERT_MSG(resource != nullptr, "Can't add nullptr resource");

	std::wstring resourcePath = resource->GetPath();

	// 이미 있는 리소스면 반환
	auto resourceIter = _mGameThreadResources.find(resourcePath);
	if (resourceIter != _mGameThreadResources.end())
	{
		return;
	}
	_mGameThreadResources[resourcePath] = std::move(resource);
}

void ResourceManager::NotifyToRemoveResource(const std::wstring& resourcePath)
{
	std::size_t eraseCount = _mGameThreadResources.erase(resourcePath);
}

void ResourceManager::NotifyToAddRenderResource(DXSharedResourceType::Type type, std::shared_ptr<DXSharedResource> resource)
{
	ASSERT_MSG(type >= 0 && type < DXSharedResourceType::Count, "Invalid render resource type");
	ASSERT_MSG(resource != nullptr, "Can't add nullptr render resource");

	const std::wstring& resourcePath = resource->GetPath();

	// 이미 있는 리소스면 반환
	auto resourceIter = _mRenderThreadResources[type].find(resourcePath);
	if (resourceIter != _mRenderThreadResources[type].end())
	{
		return;
	}
	_mRenderThreadResources[type][resourcePath] = std::move(resource);
}

void ResourceManager::NotifyToRemoveRenderResource(DXSharedResourceType::Type type, const std::wstring& resourcePath)
{
	ASSERT_MSG(type >= 0 && type < DXSharedResourceType::Count, "Invalid render resource type");

	std::size_t eraseCount = _mRenderThreadResources[type].erase(resourcePath);
}



