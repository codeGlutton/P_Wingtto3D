#pragma once

#define GAME GET_SINGLETON(Game)

//struct GameDesc
//{
//	std::shared_ptr<class IExecute> app = nullptr;
//	std::wstring appName = L"DX3DStudy";
//	HINSTANCE hInstance = 0;
//	HWND hwnd = 0;
//	float width = 800.f;
//	float height = 600.f;
//	bool bVsync = false;
//	bool bWindowed = true;
//	Color clearColor = Color(0.5f, 0.5f, 0.5f, 0.5f);
//};
//
///**
// * 메인 게임 객체
// */
//class Game
//{
//	DECLARE_SINGLETON(Game)
//
//public:
//	const GameDesc& GetGameDesc() const { return _desc; }
//	GameDesc& GetGameDesc() { return _desc; }
//
//public:
//	WPARAM Run(GameDesc& desc);
//
//private:
//	void Update();
//	void ShowDebugInfo();
//
//public:
//	/**
//	 * 윈도우 창의 메세지를 전달할 클래스를 등록하는 함수
//	 * \return 등록 클래스 식별자
//	 */
//	ATOM RegisterClassToWindow();
//	/**
//	 * 실제 창 인스턴스를 만들고 실행하는 함수
//	 * \param cmdShow cmd 창 표기 여부
//	 * \return 성공 여부
//	 */
//	BOOL InitInst(int cmdShow);
//	/**
//	 * 윈도우 입력 메세지를 받는 콜백 함수
//	 * \param hwnd 핸들
//	 * \param message 메세지 식별자
//	 * \param wParam 추가 데이터 1
//	 * \param lParam 추가 데이터 2
//	 * \return 처리 결과
//	 */
//	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//
//private:
//	GameDesc _desc;
//};
//
