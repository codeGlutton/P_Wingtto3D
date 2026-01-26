#include "pch.h"
#include "InputManager.h"

#include "Core/App/App.h"
#include "Core/App/AppWindow/AppWindow.h"
#include "Manager/AppWindowManager.h"

#include "Utils/InputUtils.h"

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

void InputManager::Init()
{
    _mScaleValues[KeyType::Mouse::Pos] = Vec2();
    _mScaleValues[KeyType::Gamepad::LStick] = Vec2();
    _mScaleValues[KeyType::Gamepad::RStick] = Vec2();
    _mScaleValues[KeyType::Gamepad::LTrigger] = 0.f;
    _mScaleValues[KeyType::Gamepad::RTrigger] = 0.f;

    // 키보드 장치 생성
    if (FAILED(
        DirectInput8Create(
            APP->GetDesc().mHInstance,      // 받아온 핸들 인스턴스
            DIRECTINPUT_VERSION,            // 버전
            IID_IDirectInput8,              // 유니코드인지 뭔지
            (void**)_mDInput.Get(),                 // (출력) DirectInput 객체
            nullptr                         // COM 집계용. 거의 nullptr로 사용
        )
    ))
    {
        // 실패 시 윈도우 모드 입력 사용
        _mSystemType = InputSystemType::Window;
        InitWinInputDevices();
    }
    else
    {
        // 성공 시 초기화
        InitDInputDevices();
    }
    InitXInputDevices();

    _mChangeFocusDelegateHandle = APP_WIN_MANAGER->mOnChangeFocus.BindLambda([this](std::shared_ptr<AppWindow> window) {
        NotifyToChangeInputTargetHWnd(window);
        });
    _mChangeWindowModeDelegateHandle = APP_WIN_MANAGER->mOnChangeWindowMode.BindLambda([this](std::shared_ptr<AppWindow> window, bool isWindowMode) {
        NotifyToChangeInputTargetHWnd(window);
        });
}

void InputManager::Update()
{
    if (APP_WIN_MANAGER->IsActiveApp() == false)
    {
        return;
    }

    UpdateInputState();
    UpdateKeyBindingCallbacks();
}

void InputManager::Destroy()
{
    _mMappingContexts.clear();
    _mActionMap.clear();
    _mBindingMap.clear();

    APP_WIN_MANAGER->mOnChangeWindowMode.Unbind(_mChangeWindowModeDelegateHandle);
    APP_WIN_MANAGER->mOnChangeFocus.Unbind(_mChangeFocusDelegateHandle);
}

KeyState::Type InputManager::GetKeyState(KeyType::Type key) const
{
    return _mKeyStates[key];
}

std::shared_ptr<const InputBinding> InputManager::Bind(KeyType::Type bindKey, bool withCtrl, bool withAlt, bool withShift, std::shared_ptr<Object> object, OnCallInputBinding::Binder binder)
{
    std::shared_ptr<InputBinding> binding = std::make_shared<InputBinding>();

    binding->mCtrl = withCtrl;
    binding->mAlt = withAlt;
    binding->mShift = withShift;
    binding->mOnCallInputBinding.Bind(object, binder);

    _mBindingMap[bindKey][binding.get()] = binding;

	return binding;
}

DelegateHandle InputManager::BindAction(SubClass<InputAction> action, KeyState::Type eventState, std::shared_ptr<Object> object, OnTiggerInputAction::Binder binder)
{
    if (action.IsValid() == false)
    {
        return 0;
    }

    std::shared_ptr<const InputAction> cdo = std::static_pointer_cast<const InputAction>(action->GetDefaultObject());
    DelegateHandle handle = _mActionMap[cdo.get()][eventState].Bind(object, binder);

    return handle;
}

bool InputManager::Unbind(std::shared_ptr<const InputBinding> target)
{
    for (auto& bindingPair : _mBindingMap)
    {
        if (bindingPair.second.erase(target.get()) != 0)
        {
            return true;
        }
    }
    return false;
}

bool InputManager::UnbindAction(SubClass<InputAction> action, KeyState::Type eventState, DelegateHandle handle)
{
    if (action.IsValid() == false)
    {
        return false;
    }
    std::shared_ptr<const InputAction> cdo = std::static_pointer_cast<const InputAction>(action->GetDefaultObject());
    return _mActionMap[cdo.get()][eventState].Unbind(handle);
}

void InputManager::AddMappingContext(SubClass<InputMappingContext> context, uint8 priority)
{
    if (context.IsValid() == false)
    {
        return;
    }
    std::shared_ptr<const InputMappingContext> contextCdo = std::static_pointer_cast<const InputMappingContext>(context->GetDefaultObject());
    
    auto iter = _mMappingContexts.find(priority);
    if (iter != _mMappingContexts.end() || iter->second.find(contextCdo.get()) != iter->second.end())
    {
        return;
    }

    uint8 prePriority = _mMappingContexts.begin() != _mMappingContexts.end() ? 0 : _mMappingContexts.begin()->first;
    for (const InputMapping& mapping : contextCdo->_mMappings)
    {
        std::shared_ptr<const InputAction> actionCdo = std::static_pointer_cast<const InputAction>(context->GetDefaultObject());
        for (const InputBinding& binding : mapping.mBindings)
        {
            std::shared_ptr<InputBinding> bindingInst = std::make_shared<InputBinding>(binding);
            bindingInst->mOnCallInputBinding.BindLambda([actionCdo, this](const InputValue* value, KeyType::Type key) {
                auto iter = _mActionMap.find(actionCdo.get());
                if (iter != _mActionMap.end())
                {
                    InputValue filtered = value;
                    
                    switch (actionCdo->_mValueType)
                    {
                    case KeyValueType::Bool:
                        filtered.Convert<bool>();
                        break;
                    case KeyValueType::Float:
                        filtered.Convert<float>();
                        break;
                    case KeyValueType::Vec2:
                        filtered.Convert<Vec2>();
                        break;
                    }
                    
                    iter->second[_mKeyStates[key]].Multicast(&filtered);
                }
            });

            _mMappingContexts[priority][contextCdo.get()].push_back(bindingInst);
        }
    }

    if (prePriority <= priority)
    {
        ChangeTopMappingContextBindings(prePriority, priority);
    }
}

void InputManager::RemoveMappingContext(SubClass<InputMappingContext> context)
{
    if (context.IsValid() == false)
    {
        return;
    }
    std::shared_ptr<const InputMappingContext> contextCdo = std::static_pointer_cast<const InputMappingContext>(context->GetDefaultObject());
    
    for (auto& contextPair : _mMappingContexts)
    {
        auto iter = contextPair.second.find(contextCdo.get());
        if (iter != contextPair.second.end())
        {
            uint8 prePriority = _mMappingContexts.begin() != _mMappingContexts.end() ? 0 : _mMappingContexts.begin()->first;
            if (prePriority == contextPair.first)
            {
                RemoveTopMappingContextBindings(prePriority);
                contextPair.second.erase(contextCdo.get());
                AddTopMappingContextBindings(prePriority);
            }
            else
            {
                contextPair.second.erase(contextCdo.get());
            }

            return;
        }
    }
}

void InputManager::InitWinInputDevices()
{
}

void InputManager::InitDInputDevices()
{
    // 키보드 장치 생성
    {
        if (FAILED(
            _mDInput->CreateDevice(
                GUID_SysKeyboard,               // 생성할 Device 종류
                _mDInputKeyboard.GetAddressOf(),// (출력) Direct Input Device 객체
                nullptr                         // COM 집계용. 거의 nullptr로 사용
            )
        ))
        {
            FAIL_MSG("Keyboard DInput creation is failed");
            return;
        }

        // Direct Input Device가 반환해야될 데이터 형식 설정
        _mDInputKeyboard->SetDataFormat(&c_dfDIKeyboard); // 키보드 입력 반환해
    }

    // 마우스 장치 생성
    {
        if (FAILED(
            _mDInput->CreateDevice(
                GUID_SysMouse,                  // 생성할 Device 종류
                _mDInputMouse.GetAddressOf(),   // (출력) Direct Input Device 객체
                nullptr                         // COM 집계용. 거의 nullptr로 사용
            )
        ))
        {
            FAIL_MSG("Mouse DInput creation is failed");
            return;
        }

        // 마우스 데이터 형식 설정
        _mDInputMouse->SetDataFormat(&c_dfDIMouse);
    }
}

void InputManager::InitXInputDevices()
{
}

void InputManager::UpdateInputState()
{
    if (_mSystemType == InputSystemType::DInput)
    {
        UpdateDInputState();
    }
    else
    {
        UpdateWinInputState();
    }
    UpdateXInputState();

    _mUseCtrl = IsActiveButtonState(_mKeyStates[KeyType::LCtrl]) || IsActiveButtonState(_mKeyStates[KeyType::RCtrl]);
    _mUseAlt = IsActiveButtonState(_mKeyStates[KeyType::LAlt]) || IsActiveButtonState(_mKeyStates[KeyType::RAlt]);
    _mUseShift = IsActiveButtonState(_mKeyStates[KeyType::LShift]) || IsActiveButtonState(_mKeyStates[KeyType::RShift]);
}

void InputManager::UpdateWinInputState()
{
    std::shared_ptr<AppWindow> focusWindow = APP_WIN_MANAGER->GetFocusWindow();
    if (focusWindow == nullptr)
    {
        return;
    }
    
    if (::GetKeyboardState(_mWinInputKeyStates.data()) == false)
    {
        return;
    }
    
    for (KeyType::Type key = KeyType::MouseStart; key < KeyType::GamepadStart; ++key)
    {
        KeyType::WinInputType winKey = ConvertKeyToWinInputType(key);
        if (winKey == 0)
        {
            continue;
        }
    
        KeyState::Type& state = _mKeyStates[key];
    
        // 키가 눌려 있는 경우
        if ((_mWinInputKeyStates[winKey] & 0x80) != 0x00)
        {
            // 이전 프레임에 키를 누른 상태라면 PRESS
            if (state == KeyState::Press || state == KeyState::Hold)
            {
                state = KeyState::Hold;
            }
            else
            {
                state = KeyState::Press;
            }
        }
        // 키가 떼어져 있는 경우
        else
        {
            // 이전 프레임에 키를 누른 상태라면 UP
            if (state == KeyState::Press || state == KeyState::Hold)
            {
                state = KeyState::Release;
            }
            else
            {
                state = KeyState::None;
            }
        }
    }
    
    POINT mousePos;
    ::GetCursorPos(&mousePos);
    ::ScreenToClient(focusWindow->GetDesc().mHWnd, &mousePos);
    
    Vec2 offset(static_cast<float>(mousePos.x - _mPreMousePos.x), static_cast<float>(mousePos.y - _mPreMousePos.y));
    if (offset.LengthSquared() > std::numeric_limits<float>::epsilon())
    {
        _mKeyStates[KeyType::Mouse::Pos] = KeyState::Trigger;
        offset.Normalize();
        _mScaleValues[KeyType::Mouse::Pos].Set(offset);
    }
    else
    {
        _mKeyStates[KeyType::Mouse::Pos] = KeyState::None;
        _mScaleValues[KeyType::Mouse::Pos].Set(Vec2::Zero);
    }

    _mPreMousePos = _mCurrentMousePos;
    _mCurrentMousePos.x = mousePos.x;
    _mCurrentMousePos.y = mousePos.y;
}

void InputManager::UpdateDInputState()
{
    std::shared_ptr<AppWindow> focusWindow = APP_WIN_MANAGER->GetFocusWindow();
    if (focusWindow == nullptr)
    {
        return;
    }

    if (_mDInputKeyboard != nullptr)
    {
        HRESULT result = _mDInputKeyboard->GetDeviceState(256, static_cast<LPVOID>(_mDInputKeyStates.data()));
        if (FAILED(result))
        {
            // 다시 Acquire 함수로 얻어온다.
            if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
            {
                if (SUCCEEDED(_mDInputKeyboard->Acquire()))
                {
                    result = _mDInputKeyboard->GetDeviceState(static_cast<DWORD>(_mDInputKeyStates.size()), static_cast<LPVOID>(_mDInputKeyStates.data()));
                }
            }
        }

        if (SUCCEEDED(result))
        {
            for (KeyType::Type key = KeyType::KeyBoardStart; key < KeyType::GamepadStart; ++key)
            {
                KeyType::DInputType dInputKey = ConvertKeyToDInputType(key);
                if (dInputKey == 0)
                {
                    continue;
                }

                KeyState::Type& state = _mKeyStates[key];

                // 키가 눌려 있는 경우
                if ((_mDInputKeyStates[dInputKey] & 0x80) != 0x00)
                {
                    // 이전 프레임에 키를 누른 상태라면 PRESS
                    if (state == KeyState::Press || state == KeyState::Hold)
                    {
                        state = KeyState::Hold;
                    }
                    else
                    {
                        state = KeyState::Press;
                    }
                }
                // 키가 떼어져 있는 경우
                else
                {
                    // 이전 프레임에 키를 누른 상태라면 UP
                    if (state == KeyState::Press || state == KeyState::Hold)
                    {
                        state = KeyState::Release;
                    }
                    else
                    {
                        state = KeyState::None;
                    }
                }
            }
        }
    }

    if (_mDInputMouse != nullptr)
    {
        HRESULT result = _mDInputMouse->GetDeviceState(sizeof(_mDInputMouseState), static_cast<LPVOID>(&_mDInputMouseState));
        if (FAILED(result))
        {
            // 다시 Acquire 함수로 얻어온다.
            if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
            {
                if (SUCCEEDED(_mDInputMouse->Acquire()))
                {
                    result = _mDInputMouse->GetDeviceState(sizeof(_mDInputMouseState), static_cast<LPVOID>(&_mDInputMouseState));
                }
            }
        }

        if (SUCCEEDED(result))
        {
            for (KeyType::Type key = KeyType::Mouse::LButton; key < KeyType::Mouse::Pos; ++key)
            {
                KeyState::Type& state = _mKeyStates[key];

                // 키가 눌려 있는 경우
                if ((_mDInputMouseState.rgbButtons[key - KeyType::Mouse::LButton] & 0x80) != 0x00)
                {
                    // 이전 프레임에 키를 누른 상태라면 PRESS
                    if (state == KeyState::Press || state == KeyState::Hold)
                    {
                        state = KeyState::Hold;
                    }
                    else
                    {
                        state = KeyState::Press;
                    }
                }
                // 키가 떼어져 있는 경우
                else
                {
                    // 이전 프레임에 키를 누른 상태라면 UP
                    if (state == KeyState::Press || state == KeyState::Hold)
                    {
                        state = KeyState::Release;
                    }
                    else
                    {
                        state = KeyState::None;
                    }
                }
            }
        }
    }

    POINT mousePos;
    ::GetCursorPos(&mousePos);
    ::ScreenToClient(focusWindow->GetDesc().mHWnd, &mousePos);

    Vec2 offset(static_cast<float>(mousePos.x - _mPreMousePos.x), static_cast<float>(mousePos.y - _mPreMousePos.y));
    if (offset.LengthSquared() > std::numeric_limits<float>::epsilon())
    {
        _mKeyStates[KeyType::Mouse::Pos] = KeyState::Trigger;
        offset.Normalize();
        _mScaleValues[KeyType::Mouse::Pos].Set(offset);
    }
    else
    {
        _mKeyStates[KeyType::Mouse::Pos] = KeyState::None;
        _mScaleValues[KeyType::Mouse::Pos].Set(Vec2::Zero);
    }

    _mPreMousePos = _mCurrentMousePos;
    _mCurrentMousePos.x = mousePos.x;
    _mCurrentMousePos.y = mousePos.y;
}

void InputManager::UpdateXInputState()
{
    ZeroMemory(&_mXInputState, sizeof(_mXInputState));

    DWORD result = XInputGetState(0, &_mXInputState);
    if (result != ERROR_SUCCESS || _mXInputState.dwPacketNumber == 0)
    {
        return;
    }

    for (KeyType::Type key = KeyType::GamepadStart; key <= KeyType::Last; ++key)
    {
        KeyType::XInputType xInputKey = ConvertKeyToXInputType(key);
        if (xInputKey == 0)
        {
            continue;
        }

        KeyState::Type& state = _mKeyStates[key];

        if ((_mXInputState.Gamepad.wButtons & xInputKey) != 0x00000000)
        {
            // 이전 프레임에 키를 누른 상태라면 PRESS
            if (state == KeyState::Press || state == KeyState::Hold)
            {
                state = KeyState::Hold;
            }
            else
            {
                state = KeyState::Press;
            }
        }
        // 키가 떼어져 있는 경우
        else
        {
            // 이전 프레임에 키를 누른 상태라면 UP
            if (state == KeyState::Press || state == KeyState::Hold)
            {
                state = KeyState::Release;
            }
            else
            {
                state = KeyState::None;
            }
        }
    }

    {
        float ratio = static_cast<float>(_mXInputState.Gamepad.bLeftTrigger) / UCHAR_MAX;
        _mKeyStates[KeyType::Gamepad::LTrigger] = _mXInputState.Gamepad.bLeftTrigger != 0 ? KeyState::Trigger : KeyState::None;
        _mScaleValues[KeyType::Gamepad::LTrigger].Set(ratio);
    }
    {
        float ratio = static_cast<float>(_mXInputState.Gamepad.bRightTrigger) / UCHAR_MAX;
        _mKeyStates[KeyType::Gamepad::RTrigger] = _mXInputState.Gamepad.bRightTrigger != 0 ? KeyState::Trigger : KeyState::None;
        _mScaleValues[KeyType::Gamepad::RTrigger].Set(ratio);
    }

    {
        Vec2 axis;
        axis.x = static_cast<float>(_mXInputState.Gamepad.sThumbLX) / SHORT_MAX;
        axis.y = static_cast<float>(_mXInputState.Gamepad.sThumbLY) / SHORT_MAX;
        _mKeyStates[KeyType::Gamepad::LStick] = axis.LengthSquared() > std::numeric_limits<float>::epsilon() ? KeyState::Trigger : KeyState::None;
        axis.Normalize();
        _mScaleValues[KeyType::Gamepad::LStick].Set(axis);
    }
    {
        Vec2 axis;
        axis.x = static_cast<float>(_mXInputState.Gamepad.sThumbRX) / SHORT_MAX;
        axis.y = static_cast<float>(_mXInputState.Gamepad.sThumbRY) / SHORT_MAX;
        _mKeyStates[KeyType::Gamepad::RStick] = axis.LengthSquared() > std::numeric_limits<float>::epsilon() ? KeyState::Trigger : KeyState::None;
        axis.Normalize();
        _mScaleValues[KeyType::Gamepad::RStick].Set(axis);
    }
}

void InputManager::UpdateKeyBindingCallbacks()
{
    for (const auto& bindingPair : _mBindingMap)
    {
        if (bindingPair.second.empty() == true)
        {
            continue;
        }

        const KeyType::Type& keyType = bindingPair.first;

        uint32 maxPriority = 0;
        for (const auto bindingDataPair : bindingPair.second)
        {
            const InputBinding* binding = bindingDataPair.first;

            // 가장 우선 순위 높은 바인딩만 호출 
            // (ex. "컨트롤 + I" 입력 시, "컨트롤 + I" 바인딩 > "I" 바인딩)
            uint8 priority = binding->GetPriority(_mUseCtrl, _mUseAlt, _mUseShift);
            if (maxPriority < priority)
            {
                maxPriority = priority;
            }
        }

        for (const auto bindingDataPair : bindingPair.second)
        {
            const InputBinding* binding = bindingDataPair.first;
            uint8 priority = binding->GetPriority(_mUseCtrl, _mUseAlt, _mUseShift);

            if (maxPriority == priority)
            {
                if (_mScaleValues.find(keyType) == _mScaleValues.end())
                {
                    InputValue inputValue(_mKeyStates[keyType] != KeyState::None);
                    binding->mOnCallInputBinding.ExecuteIfBound(&inputValue, _mKeyStates[keyType]);
                }
                else
                {
                    binding->mOnCallInputBinding.ExecuteIfBound(&_mScaleValues[keyType], _mKeyStates[keyType]);
                }
            }
        }
    }
}

void InputManager::ChangeTopMappingContextBindings(uint8 prePriority, uint8 curPriority)
{
    RemoveTopMappingContextBindings(prePriority);
    RemoveTopMappingContextBindings(curPriority);
}

void InputManager::AddTopMappingContextBindings(uint8 priority)
{
    // 상위 컨텍스트 바인딩 제거
    if (_mMappingContexts.find(priority) != _mMappingContexts.end())
    {
        for (auto& contextPair : _mMappingContexts[priority])
        {
            for (const std::shared_ptr<InputBinding>& binding : contextPair.second)
            {
                _mBindingMap[binding->mKeyType].erase(binding.get());
            }
        }
    }
}

void InputManager::RemoveTopMappingContextBindings(uint8 priority)
{
    // 상위 컨텍스트 바인딩 적용
    if (_mMappingContexts.find(priority) != _mMappingContexts.end())
    {
        for (auto& contextPair : _mMappingContexts[priority])
        {
            for (const std::shared_ptr<InputBinding>& binding : contextPair.second)
            {
                _mBindingMap[binding->mKeyType][binding.get()] = binding;
            }
        }
    }
}

void InputManager::NotifyToChangeInputTargetHWnd(std::shared_ptr<AppWindow> window)
{
    if (_mSystemType != InputSystemType::DInput && window == nullptr)
    {
        return;
    }

    // 입력장치를 윈도우와 어떤 규칙으로 공유 혹은 독점해서
    // 키를 받을지 정하는 함수이다.

    /*
       DISCL_EXCLUSIVE : 입력을 독점으로 한다.
       DISCL_NONEXCLUSIVE : 다른 앱과 입력을 공유한다.
       DISCL_FOREGROUND : 내 윈도우가 활성화(포커스)일 때만 입력
       을 받는다.
       DISCL_BACKGROUND : 내 윈도우가 비활성이어도 입력을 받는
       다.
       DISCL_NOWINKEY : 윈도우 키 같은 시스템 키를 막아준다.
    */

    if (_mDInputKeyboard != nullptr && _mDInputMouse != nullptr)
    {
        // 창모드인지 풀스크린 모드인지 체크.
        if (window->GetDesc().mWindowed == true)
        {
            // 창 모드

            _mDInputKeyboard->SetCooperativeLevel(
                window->GetDesc().mHWnd,                // 윈도우 핸들
                DISCL_NONEXCLUSIVE | DISCL_FOREGROUND   // 타 윈도우와 입력 공유 방식 설정
            );

            _mDInputMouse->SetCooperativeLevel(
                window->GetDesc().mHWnd,                // 윈도우 핸들
                DISCL_NONEXCLUSIVE | DISCL_FOREGROUND   // 타 윈도우와 입력 공유 방식 설정
            );
        }
        else
        {
            // 전체화면 모드

            _mDInputKeyboard->SetCooperativeLevel(
                window->GetDesc().mHWnd,                // 윈도우 핸들
                DISCL_EXCLUSIVE | DISCL_FOREGROUND      // 입력 독점 방식 설정
            );

            _mDInputMouse->SetCooperativeLevel(
                window->GetDesc().mHWnd,                // 윈도우 핸들
                DISCL_EXCLUSIVE | DISCL_FOREGROUND      // 타 윈도우와 입력 공유 방식 설정
            );
        }
    }
}

