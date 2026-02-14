#pragma once

#include "Utils/Memory/ObjectPool.h"

#include "Graphics/Render/RenderInclude.h"
#include "Graphics/Widget/Type/WidgetGeometry.h"
#include "Graphics/Widget/Type/WidgetBrush.h"

#include "Graphics/DXDefaultVertexData.h"

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

struct WidgetRenderBatch
{
	WidgetRenderBatch(const std::shared_ptr<DXTextureBase>& texture, uint32 startIndex) :
		mTexture(texture),
		mStartIndex(startIndex),
		mIndexCount(0u)
	{
	}

	const std::shared_ptr<DXTextureBase>& mTexture;
	uint32 mStartIndex;
	uint32 mIndexCount;
};

struct WindowRenderElementContainer
{
	using ElementList = std::vector<std::unique_ptr<WidgetRenderElement>>;

public:
	void CreateBoxElement(uint32 layerId, const WidgetGeometry& paintGeometry, const WidgetBrush& brush, const Color& color, WidgetDrawOptionFlag::Type drawOpt);
	//void CreateLineElement();
	//void CreateTextElement();

public:
	void ResetElements();

private:
	bool IsCulling(const WidgetGeometry& paintGeometry);
	bool IsCulling(const WidgetBrush& brush, const Color& color);

public:
	std::weak_ptr<AppWindow> mWindow;
	Vec2 mWindowSize;

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

