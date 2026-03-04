#pragma once

#include "Graphics/Widget/Type/WidgetTypeInclude.h"
#include "Core/Resource/Font.h"

class DXFontTexture;

/**
 * 텍스트 표기 정책
 */
enum class TextTransformPolicy
{
	None,
	Lowwer,
	Upper
};

/**
 * 텍스트 배치 정책
 */
enum class TextJustifyPolicy
{
	Left,	// 왼쪽에 몰아 배치
	Center, // 중앙에 몰아 배치
	Right,	// 오른쪽에 몰아 배치
};

struct WidgetFontData
{
	GEN_MINIMUM_STRUCT_REFLECTION(WidgetFontData)

public:
	bool operator==(const WidgetFontData& other) const
	{
		return _mFont == other._mFont &&
			_mPixelSize == other._mPixelSize &&
			_mCharSpacing == other._mCharSpacing;
	}

public:
	const std::shared_ptr<Font>& GetFont() const
	{
		return _mFont;
	}
	uint32 GetPixelSize() const
	{
		return _mPixelSize;
	}
	int32 GetCharSpacing() const
	{
		return _mCharSpacing;
	}

public:
	void SetFont(const std::shared_ptr<Font>& font)
	{
		_mFont = font;
		_mIsDirty = true;
	}
	void SetPixelSize(uint32 pixelSize)
	{
		_mPixelSize = pixelSize;
		_mIsDirty = true;
	}
	void SetCharSpacing(int32 spacing)
	{
		_mCharSpacing = spacing;
	}

public:
	bool IsValid() const
	{
		return _mFont != nullptr;
	}
	const FontAtlasDataRef& GetFontAtlasCahce() const;

private:
	PROPERTY(_mFont)
	std::shared_ptr<Font> _mFont;
	PROPERTY(_mPixelSize)
	uint32 _mPixelSize;
	PROPERTY(_mCharSpacing)
	int32 _mCharSpacing;

private:
	mutable bool _mIsDirty = true;
	mutable FontAtlasDataRef _mCache;
};

