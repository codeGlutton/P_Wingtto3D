#pragma once

#include "Graphics/Widget/Widget.h"
#include "Graphics/Widget/Type/WidgetBrush.h"

class Image : public Widget
{
	GEN_REFLECTION(Image)

public:
	Image();

	/* 생성 시, 초기화 인자 구조체 정의 */
public:
	DECALRE_WIDGET_ARGS_BEGIN(Image)

		DECLARE_WIDGET_ARG_ATTRIBUTE(WidgetBrush, Brush)
		DECLARE_WIDGET_ARG_ATTRIBUTE(WidgetColor, BackgroundColor)
		DECLARE_INIT_WIDGET_ARG_ATTRIBUTE(Vec2, DesiredSizeScale, 1.f)
		DECLARE_WIDGET_ARG_EVENT(OnCallPointEvent, OnPressMouse)

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
	const WidgetBrush& GetBrush() const
	{
		return _mBrush.GetValue();
	}
	const WidgetColor& GetBackgroundColor() const
	{
		return _mBackgroundColor.GetValue();
	}
	const Color& GetBackgroundColor(const WidgetInheritedColor& inheritedColor) const
	{
		return _mBackgroundColor.GetValue().GetColor(inheritedColor);
	}
	const Vec2& GetDesiredScale() const
	{
		return _mDesiredScale.GetValue(this);
	}

public:
	void SetBrush(const Attribute<WidgetBrush>& brush);
	void SetBackgroundColor(const Attribute<WidgetColor>& color);
	void SetDesiredScale(Attribute<Vec2> scale);

private:
	// 이미지 브러시
	PROPERTY(_mBrush)
	Attribute<WidgetBrush> _mBrush;
	// 이미지 혼합 색상
	PROPERTY(_mBackgroundColor)
	Attribute<WidgetColor> _mBackgroundColor;
	// 레이아웃 계산 시 스케일
	PROPERTY(_mDesiredScale)
	LayoutAttribute<Vec2> _mDesiredScale;
};

