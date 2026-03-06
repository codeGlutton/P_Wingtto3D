#include "pch.h"
#include "WidgetDrawBuffer.h"

#include "Manager/PathManager.h"
#include "Manager/ResourceManager.h"

#include "Core/App/AppWindow/AppWindow.h"

#include "Core/Resource/Texture.h"
#include "Graphics/Resource/DXTexture.h"

void WindowRenderElementContainer::CreateBoxElement(uint32 layerId, const WidgetGeometry& paintGeometry, const WidgetBrush& brush, const Color& color, WidgetDrawOptionFlag::Type drawOpt)
{
    if (IsCulling(paintGeometry) == true || IsCulling(brush, color) == true)
    {
        return;
    }

    std::unique_ptr<WidgetRenderBoxElement> box = std::make_unique<WidgetRenderBoxElement>();

    std::shared_ptr<Texture2D> defaultBoxTexture = RESOURCE_MANAGER->LoadOrGetResource<Texture2D>(
        PATH_MANAGER->GetEngineResourceFolderName() + L"\\Texture\\T_White"
    );
    box->mResource = defaultBoxTexture->GetProxy()->mData;

    switch (brush.mType)
    {
    case WidgetBrushType::Image:
    {
        box->mResource = brush.mTexture->GetProxy()->mData;
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

void WindowRenderElementContainer::CreateTextElement(uint32 layerId, const WidgetGeometry& paintGeometry, const WidgetFontData& fontData, const std::vector<WidgetCharRenderCache>& renderCaches, const Color& color, WidgetDrawOptionFlag::Type drawOpt)
{
    if (IsCulling(paintGeometry) == true || IsCulling(fontData, color, renderCaches) == true)
    {
        return;
    }

    std::unique_ptr<WidgetRenderTextElement> text = std::make_unique<WidgetRenderTextElement>();
    text->mResource = fontData.GetFont()->GetProxy(fontData.GetPixelSize())->mData;
    text->mAtlasData = fontData.GetFontAtlasCahce();
    text->mTint = color;
    text->mRenderCaches = renderCaches;

    text->mRenderMat = paintGeometry.GetAccRenderMatrix2D();
    text->mBoxSize = paintGeometry.mBoxSize;
    text->mLayerId = layerId;
    text->mDrawOpt = drawOpt;

    while (mElementLayers.size() < layerId + 1)
    {
        mElementLayers.push_back(ElementList());
    }
    mElementLayers[layerId].push_back(std::move(text));

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
    Vec2 resultSize = paintGeometry.mBoxSize * paintGeometry.mRootSize;
    if (resultSize.x <= 0.f && resultSize.y <= 0.f)
    {
        return true;
    }
    // 화면에 안 비침
    if (paintGeometry.GetLayoutBoundingBox().Intersects(RECT(0, 0, static_cast<long>(mWindowClientSize.x), static_cast<long>(mWindowClientSize.y))) == false)
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
    if (brush.mType == WidgetBrushType::None || brush.mType == WidgetBrushType::Image && brush.mTexture == nullptr)
    {
        return true;
    }

    return false;
}

bool WindowRenderElementContainer::IsCulling(const WidgetFontData& fontData, const Color& color, const std::vector<WidgetCharRenderCache>& renderCaches)
{
    // 투명해서 컬링
    if (color.w == 0.f)
    {
        return true;
    }
    // 폰트 혹은 텍스트가 비정상적
    if (fontData.GetPixelSize() == 0 || renderCaches.empty() == true || fontData.GetFont() == nullptr)
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
    container->mWindowClientSize = Vec2(window->GetDesc().mClientWidth, window->GetDesc().mClientHeight);

    return container;
}

