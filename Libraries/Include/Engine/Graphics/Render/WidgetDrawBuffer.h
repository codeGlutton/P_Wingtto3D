#pragma once

#include "Utils/Memory/ObjectPool.h"

#include "Graphics/Render/RenderInclude.h"
#include "Graphics/Widget/Type/WidgetGeometry.h"
#include "Graphics/Widget/Type/WidgetBrush.h"

class AppWindow;

namespace RenderElementType
{
	enum Type
	{
		Box,
		Line,
		Text,
		Count
	};
}

struct RenderElement
{
	RenderElementType::Type mType;

	Matrix2D mRenderMatrix;
	Vec2 mBoxSize;

	uint8 mSceneId;
	uint32 mLayerId;

	Matrix mRenderMat;
	WidgetDrawOptionFlag::Type mDrawOpt;
};

struct RenderTintElement
{
	Color mTint;
};

struct RenderBoxElement : public RenderElement, public RenderTintElement
{
public:
	RenderBoxElement()
	{
		mType = RenderElementType::Box;
	}

public:
	Margin mMargin;
	//const FSlateShaderResourceProxy* ResourceProxy;
	TilingType mTiling;
	WidgetBrushType mBrushType;
};

struct WindowRenderElementContainer
{
	using ElementList = std::vector<std::unique_ptr<RenderElement>>;

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
	std::array<ElementList, RenderElementType::Count> mElementLists;
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

