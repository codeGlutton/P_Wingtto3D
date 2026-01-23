#pragma once

#include "Core/App/Execute.h"

#define APP App::GetInst()

class AppWindow;

struct AppDesc
{
	std::shared_ptr<IExecute> mMode = nullptr;
	std::wstring mName = L"Wingtto";
	HINSTANCE mHInstance = 0;
};

class App
{
private:
	App();
	~App();

public:
	static App* GetInst()
	{
		static App inst;
		return &inst;
	}

	template<typename T> requires std::is_base_of_v<IExecute, T>
	static std::shared_ptr<T> GetModeInst()
	{
		return std::reinterpret_pointer_cast<T>(GetInst()->_mDesc.mMode);
	}

public:
	template<typename T> requires std::is_base_of_v<IExecute, T>
	WPARAM Run(const std::wstring& appName = L"Wingtto");

public:
	const AppDesc& GetDesc() const { return _mDesc; }

public:
	/**
	 * 윈도우 입력 메세지를 받는 콜백 함수
	 * \param hwnd 핸들
	 * \param message 메세지 식별자
	 * \param wParam 추가 데이터 1
	 * \param lParam 추가 데이터 2
	 * \return 처리 결과
	 */
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	template<typename T> requires std::is_base_of_v<IExecute, T>
	void CreateMode();

	/**
	 * 윈도우 창의 메세지를 전달할 클래스를 등록하는 함수
	 * \return 등록 클래스 식별자
	 */
	ATOM RegisterClassToWindow();

private:
	AppDesc _mDesc;
	bool isRun = false;
};

template<typename T> requires std::is_base_of_v<IExecute, T>
inline WPARAM App::Run(const std::wstring& appName)
{
	ASSERT_MSG(isRun == false, "Multiple run app");
	isRun = true;

	_mDesc.mName = appName;
	if (_mDesc.mMode == nullptr)
	{
		CreateMode<T>();
	}

	// 윈도우에 콜백 등록
	RegisterClassToWindow();
	// 부팅 전 초기화
	BOOT_SYSTEM->Init();
	// 리플렉션 및 CDO 등록
	BOOT_SYSTEM->Boot();
	// 모드 내 초기화
	_mDesc.mMode->Init();
	_mDesc.mMode->CreateMainWindow();

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		// 메세지 처리
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			// 모드 내 업데이트
			_mDesc.mMode->Update();
		}
	}
	// 모드 내 종료
	_mDesc.mMode->End();
	// 부팅 종료
	BOOT_SYSTEM->Destroy();

	isRun = false;
	return msg.wParam;
}

template<typename T> requires std::is_base_of_v<IExecute, T>
void App::CreateMode()
{
#ifdef _DEBUG
	STATIC_ASSERT_MSG((T::BuildConstraint::mFlag & AppBuildTargetFlag::Debug) != AppBuildTargetFlag::None, "App mode can't allowed");
#endif // _DEBUG
#ifdef NDEBUG
	STATIC_ASSERT_MSG((T::BuildConstraint::mFlag & AppBuildTargetFlag::Release) != AppBuildTargetFlag::None, "App mode can't allowed");
#endif // NDEBUG
#ifdef _EDITOR
	STATIC_ASSERT_MSG((T::BuildConstraint::mFlag & AppBuildTargetFlag::Editor) != AppBuildTargetFlag::None, "App mode can't allowed");
#endif // _EDITOR
#ifdef NEDITOR
	STATIC_ASSERT_MSG((T::BuildConstraint::mFlag & AppBuildTargetFlag::Game) != AppBuildTargetFlag::None, "App mode can't allowed");
#endif // NEDITOR

	_mDesc.mMode = std::make_shared<T>();
}