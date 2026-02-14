#pragma once

#include "Graphics/Widget/Type/WidgetTypeInclude.h"
#include "Utils/Thread/MainThread.h"

class DXTextureBase;
template<typename T, MainThreadType::Type MainThreadT> requires (MainThreadT == MainThreadType::Game || MainThreadT == MainThreadType::Render)
struct ThreadSafeRefCountData;

enum class WidgetBrushType
{
	None,
	Box,
	Border,
	Image,
};

struct WidgetInheritedColor
{
public:
	WidgetInheritedColor& BlendColor(const Color& color)
	{
		mInheritedColor *= color;
		return *this;
	}
	WidgetInheritedColor& BlendOpacity(float alpha)
	{
		mInheritedColor.w *= alpha;
		return *this;
	}
	WidgetInheritedColor& SetForegroundColor(const Color& color)
	{
		mForegroundColor = color;
		return *this;
	}

public:
	// 반드시 적용할 결과적 상속 색
	Color mInheritedColor = Color::White;
	// 선택적 적용할 부모 전달 색
	Color mForegroundColor = Color::White;
};

struct Tint
{
	GEN_MINIMUM_STRUCT_REFLECTION(Tint)

public:
	Color GetColor(const WidgetInheritedColor& inheritedColor) const;

public:
	PROPERTY(mColor)
	Color mColor = Color::White;
	PROPERTY(mUseInheritedColor)
	bool mUseInheritedColor = false;
};

struct WidgetBrush
{
	GEN_MINIMUM_STRUCT_REFLECTION(WidgetBrush)

public:
	using Proxy = std::shared_ptr<ThreadSafeRefCountData<std::shared_ptr<DXTextureBase>, MainThreadType::Render>>;

public:
	PROPERTY(mType)
	WidgetBrushType mType;
	PROPERTY(mTint)
	Tint mTint;
	PROPERTY(mImageSize)
	Vec2 mImageSize;
	PROPERTY(mProxy)
	Proxy mProxy;

	/* Box, Border 모드에서 사용 */
public:
	PROPERTY(mMargin)
	Margin mMargin;

	/* Image 모드에서 사용 */
public:
	PROPERTY(mTiling)
	TilingType mTiling;
};

