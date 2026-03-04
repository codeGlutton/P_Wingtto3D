#include "pch.h"
#include "Image.h"

#include "Core/Resource/Texture.h"
#include "Graphics/Render/WidgetDrawBuffer.h"

void Image::OnConstruct(const Arguments& args)
{
    Widget::OnConstruct(args);

	SetDesiredScale(args.mDesiredSizeScale);
	SetBrush(args.mBrush);
	SetBackgroundColor(args.mBackgroundColor);

	if (args.mOnPressMouse.IsBound() == true)
	{
		mOnPressMouse = args.mOnPressMouse;
	}
}

Image::Image()
{
	_mVisibility.Init(VisibilityType::Visible);
}

WidgetSlotContainer& Image::GetChildren()
{
    return EmptySlotContainer::GetInst();
}

const WidgetSlotContainer& Image::GetChildren() const
{
    return EmptySlotContainer::GetInst();
}

void Image::OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const
{
}

Vec2 Image::ComputeDesireSize(const Vec2& layoutMultiplyValue)
{
	const WidgetBrush& brush = _mBrush.GetValue();
	if (brush.mTexture != nullptr)
	{
		return (_mDesiredScale.IsSet() == true) ? _mDesiredScale.GetValue(this) : brush.mImageSize;
	}
	return Vec2::Zero;
}

uint32 Image::OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor)
{
	const WidgetBrush& brush = _mBrush.GetValue();
	switch (brush.mType)
	{
	case WidgetBrushType::Image:
		if (brush.mTexture == nullptr)
		{
			break;
		}
		[[fallthrough]];
	case WidgetBrushType::Box:
		drawElements.CreateBoxElement(
			accLayerId,
			allottedGeometry,
			// 배경 브러쉬
			brush,
			// 배경 브러쉬 틴트 * 상속 색 * 배경 색
			brush.mTint.GetColor(contentInheritedColor) * contentInheritedColor.mInheritedColor * _mBackgroundColor.GetValue().GetColor(contentInheritedColor),
			WidgetDrawOptionFlag::None
		);
		break;
	}
	return accLayerId;
}

void Image::SetBrush(const Attribute<WidgetBrush>& brush)
{
	if (brush.IsSet() == true)
	{
		_mBrush.Set(brush);
	}
}

void Image::SetBackgroundColor(const Attribute<WidgetColor>& color)
{
	if (color.IsSet() == true)
	{
		_mBackgroundColor.Set(color);
	}
}

void Image::SetDesiredScale(Attribute<Vec2> scale)
{
	if (scale.IsSet() == true)
	{
		_mDesiredScale.Set(this, scale);
	}
}
