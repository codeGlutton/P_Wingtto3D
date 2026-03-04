#pragma once

#include "Core/Resource/Resource.h"
#include "Graphics/Resource/DXTexture.h"

#include "Utils/Thread/RefCounting.h"

class Texture2D : public Resource
{
	GEN_REFLECTION(Texture2D)

public:
	using ProxyType = ThreadSafeRefCounting<std::shared_ptr<DXTextureBase>, MainThreadType::Render>;

public:
	Texture2D();
	~Texture2D();

protected:
	virtual void PostCreate() override;
	virtual void PostLoad() override;

public:
	virtual bool HasBulkData() const override;

#ifdef _EDITOR

public:
	void PushBulkData(std::shared_ptr<Texture2DBulkData> bulkData);

#endif

public:
	const ProxyType& GetProxy() const;

public:
	Vec2 GetSize() const
	{
		return Vec2(static_cast<float>(_mBulkData->mMetaData.width), static_cast<float>(_mBulkData->mMetaData.height));
	}

private:
	PROPERTY(_mBulkData)
	std::shared_ptr<Texture2DBulkData> _mBulkData;

	// 렌더 스레드에서 안전 제거되는 텍스처 데이터
	ProxyType _mProxy;
	mutable bool _mNeedInitProxy = false;
};

