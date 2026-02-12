#pragma once

#include "Graphics/Render/WidgetDrawBuffer.h"
#include "Graphics/Scene/Scene.h"

#define RENDER_MANAGER RenderManager::GetInst()
#define SCENE RENDER_MANAGER->GetScene()

struct WindowDrawInputs
{
	std::shared_ptr<WindowRenderElementContainer> mContainer;
	float mGameThreadTime;
	float mGameThreadDeltaTime;
	Vec2 mWindowSize;
	std::string mWindowTitle;
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
	void Init();
	void Destroy();

public:
	void DrawWindow();

public:
	const WidgetDrawBuffer& GetWidgetBuffer() const
	{
		return mWidgetBuffer;
	}

private:
	WidgetDrawBuffer mWidgetBuffer;
	Scene _mSceneSnapShot;
};

