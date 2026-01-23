#pragma once

#include "Core/App/App.h"
#include "Core/App/Execute.h"

#define EDITER App::GetModeInst<Editor>()

class Editor : public IExecute
{
public:
	using BuildConstraint = AppBuildConstraint<AppBuildTargetFlag::Editor | AppBuildTargetFlag::DebugAndRelease>;

public:
	Editor();
	virtual ~Editor() override;

public:
	void Init() override;
	void Update() override;
	void End() override;

private:
	void CreateMainWindow() override;
};

