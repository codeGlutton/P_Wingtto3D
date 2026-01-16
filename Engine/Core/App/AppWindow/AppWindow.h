#pragma once

#include "Core/Object.h"
#include "GraphicMinimum.h"

class Viewport;
class ViewSplitter;

struct AppWindowDesc
{
	HWND mHWnd = 0;
	float mWidth = 800.f;
	float mHeight = 600.f;
	bool mVsync = false;
	bool mWindowed = true;

	std::wstring mName = L"";
};

/**
 * 윈도우 창 객체. 내부 viewport는 렌더 스레드가 관리
 */
class AppWindow : public Object
{
	GEN_REFLECTION(AppWindow)

public:
	AppWindow();
	virtual ~AppWindow();

public:
	virtual bool InitWindow();

public:
	const AppWindowDesc& GetDesc() const 
	{
		return _mDesc;
	}

private:
	AppWindowDesc _mDesc;

private:
	std::shared_ptr<ViewSplitter> _mRoot;
};

