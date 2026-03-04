#include "pch.h"
#include "Border.h"

#include "Utils/WidgetUtils.h"

#include "Graphics/Render/WidgetDrawBuffer.h"

void Border::OnConstruct(const Arguments& args)
{
	Widget::OnConstruct(args);
	if (args.mSlots.empty() == false)
	{
		_mSlotContainer.AddChild(std::move(const_cast<Slot::Arguments&>(args.mSlots[0])));
	}

	SetContentSize(args.mContentSize);
	SetDesiredScale(args.mDesiredSizeScale);

	SetChildInheritedColor(args.mChildInheritedColor);
	SetChildForegroundColor(args.mChildForegroundColor);
	SetBrush(args.mBrush);
	SetBackgroundColor(args.mBackgroundColor);

	if (args.mOnPressMouse.IsBound() == true)
	{
		mOnPressMouse = args.mOnPressMouse;
	}
	if (args.mOnReleaseMouse.IsBound() == true)
	{
		mOnReleaseMouse = args.mOnReleaseMouse;
	}
	if (args.mOnMoveMouse.IsBound() == true)
	{
		mOnMoveMouse = args.mOnMoveMouse;
	}
	if (args.mOnDoubleClickMouse.IsBound() == true)
	{
		mOnDoubleClickMouse = args.mOnMoveMouse;
	}
}

Border::Border()
{
	_mVisibility.Init(VisibilityType::Visible);
}

Vec2 Border::ComputeDesireSize(const Vec2& layoutMultiplyValue)
{
	return _mDesiredScale.GetValue(this) * Super::ComputeDesireSize(layoutMultiplyValue);
}

uint32 Border::OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor)
{
	const WidgetBrush& brushResource = _mBrush.GetValue();
	switch (brushResource.mType)
	{
	case WidgetBrushType::Image:
		if (brushResource.mTexture == nullptr)
		{
			break;
		}
		[[fallthrough]];
	case WidgetBrushType::Box:
		drawElements.CreateBoxElement(
			accLayerId,
			allottedGeometry,
			// 배경 브러쉬
			brushResource,
			// 배경 브러쉬 틴트 * 상속 색 * 배경 색
			brushResource.mTint.GetColor(contentInheritedColor) * contentInheritedColor.mInheritedColor * _mBackgroundColor.GetValue().GetColor(contentInheritedColor),
			WidgetDrawOptionFlag::None
		);
		break;
	}
	return Super::OnPaint(drawElements, accLayerId, args, allottedGeometry, currentCulling, contentInheritedColor);
}
void Border::SetContent(std::shared_ptr<Widget> content)
{
	_mSlotContainer.GetSlotRef()
		[
			content
		];
}

void Border::SetBrush(const Attribute<WidgetBrush>& brush)
{
	if (brush.IsSet() == true)
	{
		_mBrush.Set(brush);
	}
}

void Border::SetBackgroundColor(const Attribute<WidgetColor>& color)
{
	if (color.IsSet() == true)
	{
		_mBackgroundColor.Set(color);
	}
}

void Border::SetDesiredScale(Attribute<Vec2> scale)
{
	if (scale.IsSet() == true)
	{
		_mDesiredScale.Set(this, scale);
	}
}
