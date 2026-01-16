#pragma once

#define APP App::GetInst()

class IExecute;
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

	template<typename T = IExecute>
	static T* GetModeInst()
	{
		// 캐싱
		static T* modeInst = static_cast<T*>(GetInst()->_mDesc.mMode);
		return modeInst;
	}

public:
	const AppDesc& GetDesc() const { return _mDesc; }

public:
	WPARAM Run(AppDesc& desc);

public:
	/**
	 * 윈도우 창의 메세지를 전달할 클래스를 등록하는 함수
	 * \return 등록 클래스 식별자
	 */
	ATOM RegisterClassToWindow();
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
	AppDesc _mDesc;
};
