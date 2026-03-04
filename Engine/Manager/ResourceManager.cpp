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
	std::shared_ptr<ResourceHeader> resourceHeader = NewObject<ResourceHeader>(_mPackage, objectName, flags);
	resourceHeader->_mTarget = target;

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
#ifdef _EDITOR
	if (package->IsValid() == false)
	{
		DEBUG_LOG("Try to load non existent resource");
		return nullptr;
	}
#else
	ASSERT_MSG(package->IsValid() == true, "Try to load non existent resource");
#endif
	return package->GetResource();
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
#ifdef _EDITOR
		if (package->IsValid() == false)
		{
			DEBUG_LOG("Try to load non existent resource. So create new package");
			std::shared_ptr<Resource> resource = RESOURCE_MANAGER->CreateResource(std::static_pointer_cast<ResourcePackage>(package), typeInfo, ObjectCreateFlag::DeferredLoad);
			callback(resource);
		}
#else
		ASSERT_MSG(package->IsValid() == true, "Try to load non existent resource");
#endif
		callback(std::static_pointer_cast<ResourcePackage>(package)->GetResource());
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
	ASSERT_MSG(resourceHeader != nullptr, "Can't add nullptr resource header");

	std::wstring resourcePath = resourceHeader->mResourcePtr.GetFullPath();

	// 이미 있는 리소스 헤더면 반환
	auto headerIter = _mHeaders.find(resourcePath);
	if (headerIter != _mHeaders.end())
	{
		return;
	}
	_mHeaders[resourcePath] = std::move(resourceHeader);
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



