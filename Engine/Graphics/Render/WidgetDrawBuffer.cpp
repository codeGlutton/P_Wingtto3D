#include "pch.h"
#include "WidgetDrawBuffer.h"

#include "Core/App/AppWindow/AppWindow.h"
#include "Graphics/Resource/DXTexture.h"

#include "Manager/ResourceManager.h"

void WindowRenderElementContainer::CreateBoxElement(uint32 layerId, const WidgetGeometry& paintGeometry, const WidgetBrush& brush, const Color& color, WidgetDrawOptionFlag::Type drawOpt)
{
    if (IsCulling(paintGeometry) == true || IsCulling(brush, color) == true)
    {
        return;
    }

    std::unique_ptr<WidgetRenderBoxElement> box = std::make_unique<WidgetRenderBoxElement>();
    box->mResource = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXTexture2D>(L"White", DXSharedResourceType::Texture);

    switch (brush.mType)
    {
    case WidgetBrushType::Image:
    {
        box->mResource = brush.mProxy->mData;
        [[fallthrough]];
    }
    case WidgetBrushType::Box:
    {
        box->mTint = color;

        box->mMargin = brush.mMargin;
        box->mTiling = brush.mTiling;
        box->mBrushType = brush.mType;

        box->mRenderMat = paintGeometry.GetAccRenderMatrix2D();
        box->mBoxSize = paintGeometry.mBoxSize;
        box->mLayerId = layerId;
        box->mDrawOpt = drawOpt;
        
        while (mElementLayers.size() < layerId + 1)
        {
            mElementLayers.push_back(ElementList());
        }
        mElementLayers[layerId].push_back(std::move(box));
        break;
    }
    }
    ++mElementCount;
}

void WindowRenderElementContainer::ResetElements()
{
    mElementLayers.clear();
    mBatches.clear();
    mVertices.clear();
    mIndices.clear();
    mElementCount = 0u;
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
    if (brush.mType == WidgetBrushType::None || brush.mType == WidgetBrushType::Image && brush.mProxy == nullptr)
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


