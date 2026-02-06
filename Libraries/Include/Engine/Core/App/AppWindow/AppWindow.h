#pragma once

#include "Graphics/Widget/CompoundWidget.h"
#include "GraphicMinimum.h"

#include "Utils/Thread/RefCounting.h"

#include "Graphics/Widget/Type/HitTestGrid.h"

class WidgetPath;
class VirtualWindow;
struct ArrangedWidget;
struct WindowRenderElementContainer;

struct AppWindowDesc
{
	GEN_STRUCT_REFLECTION(AppWindowDesc)

	ThreadSafeRefCounting<HWND> mHWndRef = nullptr;
	PROPERTY(mWidth)
	float mWidth = 1280.f;
	PROPERTY(mHeight)
	float mHeight = 720.f;
	PROPERTY(mVsync)
	bool mVsync = false;
	PROPERTY(mWindowed)
	bool mWindowed = true;

	PROPERTY(mOffsetX)
	uint32 mOffsetX = CW_USEDEFAULT;
	PROPERTY(mOffsetY)
	uint32 mOffsetY = 0;

	std::wstring mName = L"";
	float mClientWidth;
	float mClientHeight;
};

/**
 * 윈도우 창 객체. 내부 viewport는 렌더 스레드가 관리
 */
class AppWindow : public CompoundWidget
{
	GEN_REFLECTION(AppWindow)

	friend class AppWindowManager;

public:
	AppWindow();
	virtual ~AppWindow();

protected:
	virtual void PostLoad() override;

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
	virtual void OnMove();
	virtual void OnResize(bool isWindowed, const RECT& clientSize);

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
	std::shared_ptr<Widget> GetFocusWidget() const
	{
		return _mFocusWidget.lock();
	}
	void ChangeFocusWidget(std::shared_ptr<Widget> widget);

public:
	virtual void OnBeginFocus() override;
	virtual void OnEndFocus() override;

public:
	bool IsContainScreenPos(POINT pos) const;
	void GetWidgetUnderScreenPos(POINT pos, OUT ArrangedWidget& arrangedWidget) const;
	void GetWidgetPathUnderScreenPos(POINT pos, OUT WidgetPath& path) const;

private:
	PROPERTY(_mDesc)
	AppWindowDesc _mDesc;

	// 부모 객체
	PROPERTY(_mOwner)
	std::weak_ptr<AppWindow> _mOwner;

private:
	std::weak_ptr<Widget> _mFocusWidget;
	std::unique_ptr<HitTestGrid> _mHitTestGrid;
};

