#pragma once

#include "Core/Resource/Resource.h"
#include "Graphics/Resource/DXTexture.h"

#include "Utils/Thread/RefCounting.h"

struct FontAtlasDataRef
{
	friend class Font;

public:
	FontAtlasDataRef()
	{
	}

private:
	FontAtlasDataRef(std::shared_ptr<FontAtlasBulkData> bulkData, const FontAtlasData& atlasData) :
		_mAtlasData(&atlasData),
		_mBulkData(bulkData)
	{
	}

public:
	const FontAtlasData& GetAtlasData() const
	{
		return *_mAtlasData;
	}

private:
	const FontAtlasData* _mAtlasData = nullptr;
	std::shared_ptr<FontAtlasBulkData> _mBulkData;
};

class Font : public Resource
{
	GEN_REFLECTION(Font)

public:
	using ProxyType = ThreadSafeRefCounting<std::shared_ptr<DXFontTexture>, MainThreadType::Render>;

public:
	Font();
	~Font();

protected:
	virtual void PostLoad() override;

public:
	virtual bool HasBulkData() const override;

#ifdef _EDITOR

public:
	void PushBulkData(std::shared_ptr<FontAtlasBulkData> bulkData);

#endif // _EDITOR

public:
	const std::vector<ProxyType>& GetProxies() const;
	const ProxyType& GetProxy(uint32 pixelSize) const;
	
	FontAtlasDataRef GetFontDataRef(uint32 pixelSize) const;

private:
	PROPERTY(_mBulkData)
	std::shared_ptr<FontAtlasBulkData> _mBulkData;

	// 렌더 스레드에서 안전 제거되는 텍스처 데이터
	std::vector<ProxyType> _mProxies;
	mutable bool _mNeedInitProxy = false;
};

