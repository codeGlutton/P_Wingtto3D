#pragma once

#include "Graphics/Widget/Widget.h"
#include "Graphics/Widget/Slot/WidgetSlotOption.h"

/**
 * 자식을 하나 가질 수 있는 베이스 위젯 클래스
 */
class CompoundWidget abstract : public Widget
{
	GEN_ABSTRACT_REFLECTION(CompoundWidget)

public:
	WidgetSlotContainer& GetChildren() override;
	const WidgetSlotContainer& GetChildren() const override;

private:
	Vec2 ComputeDesireSize(const Vec2& layoutMultiplyValue) override;
	uint32 OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor) override;
	void OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const override;

public:
	const Color& GetChildInheritedColor() const
	{
		return _mChildInheritedColor;
	}
	const Color& GetChildForegroundColor() const
	{
		return _mChildForegroundColor;
	}

	void SetChildInheritedColor(const Color& color)
	{
		_mChildInheritedColor = color;
	}
	void SetChildForegroundColor(const Color& color)
	{
		_mChildForegroundColor = color;
	}

protected:
	virtual void Serialize(Archive& archive) const override;
	virtual void Deserialize(Archive& archive) override;

protected:
	struct Slot : public WidgetSlotBase, public PaddingSlotOption<Slot>, public AlignmentSlotOption<Slot>
	{
		GEN_STRUCT_REFLECTION(CompoundWidget::Slot)

		using WidgetSlotBase::WidgetSlotBase;

		PROPERTY(_mWidget)
		PROPERTY(_mWidgetConst)
		PROPERTY(_mPadding)
		PROPERTY(_mHorizonAlignment)
		PROPERTY(_mVerticalAlignment)
	};

protected:
	SingleSlotContainer<Slot> _mSlotContainer;

private:
	PROPERTY(_mChildInheritedColor)
	Color _mChildInheritedColor;
	PROPERTY(_mChildForegroundColor)
	Color _mChildForegroundColor;
};

