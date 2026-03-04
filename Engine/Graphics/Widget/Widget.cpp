#include "pch.h"
#include "Widget.h"

#include "Input/InputState.h"

#include "Core/App/AppWindow/AppWindow.h"

#include "Graphics/Widget/Type/PaintArgs.h"
#include "Graphics/Widget/Type/WidgetBrush.h"
#include "Graphics/Widget/Type/ArrangedWidget.h"
#include "Graphics/Render/WidgetDrawBuffer.h"

ReplyData ReplyData::Handled()
{
    return ReplyData(ReplyType::Handled);
}

ReplyData ReplyData::Unhandled()
{
    return ReplyData(ReplyType::Unhandled);
}

void Widget::OnConstruct(const WidgetArgumentsBase& args)
{
    _mClipping = args.mClipping;
    _mRenderOpacity = args.mRenderOpacity;

    SetVisibility(args.mVisibility);
    SetRenderTransform(args.mRenderTransform);
    SetRenderTransformPivot(args.mRenderTransformPivot);
}

void Widget::Prepass(const Vec2& layoutMultiplyValue)
{
    // 변경점이 있는가?
    if (_mNeedToPrepass == true)
    {
        Prepass_Internal(layoutMultiplyValue);
    }
}

uint32 Widget::Paint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& parentCulling, const WidgetInheritedColor& inheritedColor)
{
    // 컬링 (수정 필요)
    BoundingAABB2D cullingBox = CalculateClipping(allottedGeometry, parentCulling);

    // 부모 스타일 상속
    WidgetInheritedColor contentInheritedColor = WidgetInheritedColor(inheritedColor).BlendOpacity(_mRenderOpacity);

    // 스크린 좌표 설정

    _mRenderCache.mRenderGeometry = allottedGeometry;
    _mRenderCache.mScreenGeometry = allottedGeometry;
    _mRenderCache.mScreenGeometry.AppendTransform(args.GetRootToScreenTransform2D());
    _mRenderCache.mRootWindow = drawElements.mWindow;

    // 업데이트

    if (_mIsUpdatable == true)
    {
        Update(_mRenderCache.mScreenGeometry, args.GetDeltaTime());
    }

    // 충돌 결과 저장

    const bool isHittable = args.GetInheritedHittable() && (GetVisibility() & VisibilityType::HittableFlag);
    const bool isOutgoingHittable = (GetVisibility() & VisibilityType::EffectChildrenHittablelag) ? isHittable : args.GetInheritedHittable();

    PaintArgs childArgs = args.WithNewParent(std::static_pointer_cast<Widget>(shared_from_this()));
    childArgs.SetInheritedHittable(isOutgoingHittable);

    if (isHittable == true)
    {
        args.GetHitTestGrid().AddWidget(std::static_pointer_cast<Widget>(shared_from_this()), accLayerId);
    }

    // 페인트

    uint32 childAccLayerId = OnPaint(drawElements, accLayerId, childArgs, allottedGeometry, cullingBox, contentInheritedColor);
    return childAccLayerId;
}

void Widget::MarkLayoutDirty(bool isWindowDirty) const
{
    _mNeedToPrepass = true;
    if (isWindowDirty == true)
    {
        std::shared_ptr<AppWindow> window = _mRenderCache.mRootWindow.lock();
        if (window != nullptr)
        {
            window->_mNeedToPrepass = true;
        }
    }
}

void Widget::SetVisibility(const Attribute<VisibilityType::Flag>& visibility)
{
    if (visibility.IsSet() == true)
    {
        _mVisibility.Set(this, visibility);
    }
}

void Widget::SetRenderTransformPivot(const Attribute<Vec2>& renderTransform)
{
    if (renderTransform.IsSet() == true)
    {
        _mRenderTransformPivot.Set(this, renderTransform);
    }
}

void Widget::SetRenderTransform(const Attribute<Transform2D>& renderTransform)
{
    if (renderTransform.IsSet() == true)
    {
        _mRenderTransform.Set(this, renderTransform);
    }
}

ReplyData Widget::OnPressKey(const WidgetGeometry& geometry, const std::shared_ptr<const KeyEvent>& event)
{
    if (mOnPressKey.IsBound() == true)
    {
        return mOnPressKey.Execute(&geometry, &event);
    }
    return ReplyData::Unhandled();
}

ReplyData Widget::OnChangeAnalogValue(const WidgetGeometry& geometry, const std::shared_ptr<const AnalogInputEvent>& event)
{
    if (mOnChangeAnalogValue.IsBound() == true)
    {
        return mOnChangeAnalogValue.Execute(&geometry, &event);
    }
    return ReplyData::Unhandled();
}

ReplyData Widget::OnPressMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
{
    if (mOnPressMouse.IsBound() == true)
    {
        return mOnPressMouse.Execute(&geometry, &event);
    }
    return ReplyData::Unhandled();
}

ReplyData Widget::OnReleaseMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
{
    if (mOnReleaseMouse.IsBound() == true)
    {
        return mOnReleaseMouse.Execute(&geometry, &event);
    }
    return ReplyData::Unhandled();
}

ReplyData Widget::OnDoubleClickMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
{
    if (mOnDoubleClickMouse.IsBound() == true)
    {
        return mOnDoubleClickMouse.Execute(&geometry, &event);
    }
    return ReplyData::Unhandled();
}

ReplyData Widget::OnMoveMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
{
    if (mOnMoveMouse.IsBound() == true)
    {
        return mOnMoveMouse.Execute(&geometry, &event);
    }
    return ReplyData::Unhandled();
}

void Widget::OnEnterMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
{
    if (mOnEnterMouse.IsBound() == true)
    {
        mOnEnterMouse.Execute(&geometry, &event);
    }
}

void Widget::OnLeaveMouse(const std::shared_ptr<const PointEvent>& event)
{
    if (mOnLeaveMouse.IsBound() == true)
    {
        mOnLeaveMouse.Execute(&event);
    }
}

bool Widget::IsChildWidgetCulling(const BoundingAABB2D& cullingRect, const ArrangedWidget& child) const
{
    if (cullingRect.Intersects(child.mGeometry.GetRenderBoundingBox()) == true)
    {
        return false;
    }
    if (cullingRect.Intersects(child.mGeometry.GetLayoutBoundingBox()) == true)
    {
        return false;
    }

    return true;
}

void Widget::Prepass_Internal(const Vec2& layoutMultiplyValue)
{
    // 자식 위젯에 대해서 재귀
    if (_mCanHaveChild == true)
    {
        Prepass_Children(layoutMultiplyValue);
    }
    // 자식 위젯의 DesiredSize를 통해 부모의 DesiredSize 연산
    _mDesiredSize = ComputeDesireSize(layoutMultiplyValue);
    _mNeedToPrepass = false;
}

void Widget::Prepass_Children(const Vec2& layoutMultiplyValue)
{
    WidgetSlotContainer& container = GetChildren();
    const std::size_t containerSize = container.Size();
    for (std::size_t i = 0; i < containerSize; ++i)
    {
        std::shared_ptr<Widget>& child = container.GetChildRef(i);
        Vec2 childLayoutMultiplyValue = layoutMultiplyValue;
        if (_mHasRelativeChildScale == true)
        {
            // 자식에 대한 상대적 스케일 값이 존재할 때, 가져와 넘겨주기
            childLayoutMultiplyValue *= GetChildRelativeScale(static_cast<int32>(i), layoutMultiplyValue);
        }

        if (child->GetVisibility() & VisibilityType::TakesSpaceFlag)
        {
            // 부모가 Layout이 수정될 시에, 자식도 조건없이 재계산해주어야 한다.
            // 
            // 단 레이아웃의 연산은 부모로부터의 누적 스케일을 전달하되, 
            // 자식부터 결정되어 부모에 영향을 준다.
            child->Prepass_Internal(childLayoutMultiplyValue);
        }
    }
}

BoundingAABB2D Widget::CalculateClipping(const WidgetGeometry& allottedGeometry, const BoundingAABB2D& cullingRect) const
{
    switch (_mClipping)
    {
    case ClippingType::Bound:
        return allottedGeometry.GetRenderBoundingBox(_mCullingExtension);
    }
    return cullingRect;
}

void NullWidget::OnConstruct(const Arguments& args)
{
    Super::OnConstruct(args);
    _mVisibility.Set(this, VisibilityType::SelfHitTestInvisible);
}