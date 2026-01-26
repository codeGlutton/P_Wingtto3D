#pragma once

#include "Core/App/AppWindow/AppWindow.h"

#include "Core/Resource/Package/PackageRuntimeOwner.h"
#include "Utils/ObjectUtils.h"

#define APP_WIN_MANAGER AppWindowManager::GetInst()
#define MAIN_WINDOW AppWindowManager::GetInst()->GetMainWindow()

class AppWindowPackage;

DECLARE_DELEGATE_1_PARAM(OnRegisterAppWindow, std::shared_ptr<AppWindow>);
DECLARE_MULTICAST_DELEGATE_1_PARAM(OnChangeFocus, std::shared_ptr<AppWindow>);
DECLARE_MULTICAST_DELEGATE_2_PARAMS(OnChangeWindowMode, std::shared_ptr<AppWindow>, bool);

class AppWindowManager : public IPackageRuntimeOwner
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
	void Init();
	void Destroy();

public:
	std::shared_ptr<AppWindow> GetAppWindow(HWND hWnd) const
	{
		auto iter = _mAppWindows.find(hWnd);
		if (iter == _mAppWindows.end())
		{
			return nullptr;
		}
		return (*iter).second;
	}
	std::shared_ptr<AppWindow> GetMainWindow() const
	{
		return GetAppWindow(_mMainHWnd);
	}
	std::shared_ptr<AppWindow> GetFocusWindow() const
	{
		return GetAppWindow(_mFocusHWnd);
	}
	bool IsActiveApp() const
	{
		return _mIsActiveApp;
	}

public:
	template<typename T> requires std::is_base_of_v<AppWindow, T>
	std::shared_ptr<T> CreateAppWindow(std::shared_ptr<AppWindow> owner = nullptr, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
	std::shared_ptr<AppWindow> CreateAppWindow(std::shared_ptr<AppWindow> owner, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);

public:
	virtual void RegisterPackage(std::shared_ptr<Package> package) override;
	virtual void Save() override;
	virtual void Load() override;

public:
	void NotifyToAddAppWindow(std::shared_ptr<AppWindow> window);
	void NotifyToCloseAppWindow(HWND hWnd);

public:
	void NotifyToChangeFocus(HWND hWnd);
	void NotifyToChangeAppActivation(bool isActive);

public:
	void NotifyToResize(HWND hWnd, bool isWindowed, RECT clientSize);

public:
	OnRegisterAppWindow mOnRegisterAppWindow;
	OnChangeFocus mOnChangeFocus;
	OnChangeWindowMode mOnChangeWindowMode;

private:
	std::shared_ptr<AppWindowPackage> _mPackage;
	HWND _mMainHWnd = NULL;
	std::unordered_map<HWND, std::shared_ptr<AppWindow>> _mAppWindows;

private:
	HWND _mFocusHWnd = NULL;
	bool _mIsActiveApp = true;
};

template<typename T> requires std::is_base_of_v<AppWindow, T>
inline std::shared_ptr<T> AppWindowManager::CreateAppWindow(std::shared_ptr<AppWindow> owner, ObjectCreateFlag::Type flags)
{
	return std::static_pointer_cast<T>(CreateAppWindow(owner, &T::GetStaticTypeInfo(), flags));
}

