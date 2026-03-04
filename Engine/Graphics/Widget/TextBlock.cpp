#include "pch.h"
#include "TextBlock.h"

#include "Core/Resource/Texture.h"
#include "Graphics/Render/WidgetDrawBuffer.h"

void TextBlock::OnConstruct(const Arguments& args)
{
	Widget::OnConstruct(args);

	if (args.mStyle != nullptr)
	{
		_mStyle = args.mStyle;

		WidgetFontData fontData;
		fontData.SetFont(args.mStyle->mFont);
		fontData.SetPixelSize(24u);
		fontData.SetCharSpacing(args.mStyle->mCharSpacing);
		SetFontData(fontData);
		_mBackgroundColor.Set(args.mStyle->mBackgroundColor);
	}

	SetText(args.mText);
	SetTransformPolicy(args.mTransformPolicy);
	SetJustifyPolicy(args.mJustifyPolicy);
	SetFontData(args.mFontData);
	SetTextMargin(args.mTextMargin);

	SetBackgroundColor(args.mBackgroundColor);
	
	SetMinDesireWidth(args.mMinDesiredWidth);
	SetAutoWrap(args.mAutoWrap);

	if (args.mOnDoubleClickMouse.IsBound() == true)
	{
		mOnPressMouse = args.mOnDoubleClickMouse;
	}
}

TextBlock::TextBlock()
{
	_mVisibility.Init(VisibilityType::Visible);
}

WidgetSlotContainer& TextBlock::GetChildren()
{
	return EmptySlotContainer::GetInst();
}

const WidgetSlotContainer& TextBlock::GetChildren() const
{
	return EmptySlotContainer::GetInst();
}

void TextBlock::OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const
{
}

Vec2 TextBlock::ComputeDesireSize(const Vec2& layoutMultiplyValue)
{
	// 레이아웃이 달라졌으므로 필수적으로 갱신해야함
	_mTextCache.MarkLayoutDirty();

	// 미리 스케일링해서 레이아웃 데이터, 렌더 데이터 저장
	_mTextCache.UpdateOnPrepass(
		GetText(), 
		GetTransformPolicy(),
		GetFontData(), 
		GetMargin(), 
		layoutMultiplyValue
	);
	
	const Vec2 textSize = _mTextCache.GetDesiredSingleLineSize();
	return Vec2(std::max<float>(_mMinDesiredWidth.GetValue(this), textSize.x), textSize.y);
}

uint32 TextBlock::OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor)
{
	// 배정된 사이즈와 비교해서 실제 랜더링 데이터 계산
	_mTextCache.UpdateOnPaint(allottedGeometry.mBoxSize, GetJustifyPolicy(), GetAutoWrap());
	
	// 계산된 데이터는 이미 스케일링 되어있기 때문에 스케일 역으로 감소 필요
	Vec2 matReverseScale = Vec2(1.f) / allottedGeometry.mRootSize;

	const std::vector<WidgetCharRenderCache> caches = _mTextCache.GetCharRenderCaches();
	if (caches.empty() == false)
	{
		drawElements.CreateTextElement(
			accLayerId,
			// 랜더링에 사용될 박스 사이즈는 원본 사이즈로 줄이고
			// 위치 조정도 스케일을 줄여 적용
			allottedGeometry.MakeChild(_mTextCache.GetBoxSize() / matReverseScale, Transform2D(_mTextCache.GetBoxOffset() * matReverseScale, 0.f, Vec2(1.f))),
			GetFontData(),
			caches,
			// 상속 색 * 배경 색
			contentInheritedColor.mInheritedColor * GetBackgroundColor().GetColor(contentInheritedColor),
			WidgetDrawOptionFlag::None
		);
	}
	return accLayerId;
}

void TextBlock::SetText(Attribute<std::wstring> text)
{
	if (text.IsSet() == true)
	{
		_mText.Set(this, text);
		_mTextCache.MarkLayoutDirty();
	}
}

void TextBlock::SetTransformPolicy(Attribute<TextTransformPolicy> policy)
{
	if (policy.IsSet() == true)
	{
		_mTransformPolicy.Set(this, policy);
		_mTextCache.MarkLayoutDirty();
	}
}

void TextBlock::SetJustifyPolicy(Attribute<TextJustifyPolicy> policy)
{
	if (policy.IsSet() == true)
	{
		_mJustifyPolicy.Set(this, policy);
		_mTextCache.MarkOnlyLineDirty();
	}
}

void TextBlock::SetFontData(Attribute<WidgetFontData> fontData)
{
	if (fontData.IsSet() == true)
	{
		_mFontData.Set(this, fontData);
		_mTextCache.MarkLayoutDirty();
	}
}

void TextBlock::SetTextMargin(Attribute<Margin> margin)
{
	if (margin.IsSet() == true)
	{
		_mTextMargin.Set(this, margin);
		_mTextCache.MarkLayoutDirty();
	}
}

void TextBlock::SetBackgroundColor(const Attribute<WidgetColor>& color)
{
	if (color.IsSet() == true)
	{
		_mBackgroundColor.Set(color);
	}
}

void TextBlock::SetMinDesireWidth(Attribute<float> width)
{
	if (width.IsSet() == true)
	{
		_mMinDesiredWidth.Set(this, width);
	}
}

void TextBlock::SetAutoWrap(Attribute<bool> isAutoWrapping)
{
	if (isAutoWrapping.IsSet() == true)
	{
		_mAutoWrap.Set(this, isAutoWrapping);
		_mTextCache.MarkOnlyLineDirty();
	}
}
