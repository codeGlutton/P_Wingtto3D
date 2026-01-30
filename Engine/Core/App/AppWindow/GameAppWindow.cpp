#include "pch.h"
#include "GameAppWindow.h"

#include "Manager/AppWindowManager.h"

#include "Graphics/Viewport/SceneViewport.h"

void GameAppWindow::CreateRootViewport()
{
	std::wstring typeName = ConvertUtf8ToWString(SceneViewport::GetStaticTypeInfo().GetName());
	_mRootViewport = NewObject<SceneViewport>(GetOuter(), typeName);
}
