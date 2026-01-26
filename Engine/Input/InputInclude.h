#pragma once

#include "Types.h"
#include <limits.h>
#include <Xinput.h>
#include <dinput.h>

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
		KeyBoardStart = 0x05,
		GamepadStart = 0x5F,
		Last = 0x70,
	};

	enum Mouse : uint8
	{
		LButton = MouseStart,
		RButton,
		MButton,
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
}

