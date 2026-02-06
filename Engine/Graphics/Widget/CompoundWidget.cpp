#include "pch.h"
#include "CompoundWidget.h"

#include "Graphics/Widget/Type/PaintArgs.h"
#include "Graphics/Widget/Type/WidgetBrush.h"
#include "Graphics/Widget/Type/ArrangedWidget.h"

WidgetSlotContainer& CompoundWidget::GetChildren()
{
	return _mSlotContainer;
}

const WidgetSlotContainer& CompoundWidget::GetChildren() const
{
	return _mSlotContainer;
}

Vec2 CompoundWidget::ComputeDesireSize(const Vec2& layoutMultiplyValue)
{
	if (_mSlotContainer.Size() != 0)
	{
		VisibilityType::Flag childVisibility = _mSlotContainer.GetChild()->GetVisibility();
		if (childVisibility & VisibilityType::TakesSpaceFlag)
		{
			Vec2 childDesiredSize = _mSlotContainer.GetChildRef()->GetDesiredSize() + _mSlotContainer.GetSlotRef().GetPadding().GetDesiredSize();
			return childDesiredSize;
		}
	}
	return Vec2::Zero;
}

uint32 CompoundWidget::OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor)
{
	// 시야에 보이는 자식을 한정해서 Geometry정보 수집
    ArrangedChildren arrangedChildren(VisibilityType::VisibleFlag);
    OnArrangeChildren(allottedGeometry, arrangedChildren);

	if (arrangedChildren.Size() > 0)
	{
		// 자식 요소 영향 줄 부분 누적
		WidgetInheritedColor childWidgetInheritedColor = WidgetInheritedColor(contentInheritedColor)
			.BlendColor(_mChildInheritedColor)
			.SetForegroundColor(_mChildForegroundColor);

		// 자식 순회
		return arrangedChildren.mArrangedWidgets[0].mWidget->Paint(
			drawElements, 
			accLayerId + 1, 
			args.WithNewParent(std::static_pointer_cast<Widget>(shared_from_this())),
			arrangedChildren.mArrangedWidgets[0].mGeometry,
			currentCulling, childWidgetInheritedColor
		);
	}
	return accLayerId;
}

void CompoundWidget::OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const
{
	if (_mSlotContainer.Size() != 0)
	{
		VisibilityType::Flag childVisibility = _mSlotContainer.GetChildRef()->GetVisibility();
		if (children.CanAccept(childVisibility) == true)
		{
			CompoundWidget* mutableThis = const_cast<CompoundWidget*>(this);

			// 로컬 크기 및 로컬 오프셋 값 계산

			const Margin& childSlotPadding = _mSlotContainer.GetSlotRef().GetPadding();
			const ChildAlignmentResult xResult = AlignChildAxisX(allottedGeometry.mLocalSize.x, _mSlotContainer.GetSlotRef(), childSlotPadding);
			const ChildAlignmentResult yResult = AlignChildAxisY(allottedGeometry.mLocalSize.y, _mSlotContainer.GetSlotRef(), childSlotPadding);

			// 로컬 크기 및 로컬 오프셋 값은 계산한 값으로 대입 + 내부에서 랜더 트랜스폼은 알아서 적용
			// 부모로부터 전달받은 트랜스폼 누적 값도 알아서 적용
			children.AddWidget(
				ArrangedWidget(
					mutableThis->_mSlotContainer.GetChildRef(),
					allottedGeometry.MakeChild(
						_mSlotContainer.GetChild(),
						Vec2(xResult.mSize, yResult.mSize),
						Transform2D(Vec2(xResult.mOffset, yResult.mOffset), 0.f, Vec2(1.f))
					)
				),
				childVisibility
			);
		}
	}
}

void CompoundWidget::Serialize(Archive& archive) const
{
	Super::Serialize(archive);
	_mSlotContainer.Serialize(archive);
}

void CompoundWidget::Deserialize(Archive& archive)
{
	Super::Deserialize(archive);
	_mSlotContainer.Deserialize(archive);
}
