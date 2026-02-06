#include "pch.h"
#include "AppWindow.h"

#include "Core/App/App.h"
#include "Graphics/VirtualWindow/VirtualWindowContent.h"

#include "Manager/AppWindowManager.h"

#include "Graphics/Widget/Type/PaintArgs.h"
#include "Graphics/Widget/Type/WidgetGeometry.h"
#include "Graphics/Widget/Type/WidgetBrush.h"

#include "Graphics/Widget/Type/WidgetPath.h"

AppWindow::AppWindow()
{
	_mIsFocusable = true;
}

AppWindow::~AppWindow()
{
}

void AppWindow::PostLoad()
{
	Super::PostLoad();
	ASSERT_MSG(InitWindow() == true, "Fail to create window");
	APP_WIN_MANAGER->NotifyToAddAppWindow(std::static_pointer_cast<AppWindow>(shared_from_this()));
	ShowWindow();
}

bool AppWindow::InitWindow()
{
	_mHitTestGrid = std::make_unique<HitTestGrid>();

	const AppDesc& appDesc = APP->GetDesc();

	RECT winRect = { 0, 0, (LONG)_mDesc.mWidth, (LONG)_mDesc.mHeight };
	::AdjustWindowRect(
		&winRect,
		WS_OVERLAPPEDWINDOW,
		false
	);

	_mDesc.mHWndRef = CreateRefCounting<HWND>([](const HWND& hWnd) {
		::DestroyWindow(hWnd);
		});

	_mDesc.mHWndRef->mData = ::CreateWindowW(
		appDesc.mName.c_str(),				// 기본적인 클래스 이름
		_mDesc.mName.c_str(),				// 윈도우 창 이름
		WS_OVERLAPPEDWINDOW,				// 생성되는 창 스타일
		_mDesc.mOffsetX,					// 창의 초기 가로 위치
		_mDesc.mOffsetY,					// 창의 초기 세로 위치
		winRect.right - winRect.left,		// 창의 너비
		winRect.bottom - winRect.top,		// 창의 높이
		nullptr,							// 창의 부모 핸들
		nullptr,							// 메뉴 핸들
		appDesc.mHInstance,					// 창과 연결할 모듈 핸들
		nullptr								// 추가 전달 데이터
	);

	if (_mDesc.mHWndRef->mData == NULL)
	{
		return false;
	}

	::GetClientRect(_mDesc.mHWndRef->mData, &winRect);
	_mDesc.mClientWidth = static_cast<float>(winRect.right - winRect.left);
	_mDesc.mClientHeight = static_cast<float>(winRect.bottom - winRect.top);

	::GetWindowRect(_mDesc.mHWndRef->mData, &winRect);
	_mDesc.mOffsetX = winRect.left;
	_mDesc.mOffsetY = winRect.top;

	if (_mSlotContainer.Size() == 0)
	{
		CreateRootVirtualWindow();
	}

	return true;
}

void AppWindow::CreateRootVirtualWindow()
{
	_mSlotContainer.AddChild(
		APP_WIN_MANAGER->CreateAppWindowWidget<VirtualWindowContent>(
			std::static_pointer_cast<Widget>(shared_from_this())
		)
	);
}

void AppWindow::ShowWindow()
{
	::ShowWindow(_mDesc.mHWndRef->mData, SW_SHOW);
	::SetFocus(_mDesc.mHWndRef->mData);
	::UpdateWindow(_mDesc.mHWndRef->mData);
}

void AppWindow::Update(const WidgetGeometry& screenGeometry, float deltaTime)
{
	Super::Update(screenGeometry, deltaTime);

}

void AppWindow::PrepassWindow()
{
	Prepass(Vec2(GetDPIScale()));
}

void AppWindow::PaintWindow(OUT std::shared_ptr<WindowRenderElementContainer> container, float deltaTime)
{
	Vec2 windowOffset = Vec2(static_cast<float>(_mDesc.mOffsetX), static_cast<float>(_mDesc.mOffsetY));
	Vec2 windowSize = Vec2(static_cast<float>(_mDesc.mWidth), static_cast<float>(_mDesc.mHeight));
	Transform2D windowTransform = Transform2D(Vec2(), 0.f, Vec2(GetDPIScale()));

	_mHitTestGrid->SetHitTestArea(windowOffset, windowSize);
	_mHitTestGrid->Clear();
	PaintArgs paintArgs(nullptr, *_mHitTestGrid.get(), windowOffset, deltaTime);
	WidgetGeometry windowGeometry = WidgetGeometry::MakeRoot(windowSize / Vec2(GetDPIScale()), windowTransform);

	uint32 maxLayerId = 0;
	maxLayerId = Paint(
		*container.get(), 
		maxLayerId, 
		paintArgs, 
		windowGeometry, 
		BoundingAABB2D(0, 0, static_cast<long>(_mDesc.mWidth), static_cast<long>(_mDesc.mHeight)),
		WidgetInheritedColor()
	);
}

void AppWindow::OnMove()
{
	RECT windowSize;
	::GetWindowRect(_mDesc.mHWndRef->mData, &windowSize);

	_mDesc.mOffsetX = windowSize.left;
	_mDesc.mOffsetY = windowSize.top;
}

void AppWindow::OnResize(bool isWindowed, const RECT& clientSize)
{
	_mDesc.mWindowed = isWindowed;

	_mDesc.mClientWidth = static_cast<float>(clientSize.right - clientSize.left);
	_mDesc.mClientHeight = static_cast<float>(clientSize.bottom - clientSize.top);

	RECT windowSize;
	::GetWindowRect(_mDesc.mHWndRef->mData, &windowSize);
	_mDesc.mWidth = static_cast<float>(windowSize.right - windowSize.left);
	_mDesc.mHeight = static_cast<float>(windowSize.bottom - windowSize.top);

	_mDesc.mOffsetX = windowSize.left;
	_mDesc.mOffsetY = windowSize.top;

	_mNeedToPrepass = true;
}

void AppWindow::ChangeFocusWidget(std::shared_ptr<Widget> widget)
{
	std::shared_ptr<Widget> preWidget = _mFocusWidget.lock();
	if (widget == preWidget)
	{
		return;
	}

	if (preWidget != nullptr)
	{
		preWidget->OnEndFocus();
	}
	_mFocusWidget = widget;
	if (widget != nullptr)
	{
		widget->OnBeginFocus();
	}
}

void AppWindow::OnBeginFocus()
{
	Super::OnBeginFocus();
	ChangeFocusWidget(nullptr);
}

void AppWindow::OnEndFocus()
{
	ChangeFocusWidget(nullptr);
	Super::OnEndFocus();
}

bool AppWindow::IsContainScreenPos(POINT pos) const
{
	if (static_cast<uint32>(pos.x) < _mDesc.mOffsetX || static_cast<uint32>(pos.x) >= _mDesc.mOffsetX + _mDesc.mWidth || static_cast<uint32>(pos.y) < _mDesc.mOffsetY || static_cast<uint32>(pos.y) >= _mDesc.mOffsetY + _mDesc.mHeight)
	{
		return false;
	}
	return true;
}

void AppWindow::GetWidgetUnderScreenPos(POINT pos, OUT ArrangedWidget& arrangedWidget) const
{
	if (IsContainScreenPos(pos) == false)
	{
		return;
	}
	_mHitTestGrid->GetWidgetUnderScreenPos(pos, arrangedWidget);
}

void AppWindow::GetWidgetPathUnderScreenPos(POINT pos, OUT WidgetPath& path) const
{
	if (IsContainScreenPos(pos) == false)
	{
		return;
	}
	_mHitTestGrid->GetWidgetPathUnderScreenPos(pos, path);
}


