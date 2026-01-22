#include "pch.h"
#include "App.h"

App::App()
{
}

App::~App()
{
}

LRESULT App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hwnd, message, wParam, lParam);
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

