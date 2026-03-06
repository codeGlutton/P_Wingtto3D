#pragma once

#include "Core/Resource/Resource.h"
#include "Graphics/Resource/DXResource.h"
#include "Core/Resource/Package/PackageRuntimeOwner.h"

#include "Manager/ThreadManager.h"

#include "Utils/Memory/ObjectPool.h"

#define RESOURCE_MANAGER ResourceManager::GetInst()

DECLARE_MULTICAST_DELEGATE_1_PARAM(OnAddResourceHeader, std::shared_ptr<ResourceHeader>);

class ResourceHeader;
class ResourcePreviewPackage;
class ResourcePackage;

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
	void Save() const override;
	void Load() override;

	/* 게임 스레드 Only */
public:
	/**
	 * 전체적인 리소스들을 파악할 수 있도록 하기 위한 리소스 헤더 생성
	 * \param target 타겟 리소스
	 * \param flags 생성 플래그
	 * \return 리소스 헤더 객체
	 */
	std::shared_ptr<ResourceHeader> CreateResourceHeader(std::shared_ptr<Resource> target = nullptr, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
	std::shared_ptr<ResourceHeader> CreateResourceHeader(const std::wstring& objectName, std::shared_ptr<Resource> target = nullptr, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);

public:
	/**
	 * 게임 스레드 측에서 Object 파생 객체 리소스 가져오기. 필요시 패키징 로드
	 * \param resourcePath 리소스 경로
	 * \return 리소스 객체
	 */
	template<typename T> requires std::is_base_of_v<Resource, T>
	std::shared_ptr<T> LoadOrGetResource(const std::wstring& resourcePath);
	std::shared_ptr<Resource> LoadOrGetResource(const std::wstring& resourcePath, const ObjectTypeInfo* typeInfo);
	/**
	 * 게임 스레드 측에서 Object 파생 객체 리소스 가져오기. 필요시 패키징 비동기 로드
	 * \param resourcePath 리소스 경로
	 * \param callback 콜백 함수
	 * \return 리소스 객체
	 */
	template<typename T> requires std::is_base_of_v<Resource, T>
	void LoadOrGetResourceAsync(const std::wstring& resourcePath, std::function<void(std::shared_ptr<T>)> callback);
	void LoadOrGetResourceAsync(const std::wstring& resourcePath, const ObjectTypeInfo* typeInfo, std::function<void(std::shared_ptr<Resource>)> callback);

	/**
	 * 게임 스레드 리소스 생성
	 * \param package 연관된 리소스 패키징
	 * \param flags 생성 플래그
	 * \return 리소스 객체
	 */
	template<typename T> requires std::is_base_of_v<Resource, T>
	std::shared_ptr<T> CreateResource(std::shared_ptr<ResourcePackage> outer, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
	std::shared_ptr<Resource> CreateResource(std::shared_ptr<ResourcePackage> outer, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
	template<typename T> requires std::is_base_of_v<Resource, T>
	std::shared_ptr<T> CreateResource(const std::wstring& objectName, std::shared_ptr<ResourcePackage> outer, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
	std::shared_ptr<Resource> CreateResource(const std::wstring& objectName, std::shared_ptr<ResourcePackage> outer, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
	/**
	 * 렌더 스레드 리소스 생성 (생성 이후에는 랜더에서만 조작 가능)
	 * \param resourceName 리소스 이름
	 * \param resourcePath 리소스 경로
	 * \return 랜더 리소스 객체
	 */
	template<typename T> requires std::is_base_of_v<DXSharedResource, T>
	std::shared_ptr<T> CreateRenderResource(const std::wstring& resourceName, const std::wstring& resourcePath);

	bool IsLoadedResource(const std::wstring& resourcePath) const
	{
		return _mGameThreadResources.find(resourcePath) != _mGameThreadResources.end();
	}
	
	void NotifyToAddResourceHeader(std::shared_ptr<ResourceHeader> resourceHeader);
	void NotifyToAddResource(std::shared_ptr<Resource> resource);
	void NotifyToRemoveResource(const std::wstring& resourcePath);

	/* 렌더 스레드 Only */
public:
	/**
	 * 랜더 스레드 리소스 가져오기
	 * \param resourcePath 리소스 경로
	 * \param renderResourceType 리소스 타입
	 * \return 랜더 리소스 객체
	 */
	template<typename T> requires std::is_base_of_v<DXSharedResource, T>
	std::shared_ptr<T> GetRenderResource(const std::wstring& resourcePath, DXSharedResourceType::Type renderResourceType);
	/**
	 * 랜더 스레드 임시 리소스 생성 (이름 비명시적 부여)
	 * \param renderResourceType 리소스 타입
	 * \return 랜더 리소스 객체
	 */
	template<typename T> requires std::is_base_of_v<DXSharedResource, T>
	std::shared_ptr<T> CreateTransientRenderResource(DXSharedResourceType::Type renderResourceType);
	/**
	 * 랜더 스레드 런타임 리소스 생성
	 * \param name 리소스 이름
	 * \param renderResourceType 리소스 타입
	 * \return 랜더 리소스 객체
	 */
	template<typename T> requires std::is_base_of_v<DXSharedResource, T>
	std::shared_ptr<T> CreateOrGetRuntimeRenderResource(const std::wstring& name, DXSharedResourceType::Type renderResourceType);
	
	bool IsLoadedRenderResource(DXSharedResourceType::Type type, const std::wstring& resourcePath) const
	{
		return _mRenderThreadResources[type].find(resourcePath) != _mRenderThreadResources[type].end();
	}

	void NotifyToAddRenderResource(DXSharedResourceType::Type type, std::shared_ptr<DXSharedResource> resource);
	void NotifyToRemoveRenderResource(DXSharedResourceType::Type type, const std::wstring& resourcePath);

public:
	OnAddResourceHeader mOnAddResourceHeader;

private:
	std::shared_ptr<ResourcePreviewPackage> _mPackage;

	// 상시 존재할 헤더
	std::unordered_map<std::wstring, std::shared_ptr<ResourceHeader>> _mHeaders;
	std::unordered_map<const ObjectTypeInfo*, std::unordered_set<std::wstring>> _mClassFilter;

private:
	// 패키징 참조에 의해 로드될 리소스
	std::unordered_map<std::wstring, std::weak_ptr<Resource>> _mGameThreadResources;

private:
	// 렌더 스레드 리소스
	std::array<RenderThreadResourceMap, DXSharedResourceType::Count> _mRenderThreadResources;
};

template<typename T> requires std::is_base_of_v<Resource, T>
inline std::shared_ptr<T> ResourceManager::LoadOrGetResource(const std::wstring& resourcePath)
{
	return std::static_pointer_cast<T>(LoadOrGetResource(resourcePath, &T::GetStaticTypeInfo()));
}

template<typename T> requires std::is_base_of_v<Resource, T>
inline void ResourceManager::LoadOrGetResourceAsync(const std::wstring& resourcePath, std::function<void(std::shared_ptr<T>)> callback)
{
	LoadOrGetResourceAsync(resourcePath, &T::GetStaticTypeInfo(), [callback](std::shared_ptr<Resource> resource) {
		callback(std::static_pointer_cast<T>(resource));
		});
}

template<typename T> requires std::is_base_of_v<Resource, T>
inline std::shared_ptr<T> ResourceManager::CreateResource(std::shared_ptr<ResourcePackage> outer, ObjectCreateFlag::Type flags)
{
	return std::static_pointer_cast<T>(CreateResource(outer, &T::GetStaticTypeInfo(), flags));
}

template<typename T> requires std::is_base_of_v<Resource, T>
inline std::shared_ptr<T> ResourceManager::CreateResource(const std::wstring& objectName, std::shared_ptr<ResourcePackage> outer, ObjectCreateFlag::Type flags)
{
	return std::static_pointer_cast<T>(CreateResource(objectName, outer, &T::GetStaticTypeInfo(), flags));
}

template<typename T> requires std::is_base_of_v<DXSharedResource, T>
inline std::shared_ptr<T> ResourceManager::CreateRenderResource(const std::wstring& resourceName, const std::wstring& resourcePath)
{
	std::shared_ptr<T> renderResource = std::shared_ptr<T>(new T, [](T* p) {
		delete p;
		});
	renderResource->_mName = resourceName;
	renderResource->_mPath = resourcePath;

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
	renderResource->_mPath = fullPath;
	renderResource->PostCreate();

	return renderResource;
}

template<typename T> requires std::is_base_of_v<DXSharedResource, T>
inline std::shared_ptr<T> ResourceManager::CreateOrGetRuntimeRenderResource(const std::wstring& name, DXSharedResourceType::Type renderResourceType)
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
	renderResource->_mPath = fullPath;
	renderResource->PostCreate();

	return renderResource;
}
