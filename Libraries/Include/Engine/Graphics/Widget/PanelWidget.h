#pragma once

#include "Graphics/Widget/Widget.h"

/**
 * 자식을 여럿 가질 수 있는 베이스 위젯 클래스
 */
class PanelWidget abstract : public Widget
{
	GEN_ABSTRACT_REFLECTION(PanelWidget)

public:
	PanelWidget();

private:
	uint32 OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor) override;
	uint32 PaintArrangedChildren(OUT WindowRenderElementContainer& drawElements, const ArrangedChildren& children, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor);
};

