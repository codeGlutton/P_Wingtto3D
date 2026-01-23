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
	_mPackage = nullptr;
}

std::shared_ptr<AppWindow> AppWindowManager::CreateAppWindow(std::shared_ptr<AppWindow> owner, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	ASSERT_MSG(typeInfo->IsChildOf<AppWindow>() == true, "CreateAppWindow func is not allowed to create non AppWindow class");
	if (owner == nullptr && _mMainWindow.lock() != nullptr)
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
	ASSERT_MSG(_mAppWindows.find(window->GetDesc().mHWnd) == _mAppWindows.end(), "Trying to register same app window");
	_mAppWindows[window->GetDesc().mHWnd] = window;

	if (window->IsMainAppWindow() == true)
	{
		_mMainWindow = window;
	}
	mOnRegisterAppWindow.ExecuteIfBound(window);
}

