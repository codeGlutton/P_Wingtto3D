#pragma once

#define RENDER_MANAGER RenderManager::GetInst()

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
	void Init();
	void Destroy();
};

