#pragma once

struct WindowMessageData
{
public:
	WindowMessageData(HWND hWnd, uint32 msgType, WPARAM w, LPARAM l, POINT mousePos = POINT()) :
		mHWnd(hWnd),
		mMsgType(msgType),
		mWParam(w),
		mLParam(l),
		mMousePos(mousePos)
	{
	}

public:
	HWND mHWnd;
	uint32 mMsgType;
	WPARAM mWParam;
	LPARAM mLParam;

	// 마우스 기록용
	POINT mMousePos;
};

