#include "pch.h"
#include "WidgetDrawBuffer.h"

#include "Core/App/AppWindow/AppWindow.h"

void WindowRenderElementContainer::CreateBoxElement(uint32 layerId, const WidgetGeometry& paintGeometry, const WidgetBrush& brush, const Color& color, WidgetDrawOptionFlag::Type drawOpt)
{
}

void WindowRenderElementContainer::ResetElements()
{
    for (auto& elements : mElementLists)
    {
        elements.clear();
    }
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


