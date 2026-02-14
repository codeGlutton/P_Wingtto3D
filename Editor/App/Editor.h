#pragma once

#include "Core/App/App.h"
#include "Core/App/AppModeBase.h"

#define EDITER App::GetModeInst<Editor>()

class Editor : public AppModeBase
{
public:
	using BuildConstraint = AppBuildConstraint<AppBuildTargetFlag::Editor | AppBuildTargetFlag::DebugAndRelease>;
	//using DefaultWindow = AppWindow;

public:
	Editor();
	virtual ~Editor() override;

public:
	void Init() override;
	void Update() override;
	void End() override;

private:
	void BeginThread() override;

protected:
	virtual void RegisterDefaultResources() override;
	virtual void UnregisterDefaultResources() override;
};

