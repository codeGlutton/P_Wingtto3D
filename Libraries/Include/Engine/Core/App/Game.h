#pragma once

#include "Core/App/App.h"
#include "Core/App/AppModeBase.h"
#include "Core/App/AppWindow/GameAppWindow.h"

#define GAME App::GetModeInst<Game>()

class Game : public AppModeBase
{
public:
	using BuildConstraint = AppBuildConstraint<AppBuildTargetFlag::Game | AppBuildTargetFlag::DebugAndRelease>;
	using DefaultWindow = GameAppWindow;

public:
	Game();
	virtual ~Game() override;

public:
	void Init() override;
	void Update() override;
	void End() override;
};
