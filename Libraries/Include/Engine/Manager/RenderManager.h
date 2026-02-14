#pragma once

#include "Graphics/Render/WidgetDrawBuffer.h"
#include "Graphics/Scene/Scene.h"

#define RENDER_MANAGER RenderManager::GetInst()
#define SCENE RENDER_MANAGER->GetScene()

class DXSwapChain;
class DXViewport;
class DXWidgetBatchMesh;

struct WindowDrawInputs
{
	std::shared_ptr<WindowRenderElementContainer> mContainer;
	std::shared_ptr<DXSwapChain> mSwapChain;
	std::shared_ptr<DXViewport> mViewport;
	double mGameThreadTime;
	float mGameThreadDeltaTime;
	std::wstring mWindowTitle;
	bool mIsVsync = false;
};

class RenderManager
{
private:
	RenderManager();
	~RenderManager();

public:
	static RenderManager* GetInst()
	{
		static RenderManager inst;
		return &inst;
	}

public:
	Scene* GetScene()
	{
		return &_mSceneSnapShot;
	}

public:
	const WidgetDrawBuffer& GetWidgetBuffer() const
	{
		return mWidgetBuffer;
	}

public:
	void Init();
	void Destroy();

public:
	void DrawWindow(std::shared_ptr<WindowRenderElementContainer> container);

private:
	void DrawWindow_Internal(const WindowDrawInputs& drawInputs);

	/* 위젯 관련 */
private:
	WidgetDrawBuffer mWidgetBuffer;
	std::shared_ptr<DXWidgetBatchMesh> mWidgetMesh;

	/* 월드 관련 */
private:
	Scene _mSceneSnapShot;
};

