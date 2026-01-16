#pragma once

#include "Core/App/App.h"
#include "Core/App/Execute.h"

#define GAME App::GetModeInst<Game>()

class Game : public IExecute
{
public:
	Game();
	virtual ~Game() override;

public:
	void Create() override;
	void Init() override;
	void Update() override;
	void End() override;

private:
	void CreateMainWindow();

private:
	std::shared_ptr<AppWindow> _mMainWindow;
	//std::vector<std::shared_ptr<AppWindow>> _mSubWindows; 에디터 전용
};
