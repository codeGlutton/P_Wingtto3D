#include "pch.h"
#include "ResourceManager.h"

#include "Manager/PathManager.h"
#include "Manager/PackageManager.h"

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
}

void ResourceManager::RegisterPackage(std::shared_ptr<Package> package)
{
	_mPackage = CastSharedPointer<ResourcePreviewPackage>(package);
}

void ResourceManager::Save()
{
	std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();
	packagePath.append(L"/Preview");
	PACKAGE_MANAGER->SavePackage(packagePath);
}

void ResourceManager::Load()
{
	std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();
	packagePath.append(L"/Preview");
	PACKAGE_MANAGER->LoadPackage<ResourcePreviewPackage>(packagePath);
}

std::shared_ptr<ResourceHeader> ResourceManager::CreateResourceHeader(std::shared_ptr<Resource> target, ObjectCreateFlag::Type flags)
{
	std::wstring typeName = ConvertUtf8ToWString(ResourceHeader::GetStaticTypeInfo().GetName());
	std::shared_ptr<ResourceHeader> resourceHeader = NewObject<ResourceHeader>(_mPackage.lock(), typeName, flags);
	resourceHeader->_mTarget = target;

	return resourceHeader;
}

void ResourceManager::NotifyToAddResourceHeader(std::shared_ptr<ResourceHeader> resourceHeader)
{
	ASSERT_MSG(resourceHeader == nullptr, "Can't add nullptr resource header");

	std::wstring resourcePath = resourceHeader->mResourcePath.GetPath();

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

	std::wstring resourcePath = resource->GetPath();

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



