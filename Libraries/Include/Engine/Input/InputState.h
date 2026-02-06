#pragma once

#include "Input/InputInclude.h"
#include "Input/InputValue.h"
#include "Input/InputEvent.h"

struct InputState
{
	std::array<KeyState::Type, KEY_COUNT> mKeyStates = {};
	std::unordered_map<KeyType::Type, InputValue> mScaleValues;

	POINT mPreMouseScreenPos;
	POINT mCurrentMouseScreenPos;

	bool mUseCtrl = false;
	bool mUseAlt = false;
	bool mUseShift = false;
};

/* 인 게임 키 구조체 */

struct InputEventLog
{
	std::shared_ptr<InputEvent> mEvent;
	KeyState::Type mKeyState;
};

struct GameInputState
{
public:
	GameInputState()
	{
		mKeyDownLogs.reserve(UINT8_MAX);
	}

public:
	// 저수준 입력 처리 로그들
	std::vector<InputEventLog> mKeyEventLogs;
	// 이전 틱에서 남겨져 있던 입력 (Delta 계산해주면서 Hold 이벤트 호출용)
	std::unordered_map<KeyType::Type, std::shared_ptr<InputEvent>> mKeyDownLogs;
	// 이전 틱에 남은 델타 타임
	float mAccInputTime = 0.f;
};

