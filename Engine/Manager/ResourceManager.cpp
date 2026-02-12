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
	_mPackage = nullptr;
}

void ResourceManager::RegisterPackage(std::shared_ptr<Package> package)
{
	_mPackage = CastSharedPointer<ResourcePreviewPackage>(package);
}

void ResourceManager::Save()
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
	std::shared_ptr<ResourceHeader> resourceHeader = NewObject<ResourceHeader>(_mPackage, typeName, flags);
	resourceHeader->_mTarget = target;

	return resourceHeader;
}

std::shared_ptr<Resource> ResourceManager::CreateOrGetResource(const std::wstring& resourcePath, ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	if (_mResources.find(resourcePath) != _mResources.end())
	{
		return _mResources[resourcePath].lock();
	}

	ASSERT_MSG(typeInfo->IsChildOf<Resource>() == true, "GetResource func is not allowed to create non Resource class");

	std::wstring typeName = ConvertUtf8ToWString(typeInfo->GetName());
	std::shared_ptr<Resource> resource = std::static_pointer_cast<Resource>(NewObject(_mPackage, typeInfo, typeName, flags));
	if (resource == nullptr)
	{
		return nullptr;
	}
	return resource;
}

void ResourceManager::NotifyToAddResourceHeader(std::shared_ptr<ResourceHeader> resourceHeader)
{
	ASSERT_MSG(resourceHeader == nullptr, "Can't add nullptr resource header");

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
	ASSERT_MSG(resource == nullptr, "Can't add nullptr resource");

	std::wstring resourcePath = resource->GetFullPath();

	// 이미 있는 리소스면 반환
	auto resourceIter = _mResources.find(resourcePath);
	if (resourceIter != _mResources.end())
	{
		return;
	}
	_mResources[resourcePath] = std::move(resource);
}

void ResourceManager::NotifyToRemoveResource(const std::wstring& resourcePath)
{
	std::size_t eraseCount = _mResources.erase(resourcePath);
}

void ResourceManager::NotifyToAddRenderResource(DXSharedResourceType::Type type, std::shared_ptr<DXSharedResource> resource)
{
	ASSERT_MSG(type > DXSharedResourceType::None && type < DXSharedResourceType::Count, "Invalid render resource type");
	ASSERT_MSG(resource == nullptr, "Can't add nullptr render resource");

	const std::wstring& resourcePath = resource->GetFullPath();

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
	ASSERT_MSG(type > DXSharedResourceType::None && type < DXSharedResourceType::Count, "Invalid render resource type");

	std::size_t eraseCount = _mRenderThreadResources[type].erase(resourcePath);
}



