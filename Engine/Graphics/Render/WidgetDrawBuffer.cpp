#include "pch.h"
#include "WidgetDrawBuffer.h"

#include "Core/App/AppWindow/AppWindow.h"

void WindowRenderElementContainer::CreateBoxElement(uint32 layerId, const WidgetGeometry& paintGeometry, const WidgetBrush& brush, const Color& color, WidgetDrawOptionFlag::Type drawOpt)
{
    if (IsCulling(paintGeometry) == true || IsCulling(brush, color) == true)
    {
        return;
    }

    std::unique_ptr<RenderBoxElement> box = std::make_unique<RenderBoxElement>();

    switch (brush.mType)
    {
    case WidgetBrushType::Box:
    case WidgetBrushType::Image:
    {
        box->mTint = color;
        box->mMargin = brush.mMargin;
        box->mTiling = brush.mTiling;
        box->mBrushType = brush.mType;
        //box->mProxy
        //box.Init(ElementList, ElementType, InLayer, PaintGeometry, InDrawEffects);
        
        mElementLists[RenderElementType::Box].push_back(std::move(box));
        break;
    }
    }
}

void WindowRenderElementContainer::ResetElements()
{
    for (auto& elements : mElementLists)
    {
        elements.clear();
    }
}

bool WindowRenderElementContainer::IsCulling(const WidgetGeometry& paintGeometry)
{
    // 사이즈가 0
    Vec2 resultSize = paintGeometry.mBoxSize * paintGeometry.mLocalSize;
    if (resultSize.x <= 0.f && resultSize.y <= 0.f)
    {
        return true;
    }
    // 화면에 안 비침
    if (paintGeometry.GetLayoutBoundingBox().Intersects(RECT(0, 0, static_cast<long>(mWindowSize.x), static_cast<long>(mWindowSize.y))) == false)
    {
        return true;
    }
    return false;
}

bool WindowRenderElementContainer::IsCulling(const WidgetBrush& brush, const Color& color)
{
    // 투명해서 컬링
    if (color.w == 0.f)
    {
        return true;
    }
    // 브러쉬가 비정상적
    if (brush.mType == WidgetBrushType::None || brush.mType == WidgetBrushType::Image && brush.mResource == nullptr)
    {
        return true;
    }

    return false;
}

WidgetDrawBuffer::WidgetDrawBuffer()
{
}

std::shared_ptr<WindowRenderElementContainer> WidgetDrawBuffer::GetContainer(std::shared_ptr<AppWindow> window) const
{
    std::shared_ptr<WindowRenderElementContainer> container = ObjectPool<WindowRenderElementContainer>::MakeShared();
    container->ResetElements();
    container->mWindow = window;
    container->mWindowSize = Vec2(window->GetDesc().mWidth, window->GetDesc().mHeight);

    return container;
}


