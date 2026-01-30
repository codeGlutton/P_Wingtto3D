#include "pch.h"
#include "AppWindowManager.h"
#include "Core/Resource/Package/Package.h"

#include "Manager/PathManager.h"
#include "Manager/PackageManager.h"

#include "Core/Archive.h"
#include "Core/ObjectLinker.h"

AppWindowManager::AppWindowManager()
{
}

AppWindowManager::~AppWindowManager()
{
}

void AppWindowManager::Init()
{
	Load();
}

void AppWindowManager::Destroy()
{
	Save();
	_mAppWindows.clear();
	_mPackage = nullptr;
}

std::shared_ptr<AppWindow> AppWindowManager::CreateAppWindow(std::shared_ptr<AppWindow> owner, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	ASSERT_MSG(typeInfo->IsChildOf<AppWindow>() == true, "CreateAppWindow func is not allowed to create non AppWindow class");
	if (owner == nullptr && GetMainWindow() != nullptr)
	{
		// 이미 메인 윈도우가 있는 상황
		return nullptr;
	}

	std::wstring typeName = ConvertUtf8ToWString(typeInfo->GetName());
	std::shared_ptr<AppWindow> appWindow = std::static_pointer_cast<AppWindow>(NewObject(_mPackage, typeInfo, typeName, flags));
	if (appWindow == nullptr)
	{
		return nullptr;
	}
	appWindow->_mOwner = owner;
	return appWindow;
}

void AppWindowManager::RegisterPackage(std::shared_ptr<Package> package)
{
	_mPackage = CastSharedPointer<AppWindowPackage>(package);
}

void AppWindowManager::Save()
{
	std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();
	packagePath.append(L"\\AppWindows");
	PACKAGE_MANAGER->SavePackage(packagePath, PackageBuildScope::Isolation);
}

void AppWindowManager::Load()
{
	std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();
	packagePath.append(L"\\AppWindows");
	PACKAGE_MANAGER->LoadPackage<AppWindowPackage>(packagePath, PackageBuildScope::Isolation);
}

void AppWindowManager::NotifyToAddAppWindow(std::shared_ptr<AppWindow> window)
{
	ASSERT_MSG(_mAppWindows.find(window->GetDesc().mHWndRef->mData) == _mAppWindows.end(), "Trying to register same app window");
	_mAppWindows[window->GetDesc().mHWndRef->mData] = window;

	if (window->IsMainAppWindow() == true)
	{
		_mMainHWnd = window->GetDesc().mHWndRef->mData;
	}
	mOnRegisterAppWindow.ExecuteIfBound(window);
}

void AppWindowManager::NotifyToCloseAppWindow(HWND hWnd)
{
	ASSERT_MSG(_mAppWindows.find(hWnd) != _mAppWindows.end(), "Trying to close empty app window");
	if (hWnd == _mMainHWnd)
	{
		::PostQuitMessage(0);
	}
	else
	{
		_mAppWindows.erase(hWnd);
	}
}

void AppWindowManager::NotifyToChangeFocus(HWND hWnd)
{
	if (GetFocusWindow() != nullptr)
	{
		GetFocusWindow()->EndFocus();
	}
	_mFocusHWnd = hWnd;
	if (GetFocusWindow() != nullptr)
	{
		GetFocusWindow()->BeginFocus();
	}
	mOnChangeFocus.Multicast(GetFocusWindow());
}

void AppWindowManager::NotifyToChangeAppActivation(bool isActive)
{
	_mIsActiveApp = isActive;
}

void AppWindowManager::NotifyToResize(HWND hWnd, bool isWindowed, RECT clientSize)
{
	ASSERT_MSG(_mAppWindows.find(hWnd) != _mAppWindows.end(), "Trying to resize empty app window");
	bool isChangedWindowMode = _mAppWindows[hWnd]->GetDesc().mWindowed != isWindowed;
	_mAppWindows[hWnd]->OnResize(isWindowed, clientSize);
	if (isChangedWindowMode == true)
	{
		mOnChangeWindowMode.Multicast(_mAppWindows[hWnd], isWindowed);
	}
}
