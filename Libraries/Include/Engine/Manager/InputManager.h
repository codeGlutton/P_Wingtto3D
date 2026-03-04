#pragma once

#include "Input/InputInclude.h"
#include "Input/InputState.h"

#include "Core/App/InputMessageData.h"

#include "Graphics/Widget/Type/WidgetPath.h"

#define INPUT_MANAGER InputManager::GetInst()

class AppWindow;
struct InputBinding;

enum class InputSystemType : uint8
{
	DInput,
	Window
};

class InputManager
{
	using BindingVector = std::vector<std::shared_ptr<InputBinding>>;

private:
	InputManager();
	~InputManager();

public:
	static InputManager* GetInst()
	{
		static InputManager inst;
		return &inst;
	}

public:
	void Init();
	void Update();
	void Destroy();

public:
	KeyState::Type GetKeyState(KeyType::Type key) const
	{
		return _mState->mKeyStates[key];
	}
	const POINT& GetMousePos() const
	{
		return _mState->mCurrentMouseScreenPos;
	}
	template<typename T>
	const T& GetKeyScale(KeyType::Type key) const
	{
		ASSERT_MSG(_mState->mScaleValues.find(key) == _mState->mScaleValues.end(), "None scale key");
		return _mState->mScaleValues[key].Get<T>();
	}

public:
	void PushWinKeyMessage(const WindowMessageData& data);
	void PushRawWinKeyMessage(WindowMessageData data);

private:
	void ProcessWinKeyMessagesForWinInput();
	void ProcessWinKeyMessagesForDInput();

private:
	void InitWinInputDevices();
	void InitDInputDevices();
	void InitXInputDevices();

private:
	//void UpdateWinInputState();
	void UpdateDInputState();
	void UpdateXInputState();
	void UpdateAdditionalMouseState();

private:	
	void NotifyToChangeInputTargetHWnd(std::shared_ptr<AppWindow> window);

private:
	InputSystemType _mSystemType = InputSystemType::Window;

	/* 키 상태 결과 값 */
private:
	std::shared_ptr<InputState> _mState;

	/* 키 상태 계산용 데이터 */
private:
	//std::array<KeyType::WinInputType, WIN_INPUT_COUNT> _mWinInputKeyStates = {};
	std::array<KeyType::DInputType, DINPUT_COUNT> _mDInputKeyStates = {};
	DIMOUSESTATE _mDInputMouseState = {};
	XINPUT_STATE _mXInputState = {};

	const double _mDoubleClickThreshold = 0.25;
	double _mLClickPreReleaseTime = -1.f;
	double _mRClickPreReleaseTime = -1.f;

	// 윈도우 키 변경 누적 메세지
	std::vector<WindowMessageData> _mWinKeyMessages;

	/* DInput Device 객체들 */
private:
	ComPtr<IDirectInput8> _mDInput;
	ComPtr<IDirectInputDevice8> _mDInputKeyboard;
	ComPtr<IDirectInputDevice8> _mDInputMouse;

	/* 대리자 핸들 */
private:
	DelegateHandle _mChangeFocusDelegateHandle;
	DelegateHandle _mChangeWindowModeDelegateHandle;
};

