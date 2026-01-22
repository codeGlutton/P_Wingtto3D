#pragma once

#include "Core/Object.h"
#include "GraphicMinimum.h"

class Viewport;

struct AppWindowDesc
{
	GEN_STRUCT_REFLECTION(AppWindowDesc)

	HWND mHWnd = 0;
	PROPERTY(mWidth)
	float mWidth = 800.f;
	PROPERTY(mHeight)
	float mHeight = 600.f;
	PROPERTY(mVsync)
	bool mVsync = false;
	PROPERTY(mWindowed)
	bool mWindowed = true;

	std::wstring mName = L"";
};

/**
 * 윈도우 창 객체. 내부 viewport는 렌더 스레드가 관리
 */
class AppWindow : public Object
{
	GEN_REFLECTION(AppWindow)

	friend class AppWindowManager;

public:
	AppWindow();
	virtual ~AppWindow();

protected:
	virtual void PostLoad() override;

protected:
	virtual bool InitWindow();

public:
	const AppWindowDesc& GetDesc() const 
	{
		return _mDesc;
	}

private:
	PROPERTY(_mDesc)
	AppWindowDesc _mDesc;

private:
	// 부모 객체
	PROPERTY(_mOwner)
	std::weak_ptr<AppWindow> _mOwner;

private:
	std::shared_ptr<Viewport> _mRootViewport;
};

