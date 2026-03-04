#pragma once

#include "Graphics/Render/WidgetDrawBuffer.h"
#include "Graphics/Scene/Scene.h"

#include "Graphics/DXConstantData.h"

#define RENDER_MANAGER RenderManager::GetInst()
#define SCENE RENDER_MANAGER->GetScene()

class DXSwapChain;
class DXViewport;
class DXWidgetBatchMesh;

template<typename T>
class DXConstantBufferTemplate;

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
		return _mWidgetBuffer;
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
	WidgetDrawBuffer _mWidgetBuffer;
	std::shared_ptr<DXWidgetBatchMesh> _mWidgetMesh;

	std::shared_ptr<DXConstantBufferTemplate<WidgetConstantData>> _mWidgetCBuffer;
	std::shared_ptr<DXConstantBufferTemplate<ScreenConstantData>> _mScreenCBuffer;

	/* 월드 관련 */
private:
	Scene _mSceneSnapShot;

	CameraConstantData _mCameraData;
	std::shared_ptr<DXConstantBufferTemplate<CameraConstantData>> _mCameraCBuffer;
};

