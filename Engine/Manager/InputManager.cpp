#include "pch.h"
#include "InputManager.h"

#include "Core/App/App.h"
#include "Core/App/AppWindow/AppWindow.h"
#include "Manager/AppWindowManager.h"

#include "Manager/TimeManager.h"

#include "Utils/InputUtils.h"

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

void InputManager::Init()
{
    _mState = std::make_shared<InputState>();

    _mState->mScaleValues[KeyType::Mouse::Pos] = Vec2();
    _mState->mScaleValues[KeyType::Gamepad::LStick] = Vec2();
    _mState->mScaleValues[KeyType::Gamepad::RStick] = Vec2();
    _mState->mScaleValues[KeyType::Gamepad::LTrigger] = 0.f;
    _mState->mScaleValues[KeyType::Gamepad::RTrigger] = 0.f;

    // 키보드 장치 생성
    if (_mSystemType == InputSystemType::DInput)
    {
        if (FAILED(
            DirectInput8Create(
                APP->GetDesc().mHInstance,      // 받아온 핸들 인스턴스
                DIRECTINPUT_VERSION,            // 버전
                IID_IDirectInput8,              // 유니코드인지 뭔지
                (void**)_mDInput.Get(),         // (출력) DirectInput 객체
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
    }
    else
    {
        InitWinInputDevices();
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

    if (_mSystemType == InputSystemType::DInput)
    {
        // 자체 풀링 업데이트
        UpdateDInputState();
        UpdateAdditionalMouseState();

        // Key 문자 메세지 수신
        ProcessWinKeyMessagesForDInput();
    }
    else
    {
        // XInput 제외 모든 입력 메세지 수신
        ProcessWinKeyMessagesForWinInput();
    }
    // XInput은 풀링 업데이트
    UpdateXInputState();
}

void InputManager::Destroy()
{
    APP_WIN_MANAGER->mOnChangeWindowMode.Unbind(_mChangeWindowModeDelegateHandle);
    APP_WIN_MANAGER->mOnChangeFocus.Unbind(_mChangeFocusDelegateHandle);

    _mWinKeyMessages.clear();
    _mState.reset();
}

void InputManager::PushWinKeyMessage(const WindowMessageData& data)
{
    _mWinKeyMessages.push_back(data);
}

void InputManager::PushRawWinKeyMessage(WindowMessageData data)
{
    const HRAWINPUT hRawInput = reinterpret_cast<HRAWINPUT>(data.mLParam);

    // Raw Input 사이즈 구하기
    uint32 rawDataSize = 0;
    ::GetRawInputData(hRawInput, RID_INPUT, NULL, &rawDataSize, sizeof(RAWINPUTHEADER));

    // Raw Input 확인
    std::unique_ptr<uint8[]> rawData = std::make_unique<uint8[]>(rawDataSize);
    if (::GetRawInputData(hRawInput, RID_INPUT, rawData.get(), &rawDataSize, sizeof(RAWINPUTHEADER)) == rawDataSize)
    {
        const RAWINPUT* const rawDataPtr = reinterpret_cast<const RAWINPUT* const>(rawData.get());

        // 마우스인 경우만 체크
        if (rawDataPtr->header.dwType == RIM_TYPEMOUSE)
        {
            const bool isAbsoluteInput = (rawDataPtr->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE;
            if (isAbsoluteInput == true)
            {
                // TODO
                return;
            }

            // 상대 좌표인 경우는 대입
            data.mMousePos.x = rawDataPtr->data.mouse.lLastX;
            data.mMousePos.y = rawDataPtr->data.mouse.lLastY;
            PushWinKeyMessage(data);
        }
    }
}

void InputManager::ProcessWinKeyMessagesForWinInput()
{
    _mState->mKeyStates[KeyType::Mouse::Pos] = KeyState::None;
    _mState->mScaleValues[KeyType::Mouse::Pos].Set(Vec2::Zero);

    for (auto& keyMessage : _mWinKeyMessages)
    {
        switch (keyMessage.mMsgType)
        {
        case WM_CHAR:
        {
            std::shared_ptr<CharEvent> event = ObjectPool<CharEvent>::MakeShared();
            event->mChar = static_cast<wchar_t>(keyMessage.mWParam);
            event->mIsRepeat = (keyMessage.mLParam & 0x40000000) != 0;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;
            event->mPath = &_mFocusWidgetPath;

            APP->OnPressChar(event);
            break;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            KeyType::Type keyType = ConvertWinInputToKeyType(static_cast<KeyType::WinInputType>(keyMessage.mWParam));
            if (keyType == KeyType::None)
            {
                break;
            }

            if ((keyMessage.mLParam & 0x40000000) != 0)
            {
                _mState->mKeyStates[keyType] = KeyState::Hold;
            }
            else
            {
                _mState->mKeyStates[keyType] = KeyState::Press;
            }

            if (keyType & KeyType::Alt)
            {
                _mState->mUseAlt = true;
            }
            else if (keyType & KeyType::Ctrl)
            {
                _mState->mUseCtrl = true;
            }
            else if (keyType & KeyType::Shift)
            {
                _mState->mUseShift = true;
            }

            std::shared_ptr<KeyEvent> event = ObjectPool<KeyEvent>::MakeShared();
            event->mKeyInfo.mType = keyType;
            event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
            event->mIsRepeat = _mState->mKeyStates[keyType] == KeyState::Hold;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;
            event->mPath = &_mFocusWidgetPath;

            APP->OnPressKey(event);
            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            KeyType::Type keyType = ConvertWinInputToKeyType(static_cast<KeyType::WinInputType>(keyMessage.mWParam));
            if (keyType == KeyType::None)
            {
                break;
            }

            _mState->mKeyStates[keyType] = KeyState::Release;

            if (keyType & KeyType::Alt)
            {
                _mState->mUseAlt = IsActiveButtonState(_mState->mKeyStates[KeyType::LAlt]) || IsActiveButtonState(_mState->mKeyStates[KeyType::RAlt]);
            }
            else if (keyType & KeyType::Ctrl)
            {
                _mState->mUseCtrl = IsActiveButtonState(_mState->mKeyStates[KeyType::LCtrl]) || IsActiveButtonState(_mState->mKeyStates[KeyType::RCtrl]);
            }
            else if (keyType & KeyType::Shift)
            {
                _mState->mUseShift = IsActiveButtonState(_mState->mKeyStates[KeyType::LShift]) || IsActiveButtonState(_mState->mKeyStates[KeyType::RShift]);
            }

            std::shared_ptr<KeyEvent> event = ObjectPool<KeyEvent>::MakeShared();
            event->mKeyInfo.mType = keyType;
            event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
            event->mIsRepeat = false;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;
            event->mPath = &_mFocusWidgetPath;

            APP->OnReleaseKey(event);
            break;
        }
        case WM_LBUTTONDOWN:
        {
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(keyMessage.mHWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::LButton;
                if ((keyMessage.mLParam & 0x40000000) != 0)
                {
                    _mState->mKeyStates[keyType] = KeyState::Hold;
                }
                else
                {
                    _mState->mKeyStates[keyType] = KeyState::Press;
                }

                POINT currentMousePos;
                currentMousePos.x = static_cast<long>(keyMessage.mLParam & 0xffff);
                currentMousePos.y = static_cast<long>((keyMessage.mLParam >> 16) & 0xffff);
                ::ClientToScreen(keyMessage.mHWnd, &currentMousePos);

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mCurrentMouseScreenPos = POINT(currentMousePos.x, currentMousePos.y);
                event->mHWnd = keyMessage.mHWnd;
                event->mIsRepeat = _mState->mKeyStates[keyType] == KeyState::Hold;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnPressMouse(window, event);
            }
            break;
        }
        case WM_MBUTTONDOWN:
        {
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(keyMessage.mHWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::MButton;

                if ((keyMessage.mLParam & 0x40000000) != 0)
                {
                    _mState->mKeyStates[keyType] = KeyState::Hold;
                }
                else
                {
                    _mState->mKeyStates[keyType] = KeyState::Press;
                }

                POINT currentMousePos;
                currentMousePos.x = static_cast<long>(keyMessage.mLParam & 0xffff);
                currentMousePos.y = static_cast<long>((keyMessage.mLParam >> 16) & 0xffff);
                ::ClientToScreen(keyMessage.mHWnd, &currentMousePos);

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mCurrentMouseScreenPos = POINT(currentMousePos.x, currentMousePos.y);
                event->mHWnd = keyMessage.mHWnd;
                event->mIsRepeat = _mState->mKeyStates[keyType] == KeyState::Hold;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnPressMouse(window, event);
            }
            break;
        }
        case WM_RBUTTONDOWN:
        {
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(keyMessage.mHWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::RButton;
                if ((keyMessage.mLParam & 0x40000000) != 0)
                {
                    _mState->mKeyStates[keyType] = KeyState::Hold;
                }
                else
                {
                    _mState->mKeyStates[keyType] = KeyState::Press;
                }

                POINT currentMousePos;
                currentMousePos.x = static_cast<long>(keyMessage.mLParam & 0xffff);
                currentMousePos.y = static_cast<long>((keyMessage.mLParam >> 16) & 0xffff);
                ::ClientToScreen(keyMessage.mHWnd, &currentMousePos);

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mCurrentMouseScreenPos = POINT(currentMousePos.x, currentMousePos.y);
                event->mHWnd = keyMessage.mHWnd;
                event->mIsRepeat = _mState->mKeyStates[keyType] == KeyState::Hold;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnPressMouse(window, event);
            }
            break;
        }
        case WM_LBUTTONUP:
        {
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(keyMessage.mHWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::LButton;
                _mState->mKeyStates[keyType] = KeyState::Release;

                POINT currentMousePos;
                currentMousePos.x = static_cast<long>(keyMessage.mLParam & 0xffff);
                currentMousePos.y = static_cast<long>((keyMessage.mLParam >> 16) & 0xffff);
                ::ClientToScreen(keyMessage.mHWnd, &currentMousePos);

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mCurrentMouseScreenPos = POINT(currentMousePos.x, currentMousePos.y);
                event->mHWnd = keyMessage.mHWnd;
                event->mIsRepeat = false;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnReleaseMouse(event);
            }
            break;
        }
        case WM_MBUTTONUP:
        {
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(keyMessage.mHWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::MButton;
                _mState->mKeyStates[keyType] = KeyState::Release;

                POINT currentMousePos;
                currentMousePos.x = static_cast<long>(keyMessage.mLParam & 0xffff);
                currentMousePos.y = static_cast<long>((keyMessage.mLParam >> 16) & 0xffff);
                ::ClientToScreen(keyMessage.mHWnd, &currentMousePos);

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mCurrentMouseScreenPos = POINT(currentMousePos.x, currentMousePos.y);
                event->mHWnd = keyMessage.mHWnd;
                event->mIsRepeat = false;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnReleaseMouse(event);
            }
            break;
        }
        case WM_RBUTTONUP:
        {
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(keyMessage.mHWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::RButton;
                _mState->mKeyStates[keyType] = KeyState::Release;

                POINT currentMousePos;
                currentMousePos.x = static_cast<long>(keyMessage.mLParam & 0xffff);
                currentMousePos.y = static_cast<long>((keyMessage.mLParam >> 16) & 0xffff);
                ::ClientToScreen(keyMessage.mHWnd, &currentMousePos);

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mCurrentMouseScreenPos = POINT(currentMousePos.x, currentMousePos.y);
                event->mHWnd = keyMessage.mHWnd;
                event->mIsRepeat = false;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnReleaseMouse(event);
            }
            break;
        }
        case WM_LBUTTONDBLCLK:
        {
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(keyMessage.mHWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::LButton;

                POINT currentMousePos;
                currentMousePos.x = static_cast<long>(keyMessage.mLParam & 0xffff);
                currentMousePos.y = static_cast<long>((keyMessage.mLParam >> 16) & 0xffff);
                ::ClientToScreen(keyMessage.mHWnd, &currentMousePos);

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mCurrentMouseScreenPos = POINT(currentMousePos.x, currentMousePos.y);
                event->mHWnd = keyMessage.mHWnd;
                event->mIsRepeat = false;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnDoubleClickMouse(window, event);
            }
            break;
        }
        case WM_MBUTTONDBLCLK:
        {
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(keyMessage.mHWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::MButton;

                POINT currentMousePos;
                currentMousePos.x = static_cast<long>(keyMessage.mLParam & 0xffff);
                currentMousePos.y = static_cast<long>((keyMessage.mLParam >> 16) & 0xffff);
                ::ClientToScreen(keyMessage.mHWnd, &currentMousePos);

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mCurrentMouseScreenPos = POINT(currentMousePos.x, currentMousePos.y);
                event->mHWnd = keyMessage.mHWnd;
                event->mIsRepeat = false;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnDoubleClickMouse(window, event);
            }
            break;
        }
        case WM_RBUTTONDBLCLK:
        {
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(keyMessage.mHWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::RButton;

                POINT currentMousePos;
                currentMousePos.x = static_cast<long>(keyMessage.mLParam & 0xffff);
                currentMousePos.y = static_cast<long>((keyMessage.mLParam >> 16) & 0xffff);
                ::ClientToScreen(keyMessage.mHWnd, &currentMousePos);

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mCurrentMouseScreenPos = POINT(currentMousePos.x, currentMousePos.y);
                event->mHWnd = keyMessage.mHWnd;
                event->mIsRepeat = false;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnDoubleClickMouse(window, event);
            }
            break;
        }

        case WM_NCMOUSEMOVE:
        case WM_MOUSEMOVE:
        {
            _mState->mKeyStates[KeyType::Mouse::Pos] = KeyState::Trigger;

            _mState->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
            ::GetCursorPos(&_mState->mCurrentMouseScreenPos);

            Vec2 offset = Vec2(
                static_cast<float>(_mState->mCurrentMouseScreenPos.x - _mState->mPreMouseScreenPos.x),
                static_cast<float>(_mState->mCurrentMouseScreenPos.y - _mState->mPreMouseScreenPos.y)
            );
            offset.Normalize();
            _mState->mScaleValues[KeyType::Mouse::Pos].Set(offset);

            std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
            event->mKeyInfo.mType = KeyType::Mouse::Pos;
            event->mKeyInfo.mKeyValueType = KeyValueType::Vec2;
            event->mPreMouseScreenPos = _mState->mPreMouseScreenPos;
            event->mCurrentMouseScreenPos = _mState->mCurrentMouseScreenPos;
            event->mHWnd = keyMessage.mHWnd;
            event->mIsRepeat = false;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;

            APP->OnMoveMouse(event);

            break;
        }

        case WM_INPUT:
        {
            _mState->mKeyStates[KeyType::Mouse::Pos] = KeyState::Trigger;

            _mState->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
            _mState->mCurrentMouseScreenPos.x += keyMessage.mMousePos.x;
            _mState->mCurrentMouseScreenPos.y += keyMessage.mMousePos.y;

            Vec2 offset = Vec2(static_cast<float>(keyMessage.mMousePos.x) , static_cast<float>(keyMessage.mMousePos.y));
            offset.Normalize();
            _mState->mScaleValues[KeyType::Mouse::Pos].Set(offset);

            std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
            event->mKeyInfo.mType = KeyType::Mouse::Pos;
            event->mKeyInfo.mKeyValueType = KeyValueType::Vec2;
            event->mPreMouseScreenPos = _mState->mPreMouseScreenPos;
            event->mCurrentMouseScreenPos = _mState->mCurrentMouseScreenPos;
            event->mHWnd = keyMessage.mHWnd;
            event->mIsRepeat = false;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;

            APP->OnMoveMouse(event);

            break;
        }
        }
    }

    _mWinKeyMessages.clear();
}

void InputManager::ProcessWinKeyMessagesForDInput()
{
    for (auto& keyMessage : _mWinKeyMessages)
    {
        if (keyMessage.mMsgType == WM_CHAR)
        {
            std::shared_ptr<CharEvent> event = ObjectPool<CharEvent>::MakeShared();
            event->mChar = static_cast<wchar_t>(keyMessage.mWParam);
            event->mIsRepeat = (keyMessage.mLParam & 0x40000000) != 0;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;
            event->mPath = &_mFocusWidgetPath;

            APP->OnPressChar(event);
            break;
        }
    }

    _mWinKeyMessages.clear();
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

//void InputManager::UpdateWinInputState()
//{
//    //if (::GetKeyboardState(_mWinInputKeyStates.data()) == false)
//    //{
//    //    return;
//    //}
//    //
//    //for (KeyType::Type key = KeyType::MouseStart; key < KeyType::GamepadStart; ++key)
//    //{
//    //    KeyType::WinInputType winKey = ConvertKeyToWinInputType(key);
//    //    if (winKey == 0)
//    //    {
//    //        continue;
//    //    }
//    //
//    //    KeyState::Type& state = _mState->mKeyStates[key];
//    //
//    //    // 키가 눌려 있는 경우
//    //    if ((_mWinInputKeyStates[winKey] & 0x80) != 0x00)
//    //    {
//    //        // 이전 프레임에 키를 누른 상태라면 PRESS
//    //        if (state == KeyState::Press || state == KeyState::Hold)
//    //        {
//    //            state = KeyState::Hold;
//    //        }
//    //        else
//    //        {
//    //            state = KeyState::Press;
//    //        }
//    //    }
//    //    // 키가 떼어져 있는 경우
//    //    else
//    //    {
//    //        // 이전 프레임에 키를 누른 상태라면 UP
//    //        if (state == KeyState::Press || state == KeyState::Hold)
//    //        {
//    //            state = KeyState::Release;
//    //        }
//    //        else
//    //        {
//    //            state = KeyState::None;
//    //        }
//    //    }
//    //}
//    //
//    //POINT mousePos;
//    //::GetCursorPos(&mousePos);
//    //::ScreenToClient(focusWindow->GetDesc().mHWndRef->mData, &mousePos);
//    //
//    //Vec2 offset(static_cast<float>(mousePos.x - _mState->mPreMousePos.x), static_cast<float>(mousePos.y - _mState->mPreMousePos.y));
//    //if (offset.LengthSquared() > std::numeric_limits<float>::epsilon())
//    //{
//    //    _mState->mKeyStates[KeyType::Mouse::Pos] = KeyState::Trigger;
//    //    offset.Normalize();
//    //    _mState->mScaleValues[KeyType::Mouse::Pos].Set(offset);
//    //}
//    //else
//    //{
//    //    _mState->mKeyStates[KeyType::Mouse::Pos] = KeyState::None;
//    //    _mState->mScaleValues[KeyType::Mouse::Pos].Set(Vec2::Zero);
//    //}
//
//    //_mState->mPreMousePos = _mState->mCurrentMousePos;
//    //_mState->mCurrentMousePos.x = mousePos.x;
//    //_mState->mCurrentMousePos.y = mousePos.y;
//}

void InputManager::UpdateDInputState()
{
    /* 마우스 위치 업데이트 */

    POINT mousePos;
    ::GetCursorPos(&mousePos);

    Vec2 offset(static_cast<float>(mousePos.x - _mState->mPreMouseScreenPos.x), static_cast<float>(mousePos.y - _mState->mPreMouseScreenPos.y));
    if (IsNearlyZero(offset.LengthSquared()) == false)
    {
        _mState->mKeyStates[KeyType::Mouse::Pos] = KeyState::Trigger;
        offset.Normalize();
        _mState->mScaleValues[KeyType::Mouse::Pos].Set(offset);
    }
    else
    {
        _mState->mKeyStates[KeyType::Mouse::Pos] = KeyState::None;
        _mState->mScaleValues[KeyType::Mouse::Pos].Set(Vec2::Zero);
    }

    _mState->mPreMouseScreenPos = _mState->mCurrentMouseScreenPos;
    _mState->mCurrentMouseScreenPos.x = mousePos.x;
    _mState->mCurrentMouseScreenPos.y = mousePos.y;

    /* 키보드 입력 업데이트 */

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
            _mState->mUseAlt = 
                (_mDInputKeyStates[ConvertKeyToDInputType(KeyType::LAlt)] & 0x80) != 0x00 &&
                (_mDInputKeyStates[ConvertKeyToDInputType(KeyType::RAlt)] & 0x80) != 0x00;
            _mState->mUseCtrl =
                (_mDInputKeyStates[ConvertKeyToDInputType(KeyType::LCtrl)] & 0x80) != 0x00 &&
                (_mDInputKeyStates[ConvertKeyToDInputType(KeyType::RCtrl)] & 0x80) != 0x00;
            _mState->mUseShift =
                (_mDInputKeyStates[ConvertKeyToDInputType(KeyType::LShift)] & 0x80) != 0x00 &&
                (_mDInputKeyStates[ConvertKeyToDInputType(KeyType::RShift)] & 0x80) != 0x00;

            for (KeyType::Type key = KeyType::KeyBoardStart; key < KeyType::GamepadStart; ++key)
            {
                KeyType::DInputType dInputKey = ConvertKeyToDInputType(key);
                if (dInputKey == 0)
                {
                    continue;
                }

                KeyState::Type& state = _mState->mKeyStates[key];

                // 키가 눌려 있는 경우
                if ((_mDInputKeyStates[dInputKey] & 0x80) != 0x00)
                {
                    // 이전 프레임에 키를 누른 상태라면 PRESS
                    if (IsActiveButtonState(state) == true)
                    {
                        state = KeyState::Hold;
                    }
                    else
                    {
                        state = KeyState::Press;
                    }

                    std::shared_ptr<KeyEvent> event = ObjectPool<KeyEvent>::MakeShared();
                    event->mKeyInfo.mType = key;
                    event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                    event->mIsRepeat = state == KeyState::Hold;
                    event->mUseAlt = _mState->mUseAlt;
                    event->mUseCtrl = _mState->mUseCtrl;
                    event->mUseShift = _mState->mUseShift;
                    event->mPath = &_mFocusWidgetPath;

                    APP->OnPressKey(event);
                }
                // 키가 떼어져 있는 경우
                else
                {
                    // 이전 프레임에 키를 누른 상태라면 UP
                    if (IsActiveButtonState(state) == true)
                    {
                        state = KeyState::Release;

                        std::shared_ptr<KeyEvent> event = ObjectPool<KeyEvent>::MakeShared();
                        event->mKeyInfo.mType = key;
                        event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                        event->mIsRepeat = false;
                        event->mUseAlt = _mState->mUseAlt;
                        event->mUseCtrl = _mState->mUseCtrl;
                        event->mUseShift = _mState->mUseShift;
                        event->mPath = &_mFocusWidgetPath;

                        APP->OnReleaseKey(event);
                    }
                }
            }
        }
    }

    /* 마우스 입력 업데이트 */

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
                KeyState::Type& state = _mState->mKeyStates[key];

                // 키가 눌려 있는 경우
                if ((_mDInputMouseState.rgbButtons[key - KeyType::Mouse::LButton] & 0x80) != 0x00)
                {
                    // 이전 프레임에 키를 누른 상태라면 PRESS
                    if (IsActiveButtonState(state) == true)
                    {
                        state = KeyState::Hold;
                    }
                    else
                    {
                        state = KeyState::Press;
                    }

                    HWND hWnd = ::WindowFromPoint(_mState->mCurrentMouseScreenPos);
                    std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(hWnd);
                    if (window != nullptr && window->IsValid() == true)
                    {
                        std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                        event->mKeyInfo.mType = KeyType::Mouse::LButton;
                        event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                        event->mPreMouseScreenPos = _mState->mPreMouseScreenPos;
                        event->mCurrentMouseScreenPos = _mState->mCurrentMouseScreenPos;
                        event->mHWnd = hWnd;
                        event->mIsRepeat = _mState->mKeyStates[KeyType::Mouse::LButton] == KeyState::Hold;
                        event->mUseAlt = _mState->mUseAlt;
                        event->mUseCtrl = _mState->mUseCtrl;
                        event->mUseShift = _mState->mUseShift;

                        APP->OnPressMouse(window, event);
                    }
                }
                // 키가 떼어져 있는 경우
                else
                {
                    // 이전 프레임에 키를 누른 상태라면 UP
                    if (IsActiveButtonState(state) == true)
                    {
                        state = KeyState::Release;

                        std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                        event->mKeyInfo.mType = KeyType::Mouse::LButton;
                        event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                        event->mPreMouseScreenPos = _mState->mPreMouseScreenPos;
                        event->mCurrentMouseScreenPos = _mState->mCurrentMouseScreenPos;
                        event->mIsRepeat = false;
                        event->mUseAlt = _mState->mUseAlt;
                        event->mUseCtrl = _mState->mUseCtrl;
                        event->mUseShift = _mState->mUseShift;

                        APP->OnReleaseMouse(event);
                    }
                }
            }
        }
    }

    // 마우스 위치 이벤트 처리
    {
        std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
        event->mKeyInfo.mType = KeyType::Mouse::Pos;
        event->mKeyInfo.mKeyValueType = KeyValueType::Vec2;
        event->mPreMouseScreenPos = _mState->mPreMouseScreenPos;
        event->mCurrentMouseScreenPos = _mState->mCurrentMouseScreenPos;
        event->mIsRepeat = false;
        event->mUseAlt = _mState->mUseAlt;
        event->mUseCtrl = _mState->mUseCtrl;
        event->mUseShift = _mState->mUseShift;

        APP->OnMoveMouse(event);
    }
}

void InputManager::UpdateXInputState()
{
    ZeroMemory(&_mXInputState, sizeof(_mXInputState));

    DWORD result = XInputGetState(0, &_mXInputState);
    if (result != ERROR_SUCCESS || _mXInputState.dwPacketNumber == 0)
    {
        return;
    }

    for (KeyType::Type keyType = KeyType::GamepadStart; keyType <= KeyType::Last; ++keyType)
    {
        KeyType::XInputType xInputKey = ConvertKeyToXInputType(keyType);
        if (xInputKey == 0)
        {
            continue;
        }

        KeyState::Type& state = _mState->mKeyStates[keyType];
        if ((_mXInputState.Gamepad.wButtons & xInputKey) != 0x00000000)
        {
            // 이전 프레임에 키를 누른 상태라면 PRESS
            if (IsActiveButtonState(state) == true)
            {
                state = KeyState::Hold;
            }
            else
            {
                state = KeyState::Press;
            }

            std::shared_ptr<KeyEvent> event = ObjectPool<KeyEvent>::MakeShared();
            event->mKeyInfo.mType = keyType;
            event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
            event->mIsRepeat = state == KeyState::Hold;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;
            event->mPath = &_mFocusWidgetPath;

            APP->OnPressKey(event);
        }
        // 키가 떼어져 있는 경우
        else
        {
            // 이전 프레임에 키를 누른 상태라면 UP
            if (IsActiveButtonState(state) == true)
            {
                state = KeyState::Release;

                std::shared_ptr<KeyEvent> event = ObjectPool<KeyEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mIsRepeat = false;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;
                event->mPath = &_mFocusWidgetPath;

                APP->OnReleaseKey(event);
            }
        }
    }

    //for (KeyType::Type key = KeyType::GamepadStart; key <= KeyType::Last; ++key)
    //{
    //    KeyType::XInputType xInputKey = ConvertKeyToXInputType(key);
    //    if (xInputKey == 0)
    //    {
    //        continue;
    //    }
    //
    //    KeyState::Type& state = _mState->mKeyStates[key];
    //
    //    if ((_mXInputState.Gamepad.wButtons & xInputKey) != 0x00000000)
    //    {
    //        // 이전 프레임에 키를 누른 상태라면 PRESS
    //        if (state == KeyState::Press || state == KeyState::Hold)
    //        {
    //            state = KeyState::Hold;
    //        }
    //        else
    //        {
    //            state = KeyState::Press;
    //        }
    //    }
    //    // 키가 떼어져 있는 경우
    //    else
    //    {
    //        // 이전 프레임에 키를 누른 상태라면 UP
    //        if (state == KeyState::Press || state == KeyState::Hold)
    //        {
    //            state = KeyState::Release;
    //        }
    //        else
    //        {
    //            state = KeyState::None;
    //        }
    //    }
    //}

    {
        const float ratio = static_cast<float>(_mXInputState.Gamepad.bLeftTrigger) / UCHAR_MAX;
        _mState->mKeyStates[KeyType::Gamepad::LTrigger] = _mXInputState.Gamepad.bLeftTrigger != 0 ? KeyState::Trigger : KeyState::None;
        
        const float preVal = _mState->mScaleValues[KeyType::Gamepad::LTrigger].Get<float>();
        _mState->mScaleValues[KeyType::Gamepad::LTrigger].Set(ratio);

        if (IsNearlyZero(preVal - ratio) == false)
        {
            std::shared_ptr<AnalogInputEvent> event = ObjectPool<AnalogInputEvent>::MakeShared();
            event->mKeyInfo.mType = KeyType::Gamepad::LTrigger;
            event->mKeyInfo.mKeyValueType = KeyValueType::Float;
            event->mScaleValue = ratio;
            event->mIsRepeat = false;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;
            event->mPath = &_mFocusWidgetPath;

            APP->OnChangeAnalogValue(event);
        }
    }
    {
        const float ratio = static_cast<float>(_mXInputState.Gamepad.bRightTrigger) / UCHAR_MAX;
        _mState->mKeyStates[KeyType::Gamepad::RTrigger] = _mXInputState.Gamepad.bRightTrigger != 0 ? KeyState::Trigger : KeyState::None;

        const float preVal = _mState->mScaleValues[KeyType::Gamepad::RTrigger].Get<float>();
        _mState->mScaleValues[KeyType::Gamepad::RTrigger].Set(ratio);

        if (IsNearlyZero(preVal - ratio) == false)
        {
            std::shared_ptr<AnalogInputEvent> event = ObjectPool<AnalogInputEvent>::MakeShared();
            event->mKeyInfo.mType = KeyType::Gamepad::RTrigger;
            event->mKeyInfo.mKeyValueType = KeyValueType::Float;
            event->mScaleValue = ratio;
            event->mIsRepeat = false;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;
            event->mPath = &_mFocusWidgetPath;

            APP->OnChangeAnalogValue(event);
        }
    }

    {
        Vec2 axis;
        axis.x = static_cast<float>(_mXInputState.Gamepad.sThumbLX) / SHORT_MAX;
        axis.y = static_cast<float>(_mXInputState.Gamepad.sThumbLY) / SHORT_MAX;
        _mState->mKeyStates[KeyType::Gamepad::LStick] = axis.LengthSquared() > std::numeric_limits<float>::epsilon() ? KeyState::Trigger : KeyState::None;
        axis.Normalize();

        const Vec2 preVal = _mState->mScaleValues[KeyType::Gamepad::LStick].Get<Vec2>();
        _mState->mScaleValues[KeyType::Gamepad::LStick].Set(axis);

        if (IsNearlyZero(preVal - axis) == false)
        {
            std::shared_ptr<AnalogInputEvent> event = ObjectPool<AnalogInputEvent>::MakeShared();
            event->mKeyInfo.mType = KeyType::Gamepad::LStick;
            event->mKeyInfo.mKeyValueType = KeyValueType::Vec2;
            event->mScaleValue = axis;
            event->mIsRepeat = false;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;
            event->mPath = &_mFocusWidgetPath;

            APP->OnChangeAnalogValue(event);
        }
    }
    {
        Vec2 axis;
        axis.x = static_cast<float>(_mXInputState.Gamepad.sThumbRX) / SHORT_MAX;
        axis.y = static_cast<float>(_mXInputState.Gamepad.sThumbRY) / SHORT_MAX;
        _mState->mKeyStates[KeyType::Gamepad::RStick] = axis.LengthSquared() > std::numeric_limits<float>::epsilon() ? KeyState::Trigger : KeyState::None;
        axis.Normalize();

        const Vec2 preVal = _mState->mScaleValues[KeyType::Gamepad::RStick].Get<Vec2>();
        _mState->mScaleValues[KeyType::Gamepad::RStick].Set(axis);

        if (IsNearlyZero(preVal - axis) == false)
        {
            std::shared_ptr<AnalogInputEvent> event = ObjectPool<AnalogInputEvent>::MakeShared();
            event->mKeyInfo.mType = KeyType::Gamepad::RStick;
            event->mKeyInfo.mKeyValueType = KeyValueType::Vec2;
            event->mScaleValue = axis;
            event->mIsRepeat = false;
            event->mUseAlt = _mState->mUseAlt;
            event->mUseCtrl = _mState->mUseCtrl;
            event->mUseShift = _mState->mUseShift;
            event->mPath = &_mFocusWidgetPath;

            APP->OnChangeAnalogValue(event);
        }
    }
}

void InputManager::UpdateAdditionalMouseState()
{
    /* 더블 클릭 */

    double curTime = TIME_MANAGER->GetPlayTime();
    if (_mState->mKeyStates[KeyType::Mouse::LButton] == KeyState::Release)
    {
        if ((curTime - _mLClickPreReleaseTime) < _mDoubleClickThreshold)
        {
            HWND hWnd = ::WindowFromPoint(_mState->mCurrentMouseScreenPos);
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(hWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::LButton;

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mPreMouseScreenPos;
                event->mCurrentMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mHWnd = hWnd;
                event->mIsRepeat = false;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnDoubleClickMouse(window, event);
            }
        }
        _mLClickPreReleaseTime = curTime;
    }
    if (_mState->mKeyStates[KeyType::Mouse::RButton] == KeyState::Release)
    {
        if ((curTime - _mRClickPreReleaseTime) < _mDoubleClickThreshold)
        {
            HWND hWnd = ::WindowFromPoint(_mState->mCurrentMouseScreenPos);
            std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(hWnd);
            if (window != nullptr && window->IsValid() == true)
            {
                KeyType::Type keyType = KeyType::Mouse::RButton;

                std::shared_ptr<PointEvent> event = ObjectPool<PointEvent>::MakeShared();
                event->mKeyInfo.mType = keyType;
                event->mKeyInfo.mKeyValueType = KeyValueType::Bool;
                event->mPreMouseScreenPos = _mState->mPreMouseScreenPos;
                event->mCurrentMouseScreenPos = _mState->mCurrentMouseScreenPos;
                event->mHWnd = hWnd;
                event->mIsRepeat = false;
                event->mUseAlt = _mState->mUseAlt;
                event->mUseCtrl = _mState->mUseCtrl;
                event->mUseShift = _mState->mUseShift;

                APP->OnDoubleClickMouse(window, event);
            }
        }
        _mRClickPreReleaseTime = curTime;
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
                window->GetDesc().mHWndRef->mData,      // 윈도우 핸들
                DISCL_NONEXCLUSIVE | DISCL_FOREGROUND   // 타 윈도우와 입력 공유 방식 설정
            );

            _mDInputMouse->SetCooperativeLevel(
                window->GetDesc().mHWndRef->mData,      // 윈도우 핸들
                DISCL_NONEXCLUSIVE | DISCL_FOREGROUND   // 타 윈도우와 입력 공유 방식 설정
            );
        }
        else
        {
            // 전체화면 모드

            _mDInputKeyboard->SetCooperativeLevel(
                window->GetDesc().mHWndRef->mData,      // 윈도우 핸들
                DISCL_EXCLUSIVE | DISCL_FOREGROUND      // 입력 독점 방식 설정
            );

            _mDInputMouse->SetCooperativeLevel(
                window->GetDesc().mHWndRef->mData,      // 윈도우 핸들
                DISCL_EXCLUSIVE | DISCL_FOREGROUND      // 타 윈도우와 입력 공유 방식 설정
            );
        }
    }
}

