#pragma once

struct AppWindowDesc
{
	HWND hwnd = 0;
	float width = 800.f;
	float height = 600.f;
	bool bVsync = false;
	bool bWindowed = true;
};

/**
 * 윈도우 창 객체. 내부 viewport는 렌더 스레드가 관리
 */
class AppWindow
{
public:
	const AppWindowDesc& GetGameDesc() const 
	{
		return _mDesc;
	}

private:
	AppWindowDesc _mDesc;
};

