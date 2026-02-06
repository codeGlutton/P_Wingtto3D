#include "pch.h"
#include "PanelWidget.h"

#include "Graphics/Widget/Type/PaintArgs.h"
#include "Graphics/Widget/Type/WidgetBrush.h"
#include "Graphics/Widget/Type/ArrangedWidget.h"

PanelWidget::PanelWidget()
{
	_mCanHaveChild = true;
	_mHasRelativeChildScale = true;
}

uint32 PanelWidget::OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor)
{
    // 시야에 보이는 자식들을 한정해서 Geometry정보 수집
    ArrangedChildren arrangedChildren(VisibilityType::VisibleFlag);
    OnArrangeChildren(allottedGeometry, arrangedChildren);

    return PaintArrangedChildren(drawElements, arrangedChildren, accLayerId, args, allottedGeometry, currentCulling, contentInheritedColor);
}

uint32 PanelWidget::PaintArrangedChildren(OUT WindowRenderElementContainer& drawElements, const ArrangedChildren& children, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor)
{
	if (children.Size() > 0)
	{
		// 자식 요소 영향 줄 부분 누적
		PaintArgs childArgs = args.WithNewParent(std::static_pointer_cast<Widget>(shared_from_this()));

		// 자식들을 향해 하향식 트리 순회
		uint32 maxChildAccLayerId = accLayerId;
		for (auto& arrangedWidget : children.mArrangedWidgets)
		{
			if (IsChildWidgetCulling(currentCulling, arrangedWidget) == false)
			{
				uint32 childAccLayerId = arrangedWidget.mWidget->Paint(drawElements, maxChildAccLayerId, args, arrangedWidget.mGeometry, currentCulling, contentInheritedColor);
				if (maxChildAccLayerId < childAccLayerId)
				{
					maxChildAccLayerId = childAccLayerId;
				}
			}
		}
		return maxChildAccLayerId;
	}
	return accLayerId;
}
