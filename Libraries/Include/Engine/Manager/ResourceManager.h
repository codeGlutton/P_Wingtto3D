#pragma once

#include "Core/Resource/Resource.h"
#include "Graphics/Resource/DXResource.h"
#include "Core/Resource/Package/PackageRuntimeOwner.h"

#include "Manager/ThreadManager.h"

#include "Utils/Memory/ObjectPool.h"

#define RESOURCE_MANAGER ResourceManager::GetInst()

class ResourceHeader;
class ResourcePreviewPackage;

class ResourceManager : public IPackageRuntimeOwner
{
	using RenderThreadResourceMap = std::unordered_map<std::wstring, std::weak_ptr<DXSharedResource>>;

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

	/* 게임 스레드 Only */
public:
	template<typename T> requires std::is_base_of_v<Resource, T>
	std::shared_ptr<T> CreateOrGetResource(const std::wstring& resourcePath, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
	std::shared_ptr<Resource> CreateOrGetResource(const std::wstring& resourcePath, ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);

	template<typename T> requires std::is_base_of_v<DXSharedResource, T>
	std::shared_ptr<T> CreateRenderResource(const std::wstring& resourceName, const std::wstring& resourcePath);

	bool IsLoadedResource(const std::wstring& resourcePath) const
	{
		return _mResources.find(resourcePath) != _mResources.end();
	}
	
	void NotifyToAddResourceHeader(std::shared_ptr<ResourceHeader> resourceHeader);
	void NotifyToAddResource(std::shared_ptr<Resource> resource);
	void NotifyToRemoveResource(const std::wstring& resourcePath);

	/* 렌더 스레드 Only */
public:
	template<typename T> requires std::is_base_of_v<DXSharedResource, T>
	std::shared_ptr<T> GetRenderResource(const std::wstring& resourcePath, DXSharedResourceType::Type renderResourceType);
	template<typename T> requires std::is_base_of_v<DXSharedResource, T>
	std::shared_ptr<T> CreateTransientRenderResource(DXSharedResourceType::Type renderResourceType);
	template<typename T> requires std::is_base_of_v<DXSharedResource, T>
	std::shared_ptr<T> CreateOrGetTransientRenderResource(const std::wstring& name, DXSharedResourceType::Type renderResourceType);
	
	bool IsLoadedRenderResource(DXSharedResourceType::Type type, const std::wstring& resourcePath) const
	{
		return _mRenderThreadResources[type].find(resourcePath) != _mRenderThreadResources[type].end();
	}

	void NotifyToAddRenderResource(DXSharedResourceType::Type type, std::shared_ptr<DXSharedResource> resource);
	void NotifyToRemoveRenderResource(DXSharedResourceType::Type type, const std::wstring& resourcePath);

private:
	std::shared_ptr<ResourcePreviewPackage> _mPackage;

	// 상시 존재할 헤더
	std::unordered_map<std::wstring, std::shared_ptr<ResourceHeader>> _mHeaders;
	std::unordered_map<const ObjectTypeInfo*, std::unordered_set<std::wstring>> _mClassFilter;

private:
	// 패키징 참조에 의해 로드될 리소스
	std::unordered_map<std::wstring, std::weak_ptr<Resource>> _mResources;

private:
	// 렌더 스레드 리소스
	std::array<RenderThreadResourceMap, DXSharedResourceType::Count> _mRenderThreadResources;
};

template<typename T> requires std::is_base_of_v<Resource, T>
inline std::shared_ptr<T> ResourceManager::CreateOrGetResource(const std::wstring& resourcePath, ObjectCreateFlag::Type flags)
{
	return std::static_pointer_cast<T>(GetResource(resourcePath, &T::GetStaticTypeInfo(), flags));
}

template<typename T> requires std::is_base_of_v<DXSharedResource, T>
inline std::shared_ptr<T> ResourceManager::CreateRenderResource(const std::wstring& resourceName, const std::wstring& resourcePath)
{
	std::shared_ptr<T> renderResource = std::shared_ptr<T>(new T, [](T* p) {
		delete p;
		});
	renderResource->_mName = resourceName;
	renderResource->_mFullPath = resourcePath;

	THREAD_MANAGER->PushRenderThreadLogicUpdateJob(ObjectPool<Job>::MakeShared([renderResource = renderResource]() {
		renderResource->PostCreate();
		}));

	return renderResource;
}

template<typename T> requires std::is_base_of_v<DXSharedResource, T>
inline std::shared_ptr<T> ResourceManager::GetRenderResource(const std::wstring& resourcePath, DXSharedResourceType::Type renderResourceType)
{
	RenderThreadResourceMap& resourceMap = _mRenderThreadResources[renderResourceType];
	if (resourceMap.find(resourcePath) != resourceMap.end())
	{
		return std::static_pointer_cast<T>(resourceMap[resourcePath].lock());
	}
	return nullptr;
}

template<typename T> requires std::is_base_of_v<DXSharedResource, T>
inline std::shared_ptr<T> ResourceManager::CreateTransientRenderResource(DXSharedResourceType::Type renderResourceType)
{
	static uint64 transientName = 0ull;
	const std::wstring name = std::to_wstring(transientName++);
	const std::wstring fullPath = L"Transient/" + name;

	std::shared_ptr<T> renderResource = std::shared_ptr<T>(new T, [](T* p) {
		delete p;
		});
	renderResource->_mName = name;
	renderResource->_mFullPath = fullPath;
	renderResource->PostCreate();

	return renderResource;
}

template<typename T> requires std::is_base_of_v<DXSharedResource, T>
inline std::shared_ptr<T> ResourceManager::CreateOrGetTransientRenderResource(const std::wstring& name, DXSharedResourceType::Type renderResourceType)
{
	const std::wstring fullPath = L"Transient/" + name;

	RenderThreadResourceMap& resourceMap = _mRenderThreadResources[renderResourceType];
	if (resourceMap.find(fullPath) != resourceMap.end())
	{
		return std::static_pointer_cast<T>(resourceMap[fullPath].lock());
	}

	std::shared_ptr<T> renderResource = std::shared_ptr<T>(new T, [](T* p) {
		delete p;
		});
	renderResource->_mName = name;
	renderResource->_mFullPath = fullPath;
	renderResource->PostCreate();

	return renderResource;
}
