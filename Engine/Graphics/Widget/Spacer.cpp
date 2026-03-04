#include "pch.h"
#include "Spacer.h"

void Spacer::OnConstruct(const Arguments& args)
{
    Widget::OnConstruct(args);

    SetEmptySize(args.mEmptySize);
}

Spacer::Spacer()
{
    _mVisibility.Init(VisibilityType::Hidden);
}

WidgetSlotContainer& Spacer::GetChildren()
{
    return EmptySlotContainer::GetInst();
}

const WidgetSlotContainer& Spacer::GetChildren() const
{
    return EmptySlotContainer::GetInst();
}

void Spacer::OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const
{
}

Vec2 Spacer::ComputeDesireSize(const Vec2& layoutMultiplyValue)
{
    return _mEmptySize.GetValue(this);
}

uint32 Spacer::OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor)
{
    return accLayerId;
}
