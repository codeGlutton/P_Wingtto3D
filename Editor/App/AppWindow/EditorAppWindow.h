#pragma once

#include "Core\App\AppWindow\AppWindow.h"

class EditorAppWindow : public AppWindow
{
	GEN_REFLECTION(EditorAppWindow)

protected:
	virtual void PostLoad() override;

protected:
	virtual void CreateRootVirtualWindow() override;
};

