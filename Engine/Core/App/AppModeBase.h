#pragma once

#include "Core/App/App.h"
#include "Core/App/Execute.h"

class AppModeBase : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void End() override;

protected:
	virtual void BeginThread();
	virtual void EndThread();
};
