#pragma once

#include "Graphics/Widget/Type/WidgetTypeInclude.h"

class Texture2D;

enum class WidgetBrushType
{
	None,
	Box,
	Border,
	Image,
};

struct WidgetBrush
{
	GEN_MINIMUM_STRUCT_REFLECTION(WidgetBrush)

public:
	PROPERTY(mType)
	WidgetBrushType mType;
	PROPERTY(mTint)
	WidgetColor mTint;
	PROPERTY(mImageSize)
	Vec2 mImageSize;
	PROPERTY(mTexture)
	std::shared_ptr<Texture2D> mTexture;

	/* Box, Border 모드에서 사용 */
public:
	PROPERTY(mMargin)
	Margin mMargin;

	/* Image 모드에서 사용 */
public:
	PROPERTY(mTiling)
	TilingType mTiling;
};

