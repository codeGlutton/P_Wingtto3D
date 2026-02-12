#include "pch.h"
#include "Texture.h"

#include "Manager/ResourceManager.h"

Texture2D::Texture2D()
{
	_mBulkData = std::make_shared<Texture2DBulkData>();
}

Texture2D::~Texture2D()
{
}

void Texture2D::PostCreate()
{
	Super::PostCreate();
	_mProxy = CreateRefCounting<std::shared_ptr<DXTextureBase>, MainThreadType::Render>();
}

void Texture2D::PostLoad()
{
	Super::PostLoad();
	_mBulkData->SerializeFromMemory();
	if (_mBulkData->IsArray() == true)
	{
		_mProxy->mData = RESOURCE_MANAGER->CreateRenderResource<DXConstTexture2DArray>(GetName(), GetFullPath());
	}
	else
	{
		_mProxy->mData = RESOURCE_MANAGER->CreateRenderResource<DXConstTexture2D>(GetName(), GetFullPath());
	}
	_mNeedInitProxy = true;

	// 게임 스레드에서 생성 후, 랜더 스레드에 일감 큐로 등록만 실시
	// 이후에 게임 스레드 업데이트 과정에서 해당 프록시를 GetProxy()를 통해 접근하는 경우, Init() 일감 큐로 등록  
}

const Texture2D::ProxyType& Texture2D::GetProxy() const
{
	if (_mNeedInitProxy == true)
	{
		_mNeedInitProxy = false;
		if (_mBulkData->IsArray() == true)
		{
			THREAD_MANAGER->PushRenderThreadLogicUpdateJob(ObjectPool<Job>::MakeShared(
				[bulkData = _mBulkData, renderResource = std::static_pointer_cast<DXConstTexture2DArray>(_mProxy->mData)]() {
					renderResource->Init(bulkData, 0, DXResourceUsageFlag::None);
				})
			);
		}
		else
		{
			THREAD_MANAGER->PushRenderThreadLogicUpdateJob(ObjectPool<Job>::MakeShared(
				[bulkData = _mBulkData, renderResource = std::static_pointer_cast<DXConstTexture2D>(_mProxy->mData)]() {
				renderResource->Init(bulkData, 0, DXResourceUsageFlag::None);
				}));
		}
	}
	return _mProxy;
}

