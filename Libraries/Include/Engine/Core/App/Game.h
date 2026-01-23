#pragma once

#include "Core/App/App.h"
#include "Core/App/Execute.h"

#define GAME App::GetModeInst<Game>()

class Game : public IExecute
{
public:
	using BuildConstraint = AppBuildConstraint<AppBuildTargetFlag::Game | AppBuildTargetFlag::DebugAndRelease>;

public:
	Game();
	virtual ~Game() override;

public:
	void Init() override;
	void Update() override;
	void End() override;

private:
	void CreateMainWindow() override;
};
