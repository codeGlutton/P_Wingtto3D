#pragma once

#include "Graphics/Widget/CompoundWidget.h"
#include "GraphicMinimum.h"

#include "Utils/Thread/RefCounting.h"

#include "Graphics/Widget/Type/HitTestGrid.h"

class DXSwapChain;
class DXViewport;

class WidgetPath;
class VirtualWindow;
struct ArrangedWidget;
struct WindowRenderElementContainer;

struct AppWindowDesc
{
	GEN_STRUCT_REFLECTION(AppWindowDesc)

	ThreadSafeRefCounting<HWND> mHWndRef = nullptr;
	PROPERTY(mClientWidth)
	float mClientWidth = 1280.f;
	PROPERTY(mClientHeight)
	float mClientHeight = 720.f;
	PROPERTY(mVsync)
	bool mVsync = false;
	PROPERTY(mWindowed)
	bool mWindowed = true;

	PROPERTY(mWindowOffsetX)
	uint32 mWindowOffsetX = CW_USEDEFAULT;
	PROPERTY(mWindowOffsetY)
	uint32 mWindowOffsetY = 0;

	std::wstring mName = L"";
	float mWindowWidth;
	float mWindowHeight;

	uint32 mToolbarSizeX = 0;
	uint32 mToolbarSizeY = 0;
};

/**
 * 윈도우 창 객체. 내부 viewport는 렌더 스레드가 관리
 */
class AppWindow : public CompoundWidget
{
	GEN_REFLECTION(AppWindow)

	friend class AppWindowManager;

public:
	using SwapChainProxyType = ThreadSafeRefCounting<std::shared_ptr<DXSwapChain>, MainThreadType::Render>;
	using ViewportProxyType = ThreadSafeRefCounting<std::shared_ptr<DXViewport>, MainThreadType::Render>;

public:
	AppWindow();
	virtual ~AppWindow();

protected:
	virtual void PostCreate() override;
	virtual void PostLoad() override;

public:
	const SwapChainProxyType& GetSwapChainProxy() const;
	const ViewportProxyType& GetViewportProxy() const;

protected:
	bool InitWindow();
	virtual void CreateRootVirtualWindow();
	void ShowWindow();

protected:
	virtual void Update(const WidgetGeometry& screenGeometry, float deltaTime) override;

protected:
	void PrepassWindow();
	void PaintWindow(OUT std::shared_ptr<WindowRenderElementContainer> container, float deltaTime);

protected:
	void OnMoveWindow();
	void OnResizeWindow(bool isWindowed, const RECT& clientSize);

public:
	const AppWindowDesc& GetDesc() const 
	{
		return _mDesc;
	}
	const std::shared_ptr<AppWindow>& GetOwner() const
	{
		return _mOwner.lock();
	}
	bool IsMainAppWindow() const
	{
		return _mOwner.lock() == nullptr;
	}
	float GetDPIScale() const
	{
		return ::GetDpiForWindow(_mDesc.mHWndRef->mData) / 96.f;
	}

public:
	bool IsContainScreenPos(POINT pos) const;
	void GetWidgetUnderScreenPos(POINT pos, OUT ArrangedWidget& arrangedWidget) const;
	void GetWidgetPathUnderScreenPos(POINT pos, OUT WidgetPath& path) const;

private:
	PROPERTY(_mDesc)
	AppWindowDesc _mDesc;

	// 메인 윈도우를 의미
	PROPERTY(_mOwner)
	std::weak_ptr<AppWindow> _mOwner;

private:
	SwapChainProxyType _mSwapChainProxy;
	ViewportProxyType _mViewportProxy;

private:
	mutable bool _mNeedInitProxy = false;

private:
	std::unique_ptr<HitTestGrid> _mHitTestGrid;
};

