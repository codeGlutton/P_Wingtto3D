#pragma once

#include "Core/App/App.h"
#include "Core/App/AppModeBase.h"
#include "App/AppWindow/EditorAppWindow.h"

#define EDITER App::GetModeInst<Editor>()

class Editor : public AppModeBase
{
public:
	using BuildConstraint = AppBuildConstraint<AppBuildTargetFlag::Editor | AppBuildTargetFlag::DebugAndRelease>;
	using DefaultWindow = EditorAppWindow;

public:
	Editor();
	virtual ~Editor() override;

public:
	void Init() override;
	void Update() override;
	void End() override;

public:
	virtual void OnDropFile(const wchar_t* fileFullPath) override;

private:
	void BeginThread() override;

protected:
	virtual void RegisterDefaultResources() override;
	virtual void UnregisterDefaultResources() override;
};

