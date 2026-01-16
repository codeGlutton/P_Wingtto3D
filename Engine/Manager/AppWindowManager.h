#pragma once

#include "Core/App/AppWindow/AppWindow.h"

#include "Utils/ObjectUtils.h"

#define APP_WIN_MANAGER AppWindowManager::GetInst()

class AppWindowManager
{
private:
	AppWindowManager();
	~AppWindowManager();

public:
	static AppWindowManager* GetInst()
	{
		static AppWindowManager inst;
		return &inst;
	}

public:
	template<typename T> requires std::is_base_of_v<AppWindow, T>
	std::shared_ptr<T> CreateAppWindow(AppWindow* outer = nullptr);

private:
	std::unordered_map<HWND, std::shared_ptr<AppWindow>> _mAppWindows;
};

template<typename T> requires std::is_base_of_v<AppWindow, T>
inline std::shared_ptr<T> AppWindowManager::CreateAppWindow(AppWindow* outer)
{
	std::shared_ptr<AppWindow> appWindow = NewObject<AppWindow>(outer, &T::GetStaticTypeInfo());
	if (appWindow->InitWindow() == false)
	{
		return nullptr;
	}

	const AppWindowDesc& appWndDesc = appWindow->GetDesc();
	_mAppWindows.insert(std::make_pair(appWndDesc.mHWnd, appWindow));

	return std::static_pointer_cast<T>(appWindow);
}
