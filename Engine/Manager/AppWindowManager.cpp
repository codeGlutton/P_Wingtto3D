#include "pch.h"
#include "AppWindowManager.h"
#include "Core/Resource/Package/Package.h"

#include "Manager/PathManager.h"
#include "Manager/PackageManager.h"
#include "Manager/RenderManager.h"
#include "Manager/TimeManager.h"

#include "Core/Archive.h"
#include "Core/ObjectLinker.h"

#include "Utils/WidgetUtils.h"

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

void AppWindowManager::Update()
{
	std::shared_ptr<AppWindow> focusedWindow = GetFocusWindow();
	if (focusedWindow != nullptr && focusedWindow->IsValid() == true)
	{
		focusedWindow->PrepassWindow();
		std::shared_ptr<WindowRenderElementContainer> container = RENDER_MANAGER->GetWidgetBuffer().GetContainer(focusedWindow);
		focusedWindow->PaintWindow(OUT container, TIME_MANAGER->GetDeltaTime());

		RENDER_MANAGER->DrawWindow(container);
	}
}

void AppWindowManager::Destroy()
{
	Save();
	_mNullWidgetInst.reset();
	_mAppWindows.clear();
	_mPackage = nullptr;
}

std::shared_ptr<NullWidget> AppWindowManager::NullWidgetInst()
{
	AppWindowManager* manager = GetInst();
	if (manager->_mNullWidgetInst == nullptr)
	{
		manager->_mNullWidgetInst = NEW_EDIT_WIDGET(NullWidget).Visibility(VisibilityType::Hidden);
	}
	return manager->_mNullWidgetInst;
}

std::shared_ptr<AppWindow> AppWindowManager::CreateAppWindow(std::shared_ptr<AppWindow> owner, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	std::wstring typeName = ConvertUtf8ToWString(typeInfo->GetName());
	return CreateAppWindow(typeName, owner, typeInfo, flags);
}

std::shared_ptr<AppWindow> AppWindowManager::CreateAppWindow(const std::wstring& name, std::shared_ptr<AppWindow> owner, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	ASSERT_MSG(typeInfo->IsChildOf<AppWindow>() == true, "CreateAppWindow func is not allowed to create non AppWindow class");

	std::shared_ptr<AppWindow> appWindow = std::static_pointer_cast<AppWindow>(CreateAppWindowWidget(name, nullptr, typeInfo, flags));
	if (appWindow == nullptr)
	{
		return nullptr;
	}
	appWindow->_mOwner = owner;
	return appWindow;
}

std::shared_ptr<Widget> AppWindowManager::CreateAppWindowWidget(std::shared_ptr<Widget> parent, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	std::wstring typeName = ConvertUtf8ToWString(typeInfo->GetName());
	return CreateAppWindowWidget(typeName, parent, typeInfo, flags);
}

std::shared_ptr<Widget> AppWindowManager::CreateAppWindowWidget(const std::wstring& name, std::shared_ptr<Widget> parent, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	ASSERT_MSG(typeInfo->IsChildOf<Widget>() == true, "CreateAppWindowWidget func is not allowed to create non Widget class");

	std::shared_ptr<Widget> widget = std::static_pointer_cast<Widget>(NewObject(_mPackage, typeInfo, name, flags));
	if (widget == nullptr)
	{
		return nullptr;
	}
	widget->_mParent = parent;
	return widget;
}

void AppWindowManager::RegisterPackage(std::shared_ptr<Package> package)
{
	_mPackage = CastSharedPointer<AppWindowPackage>(package);
}

void AppWindowManager::Save() const
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
	if (_mFocusHWnd == hWnd)
	{
		return;
	}

	_mFocusHWnd = hWnd;
	if (GetFocusWindow() != nullptr)
	{
		FLASHWINFO flashWin = {};
		flashWin.cbSize = sizeof(FLASHWINFO);
		flashWin.hwnd = hWnd;
		flashWin.dwFlags = FLASHW_TRAY;
		flashWin.uCount = 3;
		::FlashWindowEx(&flashWin);
	}
	mOnChangeFocus.Multicast(GetFocusWindow());
}

void AppWindowManager::NotifyToChangeAppActivation(bool isActive)
{
	_mIsActiveApp = isActive;
}

void AppWindowManager::NotifyToMove(HWND hWnd)
{
	ASSERT_MSG(_mAppWindows.find(hWnd) != _mAppWindows.end(), "Trying to move empty app window");
	_mAppWindows[hWnd]->OnMoveWindow();
}

void AppWindowManager::NotifyToResize(HWND hWnd, bool isWindowed, RECT clientSize)
{
	ASSERT_MSG(_mAppWindows.find(hWnd) != _mAppWindows.end(), "Trying to resize empty app window");
	bool isChangedWindowMode = _mAppWindows[hWnd]->GetDesc().mWindowed != isWindowed;
	_mAppWindows[hWnd]->OnResizeWindow(isWindowed, clientSize);
	if (isChangedWindowMode == true)
	{
		mOnChangeWindowMode.Multicast(_mAppWindows[hWnd], isWindowed);
	}
}
