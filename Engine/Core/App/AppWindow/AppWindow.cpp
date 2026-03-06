#include "pch.h"
#include "AppWindow.h"

#include "Utils/WidgetUtils.h"

#include "Core/App/App.h"
#include "Graphics/Widget/VirtualWindowSplitter.h"
#include "Graphics/Widget/VirtualWindowContent.h"

#include "Manager/WidgetStyleManager.h"
#include "Manager/AppWindowManager.h"

#include "Graphics/Widget/Type/PaintArgs.h"
#include "Graphics/Widget/Type/WidgetGeometry.h"
#include "Graphics/Widget/Type/WidgetBrush.h"

#include "Graphics/Widget/Type/WidgetPath.h"

#include "Graphics/DXSwapChain.h"
#include "Graphics/Viewport/DXViewport.h"

AppWindow::AppWindow()
{
	_mIsFocusable = true;
}

AppWindow::~AppWindow()
{
}

void AppWindow::PostCreate()
{
	Super::PostCreate();
	_mRenderCache.mRootWindow = std::static_pointer_cast<AppWindow>(shared_from_this());
	_mSwapChainProxy = CreateRefCounting<std::shared_ptr<DXSwapChain>, MainThreadType::Render>();
	_mViewportProxy = CreateRefCounting<std::shared_ptr<DXViewport>, MainThreadType::Render>();
}

void AppWindow::PostLoad()
{
	Super::PostLoad();
	_mDesc.mName = GetName();

	ASSERT_MSG(InitWindow() == true, "Fail to create window");
	APP_WIN_MANAGER->NotifyToAddAppWindow(std::static_pointer_cast<AppWindow>(shared_from_this()));

	_mSwapChainProxy->mData = std::make_shared<DXSwapChain>(static_cast<uint32>(_mDesc.mClientWidth), static_cast<uint32>(_mDesc.mClientHeight), _mDesc.mWindowed, _mDesc.mHWndRef);
	_mViewportProxy->mData = std::make_shared<DXViewport>(_mDesc.mClientWidth, _mDesc.mClientHeight);

	_mNeedInitProxy = true;

	ShowWindow();
}

const AppWindow::SwapChainProxyType& AppWindow::GetSwapChainProxy() const
{
	if (_mNeedInitProxy == true)
	{
		_mNeedInitProxy = false;

		THREAD_MANAGER->PushRenderThreadLogicUpdateJob(ObjectPool<Job>::MakeShared([swapChain = _mSwapChainProxy->mData, viewport = _mViewportProxy->mData]() {
			swapChain->Init();
			viewport->Init();
			}));
	}
	return _mSwapChainProxy;
}

const AppWindow::ViewportProxyType& AppWindow::GetViewportProxy() const
{
	if (_mNeedInitProxy == true)
	{
		_mNeedInitProxy = false;

		THREAD_MANAGER->PushRenderThreadLogicUpdateJob(ObjectPool<Job>::MakeShared([swapChain = _mSwapChainProxy->mData, viewport = _mViewportProxy->mData]() {
			swapChain->Init();
			viewport->Init();
			}));
	}
	return _mViewportProxy;
}

bool AppWindow::InitWindow()
{
	_mHitTestGrid = std::make_unique<HitTestGrid>();

	const AppDesc& appDesc = APP->GetDesc();

	// 원하는 클라이언트 크기
	RECT winRect = { 0, 0, (LONG)_mDesc.mClientWidth, (LONG)_mDesc.mClientHeight };
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
		_mDesc.mWindowOffsetX,				// 창의 초기 가로 위치
		_mDesc.mWindowOffsetY,				// 창의 초기 세로 위치
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

	// 윈도우 크기
	_mDesc.mWindowWidth = static_cast<float>(winRect.right - winRect.left);
	_mDesc.mWindowHeight = static_cast<float>(winRect.bottom - winRect.top);

	// 스크린 상의 윈도우 오프셋 
	::GetWindowRect(_mDesc.mHWndRef->mData, &winRect);
	_mDesc.mWindowOffsetX = winRect.left;
	_mDesc.mWindowOffsetY = winRect.top;

	// 윈도우 툴바 껍질 크기 
	POINT clientTopPoint = {0, 0};
	::ClientToScreen(_mDesc.mHWndRef->mData, &clientTopPoint);
	_mDesc.mToolbarSizeX = clientTopPoint.x - _mDesc.mWindowOffsetX;
	_mDesc.mToolbarSizeY = clientTopPoint.y - _mDesc.mWindowOffsetY;

	if (_mSlotContainer.Size() == 0)
	{
		CreateRootVirtualWindow();
	}

	return true;
}

void AppWindow::CreateRootVirtualWindow()
{
	std::shared_ptr<VirtualWindowSplitter> splitter = NEW_EDIT_WIDGET(VirtualWindowSplitter);
	std::shared_ptr<VirtualWindowContent> content = NEW_EDIT_WIDGET(VirtualWindowContent);

	AddSlot()
		[
			splitter
		];

	splitter->AddChildVirtualWindow(
		content
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
	Vec2 windowOffset = Vec2(static_cast<float>(_mDesc.mWindowOffsetX + _mDesc.mToolbarSizeX), static_cast<float>(_mDesc.mWindowOffsetY + _mDesc.mToolbarSizeY));
	Vec2 windowSize = Vec2(static_cast<float>(_mDesc.mClientWidth), static_cast<float>(_mDesc.mClientHeight));
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
		BoundingAABB2D(0, 0, static_cast<long>(_mDesc.mClientWidth), static_cast<long>(_mDesc.mClientHeight)),
		WidgetInheritedColor()
	);
}

void AppWindow::OnMoveWindow()
{
	RECT windowSize;
	::GetWindowRect(_mDesc.mHWndRef->mData, &windowSize);

	_mDesc.mWindowOffsetX = windowSize.left;
	_mDesc.mWindowOffsetY = windowSize.top;
}

void AppWindow::OnResizeWindow(bool isWindowed, const RECT& clientSize)
{
	_mDesc.mWindowed = isWindowed;

	_mDesc.mClientWidth = static_cast<float>(clientSize.right - clientSize.left);
	_mDesc.mClientHeight = static_cast<float>(clientSize.bottom - clientSize.top);

	RECT windowSize;
	::GetWindowRect(_mDesc.mHWndRef->mData, &windowSize);
	_mDesc.mWindowWidth = static_cast<float>(windowSize.right - windowSize.left);
	_mDesc.mWindowHeight = static_cast<float>(windowSize.bottom - windowSize.top);

	_mDesc.mWindowOffsetX = windowSize.left;
	_mDesc.mWindowOffsetY = windowSize.top;

	POINT clientTopPoint = { 0, 0 };
	::ClientToScreen(_mDesc.mHWndRef->mData, &clientTopPoint);
	_mDesc.mToolbarSizeX = clientTopPoint.x - _mDesc.mWindowOffsetX;
	_mDesc.mToolbarSizeY = clientTopPoint.y - _mDesc.mWindowOffsetY;

	MarkLayoutDirty();
	THREAD_MANAGER->PushRenderThreadLogicUpdateJob(ObjectPool<Job>::MakeShared(
		[width = _mDesc.mClientWidth, height = _mDesc.mClientHeight, swapChain = GetSwapChainProxy()->mData, viewport = GetViewportProxy()->mData]() {
			swapChain->Resize(static_cast<uint32>(width), static_cast<uint32>(height));
			viewport->Resize(width, height);
		})
	);
}

bool AppWindow::IsContainScreenPos(POINT pos) const
{
	if (
		static_cast<uint32>(pos.x) < _mDesc.mWindowOffsetX || 
		static_cast<uint32>(pos.x) >= (_mDesc.mWindowOffsetX + _mDesc.mClientWidth + _mDesc.mToolbarSizeX * 2) ||
		static_cast<uint32>(pos.y) < _mDesc.mWindowOffsetY ||
		static_cast<uint32>(pos.y) >= (_mDesc.mWindowOffsetY + _mDesc.mClientHeight + _mDesc.mToolbarSizeX * 2)
		)
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


