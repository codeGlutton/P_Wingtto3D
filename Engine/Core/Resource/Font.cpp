#include "pch.h"
#include "Font.h"

#include "Manager/ResourceManager.h"

Font::Font()
{
	_mBulkData = std::make_shared<FontAtlasBulkData>();
}

Font::~Font()
{
}

void Font::PostLoad()
{
	Super::PostLoad();
	_mProxies.reserve(_mBulkData->mValue.size());
	for (FontAtlasData& atlasData : _mBulkData->mValue)
	{
		std::wstring pixelSizeStr = L"_" + std::to_wstring(atlasData.mPixelSize);

		_mProxies.push_back(CreateRefCounting<std::shared_ptr<DXFontTexture>, MainThreadType::Render>());
		_mProxies.back()->mData = RESOURCE_MANAGER->CreateRenderResource<DXFontTexture>(GetName() + pixelSizeStr, GetPath() + pixelSizeStr);
	}
	_mNeedInitProxy = true;
}

bool Font::HasBulkData() const
{
	return _mBulkData->mIsLoad;
}

#ifdef _EDITOR

void Font::PushBulkData(std::shared_ptr<FontAtlasBulkData> bulkData)
{
	ASSERT_MSG(HasBulkData() == false, "Already bulk data is existed");

	_mBulkData = bulkData;
	PostLoad();
}

#endif

const std::vector<Font::ProxyType>& Font::GetProxies() const
{
	if (_mNeedInitProxy == true)
	{
		for (std::size_t i = 0; i < _mProxies.size(); ++i)
		{
			const Font::ProxyType& proxy = _mProxies[i];

			_mNeedInitProxy = false;
			THREAD_MANAGER->PushRenderThreadLogicUpdateJob(ObjectPool<Job>::MakeShared(
				[i, bulkData = _mBulkData, renderResource = proxy->mData]() {
					renderResource->Init(bulkData, i, 0, DXResourceUsageFlag::None);
				}));
		}
	}
	return _mProxies;
}

const Font::ProxyType& Font::GetProxy(uint32 pixelSize) const
{
	const std::vector<Font::ProxyType>& proxies = GetProxies();
	for (std::size_t i = 0; i < _mBulkData->mValue.size(); ++i)
	{
		// 저장된 텍스처의 픽셀 사이즈가 원하는 픽셀 사이즈 보다 큰 경우
		// 해당 텍스처 사용
		if (_mBulkData->mValue[i].mPixelSize > pixelSize)
		{
			return proxies[i];
		}
	}

	// 큰 글씨인 경우, 그나마 최대 크기 텍스처 사용
	return _mProxies.back();
}

FontAtlasDataRef Font::GetFontDataRef(uint32 pixelSize) const
{
	const std::vector<Font::ProxyType>& proxies = GetProxies();
	for (std::size_t i = 0; i < _mBulkData->mValue.size(); ++i)
	{
		if (_mBulkData->mValue[i].mPixelSize > pixelSize)
		{
			return FontAtlasDataRef(_mBulkData, _mBulkData->mValue[i]);
		}
	}
	return FontAtlasDataRef(_mBulkData, _mBulkData->mValue.back());
}

