#pragma once

#include "Core\App\AppWindow\AppWindow.h"

class GameAppWindow : public AppWindow
{
	GEN_REFLECTION(GameAppWindow)

protected:
	virtual void CreateRootVirtualWindow() override;
};

