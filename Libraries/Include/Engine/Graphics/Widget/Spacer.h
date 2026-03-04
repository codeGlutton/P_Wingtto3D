#pragma once

#include "Graphics/Widget/Widget.h"

class Spacer : public Widget
{
	GEN_REFLECTION(Spacer)

public:
	Spacer();

	/* 생성 시, 초기화 인자 구조체 정의 */
public:
	DECALRE_WIDGET_ARGS_BEGIN(Spacer)

		DECLARE_INIT_WIDGET_ARG_CONST(Vec2, EmptySize, Vec2(0.f))

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
	Vec2 GetEmptySize() const
	{
		_mEmptySize.GetValue(this);
	}

public:
	void SetEmptySize(const Attribute<Vec2>& size)
	{
		_mEmptySize.Set(this, size);
	}

private:
	LayoutAttribute<Vec2> _mEmptySize;
};

