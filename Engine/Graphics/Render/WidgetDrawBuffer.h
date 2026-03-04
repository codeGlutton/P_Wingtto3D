#pragma once

#include "Utils/Memory/ObjectPool.h"

#include "Graphics/Render/RenderInclude.h"
#include "Graphics/Widget/Type/WidgetGeometry.h"
#include "Graphics/Widget/Type/WidgetBrush.h"
#include "Graphics/Widget/Type/WidgetFontData.h"
#include "Graphics/Widget/Type/WidgetTextCache.h"

#include "Graphics/DXDefaultVertexData.h"

#include "Core/Resource/Font.h"

class AppWindow;
class DXTextureBase;

namespace WidgetRenderElementType
{
	enum Type
	{
		Box,
		Line,
		Text,
		Count
	};
}

struct WidgetRenderElement
{
	WidgetRenderElementType::Type mType;

	Matrix mRenderMat = Matrix::Identity;
	Vec2 mBoxSize = Vec2();

	uint8 mSceneId = 0u;
	uint32 mLayerId = 0u;

	// TODO 아직 미적용
	WidgetDrawOptionFlag::Type mDrawOpt = WidgetDrawOptionFlag::None;
};

struct WidgetRenderTintElement
{
	Color mTint = Color::White;
};

struct WidgetRenderBoxElement : public WidgetRenderElement, public WidgetRenderTintElement
{
public:
	WidgetRenderBoxElement()
	{
		mType = WidgetRenderElementType::Box;
	}

public:
	Margin mMargin;
	std::shared_ptr<DXTextureBase> mResource;
	TilingType mTiling;
	WidgetBrushType mBrushType;
};

struct WidgetRenderTextElement : public WidgetRenderElement, public WidgetRenderTintElement
{
public:
	WidgetRenderTextElement()
	{
		mType = WidgetRenderElementType::Text;
	}

public:
	std::shared_ptr<DXFontTexture> mResource;
	FontAtlasDataRef mAtlasData;
	std::vector<WidgetCharRenderCache> mRenderCaches;
};

struct WidgetRenderBatch
{
	WidgetRenderBatch(const std::shared_ptr<DXTextureBase>& texture, bool isText, uint32 startIndex) :
		mTexture(texture),
		mIsText(isText),
		mStartIndex(startIndex),
		mIndexCount(0u)
	{
	}

	const std::shared_ptr<DXTextureBase>& mTexture;
	bool mIsText;
	uint32 mStartIndex;
	uint32 mIndexCount;
};

struct WindowRenderElementContainer
{
	using ElementList = std::vector<std::unique_ptr<WidgetRenderElement>>;

public:
	void CreateBoxElement(uint32 layerId, const WidgetGeometry& paintGeometry, const WidgetBrush& brush, const Color& color, WidgetDrawOptionFlag::Type drawOpt);
	//void CreateLineElement();
	void CreateTextElement(uint32 layerId, const WidgetGeometry& paintGeometry, const WidgetFontData& fontData, const std::vector<WidgetCharRenderCache>& renderCaches, const Color& color, WidgetDrawOptionFlag::Type drawOpt);

public:
	void ResetElements();

private:
	bool IsCulling(const WidgetGeometry& paintGeometry);
	bool IsCulling(const WidgetBrush& brush, const Color& color);
	bool IsCulling(const WidgetFontData& fontData, const Color& color, const std::vector<WidgetCharRenderCache>& renderCaches);

public:
	std::weak_ptr<AppWindow> mWindow;
	Vec2 mWindowClientSize;

public:
	std::vector<WidgetRenderBatch> mBatches;
	std::vector<UIVertexData> mVertices;
	std::vector<uint32> mIndices;

	std::vector<ElementList> mElementLayers;
	std::size_t mElementCount = 0;
};

/**
 * 위젯 랜더링 타겟들을 랜더 스레드에 전송해주기 위한 버퍼
 */
class WidgetDrawBuffer
{
	friend class RenderManager;

private:
	WidgetDrawBuffer();

public:
	std::shared_ptr<WindowRenderElementContainer> GetContainer(std::shared_ptr<AppWindow> window) const;
};

