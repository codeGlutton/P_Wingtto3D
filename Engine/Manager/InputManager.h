#pragma once

#include "Input/InputInclude.h"
#include "Input/InputBinding.h"
#include "Input/InputAction.h"
#include "Input/InputMapping.h"

#define INPUT_MANAGER InputManager::GetInst()

class AppWindow;

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
	KeyState::Type GetKeyState(KeyType::Type key) const;
	const POINT& GetMousePos() const
	{
		return _mCurrentMousePos;
	}
	template<typename T>
	const T& GetKeyScale(KeyType::Type key) const
	{
		ASSERT_MSG(_mScaleValues.find(key) == _mScaleValues.end(), "None scale key");
		return _mScaleValues[key].Get<T>();
	}

public:
	std::shared_ptr<const InputBinding> Bind(KeyType::Type bindKey, bool withCtrl, bool withAlt, bool withShift, std::shared_ptr<Object> object, OnCallInputBinding::Binder binder);
	DelegateHandle BindAction(SubClass<InputAction> action, KeyState::Type eventState, std::shared_ptr<Object> object, OnTiggerInputAction::Binder binder);
	bool Unbind(std::shared_ptr<const InputBinding> target);
	bool UnbindAction(SubClass<InputAction> action, KeyState::Type eventState, DelegateHandle handle);

public:
	void AddMappingContext(SubClass<InputMappingContext> context, uint8 priority = 0);
	void RemoveMappingContext(SubClass<InputMappingContext> context);

private:
	void InitWinInputDevices();
	void InitDInputDevices();
	void InitXInputDevices();

private:
	void UpdateInputState();
	void UpdateWinInputState();
	void UpdateDInputState();
	void UpdateXInputState();

	void UpdateKeyBindingCallbacks();

private:
	void ChangeTopMappingContextBindings(uint8 prePriority, uint8 curPriority);
	void AddTopMappingContextBindings(uint8 priority);
	void RemoveTopMappingContextBindings(uint8 priority);

private:	
	void NotifyToChangeInputTargetHWnd(std::shared_ptr<AppWindow> window);

private:
	InputSystemType _mSystemType = InputSystemType::DInput;

	/* 키 상태에 따른 함수 바인딩 */
private:
	std::map<uint8, std::unordered_map<const InputMappingContext*, std::vector<std::shared_ptr<InputBinding>>>, std::greater<uint8>> _mMappingContexts;
	std::unordered_map<const InputAction*, std::array<OnTiggerInputAction, KeyState::Count>> _mActionMap;
	std::unordered_map<KeyType::Type, BindingVector> _mBindingMap;

	/* 키 상태 결과 값 */
private:
	std::array<KeyState::Type, KEY_COUNT> _mKeyStates = {};
	std::unordered_map<KeyType::Type, InputValue> _mScaleValues;
	POINT _mPreMousePos;
	POINT _mCurrentMousePos;
	bool _mUseCtrl = false;
	bool _mUseAlt = false;
	bool _mUseShift = false;

	/* 키 상태 계산용 데이터 */
private:
	std::array<KeyType::WinInputType, WIN_INPUT_COUNT> _mWinInputKeyStates = {};
	std::array<KeyType::DInputType, DINPUT_COUNT> _mDInputKeyStates = {};
	DIMOUSESTATE _mDInputMouseState = {};
	XINPUT_STATE _mXInputState = {};

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

