#pragma once

#include "Graphics/Widget/CompoundWidget.h"
#include "Graphics/Widget/Type/WidgetBrush.h"

class Border : public CompoundWidget
{
	GEN_REFLECTION(Border)

public:
	Border();

	/* 생성 시, 초기화 인자 구조체 정의 */
public:
	DECALRE_WIDGET_ARGS_BEGIN(Border)

		DECLARE_WIDGET_ARG_SLOT(Slot, Slots)

		DECLARE_INIT_WIDGET_ARG_ATTRIBUTE(Vec2, DesiredSizeScale, 1.f)
		DECLARE_INIT_WIDGET_ARG_ATTRIBUTE(Vec2, ContentSize, 1.f)

		DECLARE_INIT_WIDGET_ARG_ATTRIBUTE(Color, ChildInheritedColor, Color::White)
		DECLARE_INIT_WIDGET_ARG_ATTRIBUTE(WidgetColor, ChildForegroundColor, WidgetColor::SpecificInst())
		DECLARE_WIDGET_ARG_ATTRIBUTE(WidgetBrush, Brush)
		DECLARE_INIT_WIDGET_ARG_ATTRIBUTE(WidgetColor, BackgroundColor, WidgetColor::SpecificInst())

		DECLARE_WIDGET_ARG_EVENT(OnCallPointEvent, OnPressMouse)
		DECLARE_WIDGET_ARG_EVENT(OnCallPointEvent, OnReleaseMouse)
		DECLARE_WIDGET_ARG_EVENT(OnCallPointEvent, OnMoveMouse)
		DECLARE_WIDGET_ARG_EVENT(OnCallPointEvent, OnDoubleClickMouse)

	DECALRE_WIDGET_ARGS_END()

public:
	void OnConstruct(const Arguments& args);

private:
	virtual Vec2 ComputeDesireSize(const Vec2& layoutMultiplyValue) override;
	virtual uint32 OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor) override;

public:
	std::shared_ptr<Widget>& GetContent()
	{
		return _mSlotContainer.GetChildRef();
	}
	const std::shared_ptr<const Widget>& GetContent() const
	{
		return _mSlotContainer.GetChildRef();
	}

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
	virtual void SetContent(std::shared_ptr<Widget> content);
	void SetBrush(const Attribute<WidgetBrush>& brush);
	void SetBackgroundColor(const Attribute<WidgetColor>& color);
	void SetDesiredScale(Attribute<Vec2> scale);

private:
	// 배경 브러시
	PROPERTY(_mBrush)
	Attribute<WidgetBrush> _mBrush;
	// 배경 색상
	PROPERTY(_mBackgroundColor)
	Attribute<WidgetColor> _mBackgroundColor;
	// 레이아웃 계산 시 스케일
	PROPERTY(_mDesiredScale)
	LayoutAttribute<Vec2> _mDesiredScale;
};

