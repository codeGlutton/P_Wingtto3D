#pragma once

#include "Graphics/Widget/Widget.h"
#include "Graphics/Widget/Type/WidgetStyle.h"
#include "Graphics/Widget/Type/WidgetFontData.h"
#include "Graphics/Widget/Type/WidgetTextCache.h"

class TextBlockStyle : public WidgetStyle
{
	GEN_REFLECTION(TextBlockStyle)

public:
	PROPERTY(mFont)
	std::shared_ptr<Font> mFont;
	PROPERTY(mCharSpacing)
	int32 mCharSpacing;
	PROPERTY(mBackgroundColor)
	WidgetColor mBackgroundColor;
};

class TextBlock : public Widget
{
	GEN_REFLECTION(TextBlock)

public:
	TextBlock();

	/* 생성 시, 초기화 인자 구조체 정의 */
public:
	DECALRE_WIDGET_ARGS_BEGIN(TextBlock)

		// 객체 스타일
		// 초기 값으로 선택됨
		DECLARE_WIDGET_ARG_CONST(std::shared_ptr<const TextBlockStyle>, Style)

		DECLARE_WIDGET_ARG_ATTRIBUTE(std::wstring, Text)
		DECLARE_WIDGET_ARG_ATTRIBUTE(TextTransformPolicy, TransformPolicy)
		DECLARE_WIDGET_ARG_ATTRIBUTE(TextJustifyPolicy, JustifyPolicy)
		DECLARE_WIDGET_ARG_ATTRIBUTE(WidgetFontData, FontData)
		DECLARE_WIDGET_ARG_ATTRIBUTE(Margin, TextMargin)

		DECLARE_WIDGET_ARG_ATTRIBUTE(WidgetColor, BackgroundColor)
		
		DECLARE_WIDGET_ARG_ATTRIBUTE(float, MinDesiredWidth)
		DECLARE_WIDGET_ARG_ATTRIBUTE(bool, AutoWrap)

		DECLARE_WIDGET_ARG_EVENT(OnCallPointEvent, OnDoubleClickMouse)

	DECALRE_WIDGET_ARGS_END()

public:
	void OnConstruct(const Arguments& args);

public:
	virtual WidgetSlotContainer& GetChildren() override;
	virtual const WidgetSlotContainer& GetChildren() const override;

private:
	virtual void OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const override;
	virtual Vec2 ComputeDesireSize(const Vec2& layoutMultiplyValue) override;
	virtual uint32 OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor) override;

public:
	const std::wstring& GetText() const
	{
		return _mText.GetValue(this);
	}
	TextTransformPolicy GetTransformPolicy() const
	{
		return _mTransformPolicy.GetValue(this);
	}
	TextJustifyPolicy GetJustifyPolicy() const
	{
		return _mJustifyPolicy.GetValue(this);
	}
	const WidgetFontData& GetFontData() const
	{
		return _mFontData.GetValue(this);
	}
	const Margin& GetMargin() const
	{
		return _mTextMargin.GetValue(this);
	}

	const WidgetColor& GetBackgroundColor() const
	{
		return _mBackgroundColor.GetValue();
	}

	float GetMinDesireWidth() const
	{
		return _mMinDesiredWidth.GetValue(this);
	}
	bool GetAutoWrap() const
	{
		return _mAutoWrap.GetValue(this);
	}

public:
	void SetText(Attribute<std::wstring> text);
	void SetTransformPolicy(Attribute<TextTransformPolicy> policy);
	void SetJustifyPolicy(Attribute<TextJustifyPolicy> policy);
	void SetFontData(Attribute<WidgetFontData> fontData);
	void SetTextMargin(Attribute<Margin> margin);

	void SetBackgroundColor(const Attribute<WidgetColor>& color);

	void SetMinDesireWidth(Attribute<float> width);
	void SetAutoWrap(Attribute<bool> isAutoWrapping);

private:
	PROPERTY(_mText)
	LayoutAttribute<std::wstring> _mText;
	PROPERTY(_mTransformPolicy)
	LayoutAttribute<TextTransformPolicy> _mTransformPolicy;
	PROPERTY(_mJustifyPolicy)
	LayoutAttribute<TextJustifyPolicy> _mJustifyPolicy;
	PROPERTY(_mFontData)
	LayoutAttribute<WidgetFontData> _mFontData;
	PROPERTY(_mTextMargin)
	LayoutAttribute<Margin> _mTextMargin;

	// 이미지 혼합 색상
	PROPERTY(_mBackgroundColor)
	Attribute<WidgetColor> _mBackgroundColor;

	// 레이아웃 계산 시 최소치 너비
	PROPERTY(_mMinDesiredWidth)
	LayoutAttribute<float> _mMinDesiredWidth;
	// 줄 자동 바꾸기 여부
	PROPERTY(_mAutoWrap)
	LayoutAttribute<bool> _mAutoWrap;

	PROPERTY(_mStyle)
	std::weak_ptr<const TextBlockStyle> _mStyle;

private:
	mutable WidgetTextCache _mTextCache;
};

