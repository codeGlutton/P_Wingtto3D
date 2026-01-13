#pragma once

class AppWindow;

class Game
{
private:
	AppWindow* _mMainWindow;
	std::vector<AppWindow*> _mSubWindows;
};
