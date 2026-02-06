#include "pch.h"
#include "App.h"

#include <windowsx.h>

#include "Manager/InputManager.h"

App::App()
{
}

App::~App()
{
}

LRESULT App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		/* 입력 관련 */
	case WM_CHAR:
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
	{
		INPUT_MANAGER->PushWinKeyMessage(WindowMessageData(hWnd, message, wParam, lParam));
		break;
	}
	case WM_INPUT:
	{
		INPUT_MANAGER->PushRawWinKeyMessage(WindowMessageData(hWnd, message, wParam, lParam));
		break;
	}

		/* 포커스 관련 */
	case WM_ACTIVATEAPP:
	{
		APP_WIN_MANAGER->NotifyToChangeAppActivation(wParam == TRUE);
		break;
	}
	case WM_SETFOCUS:
	{
		APP_WIN_MANAGER->NotifyToChangeFocus(hWnd);
		break;
	}
	case WM_KILLFOCUS:
	{
		APP_WIN_MANAGER->NotifyToChangeFocus(NULL);
		break;
	}

		/* 창 이동 관련 */
	case WM_MOVE:
	{
		APP_WIN_MANAGER->NotifyToMove(hWnd);
		break;
	}

		/* 창 크기 관련 */
	case WM_SIZE:
	{
		uint32 state = (uint32)wParam;
		RECT rect = { 0, 0, LOWORD(lParam), HIWORD(lParam) };

		APP_WIN_MANAGER->NotifyToResize(hWnd, state != SIZE_MAXIMIZED, rect);
		break;
	}
		/* 창 생명주기 관련 */
	case WM_CLOSE:
	case WM_DESTROY:
	{
		APP_WIN_MANAGER->NotifyToCloseAppWindow(hWnd);
		break;
	}

	default:
	{
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	}
	return LRESULT();
}

ATOM App::RegisterClassToWindow()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);										// 클래스 혹은 구조체 크기
	wcex.style = CS_HREDRAW | CS_VREDRAW;									// 클래스 스타일 (윈도우 창 설정 영향)
	wcex.lpfnWndProc = WndProc;												// (창 프로시저 포인터) 콜백 함수 등록							   
	wcex.cbClsExtra = 0;													// 클래스 혹은 구조체 다음 추가 할당 바이트
	wcex.cbWndExtra = 0;													// 창 인스턴스 다음 추가 할당 바이트
	wcex.hInstance = _mDesc.mHInstance;										// 창 인스턴스 핸들
	wcex.hIcon = ::LoadIcon(NULL, IDI_WINLOGO);								// 창 아이콘 핸들
	wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);						// 창 커서 핸들
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);						// 배경 브러시 핸들
	wcex.lpszMenuName = NULL;												// 리소스 파일 상에서의 이름
	wcex.lpszClassName = _mDesc.mName.c_str();								// 기본적인 클래스 이름
	wcex.hIconSm = ::LoadIcon(NULL, IDI_WINLOGO);							// 작은 아이콘 핸들

	return ::RegisterClassExW(&wcex);
}

