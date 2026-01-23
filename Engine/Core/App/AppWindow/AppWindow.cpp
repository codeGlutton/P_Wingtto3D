#include "pch.h"
#include "AppWindow.h"

#include "Core/App/App.h"

#include "Manager/AppWindowManager.h"

AppWindow::AppWindow()
{
}

AppWindow::~AppWindow()
{
}

void AppWindow::PostLoad()
{
	Super::PostLoad();
	ASSERT_MSG(InitWindow() == true, "Fail to create window");
	APP_WIN_MANAGER->NotifyToAddAppWindow(std::static_pointer_cast<AppWindow>(shared_from_this()));
}

bool AppWindow::InitWindow()
{
	const AppDesc& appDesc = APP->GetDesc();

	RECT winRect = { 0, 0, (LONG)_mDesc.mWidth, (LONG)_mDesc.mHeight };
	::AdjustWindowRect(
		&winRect,
		WS_OVERLAPPEDWINDOW,
		false
	);

	_mDesc.mHWnd = ::CreateWindowW(
		appDesc.mName.c_str(),				// 기본적인 클래스 이름
		_mDesc.mName.c_str(),			// 윈도우 창 이름
		WS_OVERLAPPEDWINDOW,				// 생성되는 창 스타일
		CW_USEDEFAULT,						// 창의 초기 가로 위치
		0,									// 창의 초기 세로 위치
		winRect.right - winRect.left,		// 창의 너비
		winRect.bottom - winRect.top,		// 창의 높이
		nullptr,							// 창의 부모 핸들
		nullptr,							// 메뉴 핸들
		appDesc.mHInstance,					// 창과 연결할 모듈 핸들
		nullptr								// 추가 전달 데이터
	);

	if (_mDesc.mHWnd == NULL)
	{
		return false;
	}

	::ShowWindow(_mDesc.mHWnd, TRUE);
	::UpdateWindow(_mDesc.mHWnd);

	return true;
}
