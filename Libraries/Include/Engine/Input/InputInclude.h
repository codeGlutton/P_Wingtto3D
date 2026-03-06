#pragma once

#include "Types.h"
#include <limits.h>
#include <Xinput.h>
#include <dinput.h>
#include <unordered_set>
#include <unordered_map>

#define WIN_INPUT_COUNT (UCHAR_MAX + 1)
#define DINPUT_COUNT (UCHAR_MAX + 1)
#define XINPUT_COUNT (UINT_MAX + 1)

#define KEY_COUNT (UCHAR_MAX + 1)

namespace KeyType
{
	using Type = uint8;
	using WinInputType = uint8;
	using DInputType = uint8;
	using XInputType = uint32;

	enum : uint8
	{
		None = 0x00,
		Start = 0x01,
		MouseStart = 0x01,
		KeyBoardStart = 0x06,
		GamepadStart = 0x60,
		Last = 0x71,
	};

	enum Mouse : uint8
	{
		LButton = MouseStart,
		RButton,
		MButton,
		Wheel,
		Pos,
	};

	enum KeyBoard : uint8
	{
		Tab = KeyBoardStart,
		Enter,
		Space,
		Pause,
		Esc,
		Backspace,

		Left,
		Right,
		Up,
		Down,

		LShift, RShift,
		LCtrl, RCtrl,
		LAlt, RAlt,

		A, B, C, D, E, F, G,
		H, I, J, K, L, M, N,
		O, P, Q, R, S, T, U,
		V, W, X, Y, Z,

		Num0, Num1, Num2, Num3, Num4,
		Num5, Num6, Num7, Num8, Num9,

		Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
		Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,

		F1, F2, F3, F4, F5, F6,
		F7, F8, F9, F10, F11, F12,

		Add,
		Subtract,
		Multiply,
		Divide,
		Decimal,

		Grave,		// `~
		Minus,		// -_
		Equal,		// =+
		LBracket,	// [{
		RBracket,	// ]}
		Backslash,	// \|
		Semicolon,	// ;:
		Apostrophe,	// '"
		Comma,		// ,<
		Period,		// .>
		Slash,		// /?
	};

	enum Gamepad : uint8
	{
		DpadUp = GamepadStart,
		DpadDown,
		DpadLeft,
		DpadRight,

		LStick, RStick,
		LThumb, RThumb,

		GpadStart,
		GpadBack,

		LTrigger, RTrigger,
		LBumper, RBumper,

		GpadA,
		GpadB,
		GpadX,
		GpadY,
	};

	inline bool IsAlt(KeyType::Type key)
	{
		return key == KeyBoard::LAlt || key == KeyBoard::RAlt;
	}
	inline bool IsCtrl(KeyType::Type key)
	{
		return key == KeyBoard::LCtrl || key == KeyBoard::RCtrl;
	}
	inline bool IsShift(KeyType::Type key)
	{
		return key == KeyBoard::LShift || key == KeyBoard::RShift;
	}
}

namespace KeyState
{
	enum Type : uint8
	{
		None = 0xFF,
		Press = 0,
		Hold,
		Release,
		Trigger,
		Count
	};
}

enum class KeyValueType
{
	Bool,
	Float,
	Vec2,
};

struct KeyInfo
{
	KeyType::Type mType;
	KeyValueType mKeyValueType;

	bool operator==(const KeyInfo& other) const {
		return mType == other.mType;
	}
};

namespace std {
	template<>
	struct hash<KeyInfo> {
		size_t operator()(const KeyInfo& member) const noexcept {
			return std::hash<uint8>{}(static_cast<uint8>(member.mType));
		}
	};
}
