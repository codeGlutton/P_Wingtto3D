#include "pch.h"
#include "InputUtils.h"

namespace KeyMapStatic
{
	using WinInputKeyPair = std::pair<KeyType::Type, KeyType::WinInputType>;
	using DInputKeyPair = std::pair<KeyType::Type, KeyType::DInputType>;
	using XInputKeyPair = std::pair<KeyType::Type, KeyType::XInputType>;

	constexpr WinInputKeyPair winInputKeyMap[] = {
		/* 마우스 */
		{ KeyType::LButton,		VK_LBUTTON },
		{ KeyType::RButton,		VK_RBUTTON },
		{ KeyType::MButton,		VK_MBUTTON },

		/* 키보드 */
		{ KeyType::Tab,			VK_TAB },
		{ KeyType::Enter,		VK_RETURN },
		{ KeyType::Space,		VK_SPACE },
		{ KeyType::Pause,		VK_PAUSE },
		{ KeyType::Esc,			VK_ESCAPE },
		{ KeyType::Backspace,	VK_BACK },

		{ KeyType::Left,		VK_LEFT },
		{ KeyType::Up,			VK_UP },
		{ KeyType::Right,		VK_RIGHT },
		{ KeyType::Down,		VK_DOWN },

		{ KeyType::LShift,		VK_LSHIFT },
		{ KeyType::RShift,		VK_RSHIFT },
		{ KeyType::LCtrl,		VK_LCONTROL },
		{ KeyType::RCtrl,		VK_RCONTROL },
		{ KeyType::LAlt,		VK_LMENU },
		{ KeyType::RAlt,		VK_RMENU },

		{ KeyType::A, 'A' }, { KeyType::B, 'B' }, { KeyType::C, 'C' },
		{ KeyType::D, 'D' }, { KeyType::E, 'E' }, { KeyType::F, 'F' },
		{ KeyType::G, 'G' }, { KeyType::H, 'H' }, { KeyType::I, 'I' },
		{ KeyType::J, 'J' }, { KeyType::K, 'K' }, { KeyType::L, 'L' },
		{ KeyType::M, 'M' }, { KeyType::N, 'N' }, { KeyType::O, 'O' },
		{ KeyType::P, 'P' }, { KeyType::Q, 'Q' }, { KeyType::R, 'R' },
		{ KeyType::S, 'S' }, { KeyType::T, 'T' }, { KeyType::U, 'U' },
		{ KeyType::V, 'V' }, { KeyType::W, 'W' }, { KeyType::X, 'X' },
		{ KeyType::Y, 'Y' }, { KeyType::Z, 'Z' },

		{ KeyType::Num0, '0' }, { KeyType::Num1, '1' },
		{ KeyType::Num2, '2' }, { KeyType::Num3, '3' },
		{ KeyType::Num4, '4' }, { KeyType::Num5, '5' },
		{ KeyType::Num6, '6' }, { KeyType::Num7, '7' },
		{ KeyType::Num8, '8' }, { KeyType::Num9, '9' },

		{ KeyType::Numpad0,		VK_NUMPAD0 },
		{ KeyType::Numpad1,		VK_NUMPAD1 },
		{ KeyType::Numpad2,		VK_NUMPAD2 },
		{ KeyType::Numpad3,		VK_NUMPAD3 },
		{ KeyType::Numpad4,		VK_NUMPAD4 },
		{ KeyType::Numpad5,		VK_NUMPAD5 },
		{ KeyType::Numpad6,		VK_NUMPAD6 },
		{ KeyType::Numpad7,		VK_NUMPAD7 },
		{ KeyType::Numpad8,		VK_NUMPAD8 },
		{ KeyType::Numpad9,		VK_NUMPAD9 },

		{ KeyType::F1,  VK_F1  }, { KeyType::F2,  VK_F2  },
		{ KeyType::F3,  VK_F3  }, { KeyType::F4,  VK_F4  },
		{ KeyType::F5,  VK_F5  }, { KeyType::F6,  VK_F6  },
		{ KeyType::F7,  VK_F7  }, { KeyType::F8,  VK_F8  },
		{ KeyType::F9,  VK_F9  }, { KeyType::F10, VK_F10 },
		{ KeyType::F11, VK_F11 }, { KeyType::F12, VK_F12 },

		{ KeyType::Add,			VK_ADD },
		{ KeyType::Subtract,	VK_SUBTRACT },
		{ KeyType::Multiply,	VK_MULTIPLY },
		{ KeyType::Divide,		VK_DIVIDE },
		{ KeyType::Decimal,		VK_DECIMAL },

		{ KeyType::Grave,		VK_OEM_3 },
		{ KeyType::Minus,		VK_OEM_MINUS },
		{ KeyType::Equal,		VK_OEM_PLUS },
		{ KeyType::LBracket,	VK_OEM_4 },
		{ KeyType::RBracket,	VK_OEM_6 },
		{ KeyType::Backslash,	VK_OEM_5 },
		{ KeyType::Semicolon,	VK_OEM_1 },
		{ KeyType::Apostrophe,	VK_OEM_7 },
		{ KeyType::Comma,		VK_OEM_COMMA },
		{ KeyType::Period,		VK_OEM_PERIOD },
		{ KeyType::Slash,		VK_OEM_2 },
	};
	constexpr DInputKeyPair dInputKeyMap[] = {
		/* 키보드 */
		{ KeyType::Tab,			DIK_TAB },
		{ KeyType::Enter,		DIK_RETURN },
		{ KeyType::Space,		DIK_SPACE },
		{ KeyType::Pause,		DIK_PAUSE },
		{ KeyType::Esc,			DIK_ESCAPE },
		{ KeyType::Backspace,	DIK_BACK },

		{ KeyType::Left,		DIK_LEFT },
		{ KeyType::Up,			DIK_UP },
		{ KeyType::Right,		DIK_RIGHT },
		{ KeyType::Down,		DIK_DOWN },

		{ KeyType::LShift,		DIK_LSHIFT },
		{ KeyType::RShift,		DIK_RSHIFT },
		{ KeyType::LCtrl,		DIK_LCONTROL },
		{ KeyType::RCtrl,		DIK_RCONTROL },
		{ KeyType::LAlt,		DIK_LMENU },
		{ KeyType::RAlt,		DIK_RMENU },

		{ KeyType::A, DIK_A }, { KeyType::B, DIK_B }, { KeyType::C, DIK_C },
		{ KeyType::D, DIK_D }, { KeyType::E, DIK_E }, { KeyType::F, DIK_F },
		{ KeyType::G, DIK_G }, { KeyType::H, DIK_H }, { KeyType::I, DIK_I },
		{ KeyType::J, DIK_J }, { KeyType::K, DIK_K }, { KeyType::L, DIK_L },
		{ KeyType::M, DIK_M }, { KeyType::N, DIK_N }, { KeyType::O, DIK_O },
		{ KeyType::P, DIK_P }, { KeyType::Q, DIK_Q }, { KeyType::R, DIK_R },
		{ KeyType::S, DIK_S }, { KeyType::T, DIK_T }, { KeyType::U, DIK_U },
		{ KeyType::V, DIK_V }, { KeyType::W, DIK_W }, { KeyType::X, DIK_X },
		{ KeyType::Y, DIK_Y }, { KeyType::Z, DIK_Z },

		{ KeyType::Num1,		DIK_1 },
		{ KeyType::Num2,		DIK_2 },
		{ KeyType::Num3,		DIK_3 },
		{ KeyType::Num4,		DIK_4 },
		{ KeyType::Num5,		DIK_5 },
		{ KeyType::Num6,		DIK_6 },
		{ KeyType::Num7,		DIK_7 },
		{ KeyType::Num8,		DIK_8 },
		{ KeyType::Num9,		DIK_9 },
		{ KeyType::Num0,		DIK_0 },

		{ KeyType::Numpad0,		DIK_NUMPAD0 },
		{ KeyType::Numpad1,		DIK_NUMPAD1 },
		{ KeyType::Numpad2,		DIK_NUMPAD2 },
		{ KeyType::Numpad3,		DIK_NUMPAD3 },
		{ KeyType::Numpad4,		DIK_NUMPAD4 },
		{ KeyType::Numpad5,		DIK_NUMPAD5 },
		{ KeyType::Numpad6,		DIK_NUMPAD6 },
		{ KeyType::Numpad7,		DIK_NUMPAD7 },
		{ KeyType::Numpad8,		DIK_NUMPAD8 },
		{ KeyType::Numpad9,		DIK_NUMPAD9 },

		{ KeyType::F1,  DIK_F1  }, { KeyType::F2,  DIK_F2  },
		{ KeyType::F3,  DIK_F3  }, { KeyType::F4,  DIK_F4  },
		{ KeyType::F5,  DIK_F5  }, { KeyType::F6,  DIK_F6  },
		{ KeyType::F7,  DIK_F7  }, { KeyType::F8,  DIK_F8  },
		{ KeyType::F9,  DIK_F9  }, { KeyType::F10, DIK_F10 },
		{ KeyType::F11, DIK_F11 }, { KeyType::F12, DIK_F12 },

		{ KeyType::Add,			DIK_ADD },
		{ KeyType::Subtract,	DIK_SUBTRACT },
		{ KeyType::Multiply,	DIK_MULTIPLY },
		{ KeyType::Divide,		DIK_DIVIDE },
		{ KeyType::Decimal,		DIK_DECIMAL },

		{ KeyType::Grave,		DIK_GRAVE },
		{ KeyType::Minus,		DIK_MINUS },
		{ KeyType::Equal,		DIK_EQUALS },
		{ KeyType::LBracket,	DIK_LBRACKET },
		{ KeyType::RBracket,	DIK_RBRACKET },
		{ KeyType::Backslash,	DIK_BACKSLASH },
		{ KeyType::Semicolon,	DIK_SEMICOLON },
		{ KeyType::Apostrophe,	DIK_APOSTROPHE },
		{ KeyType::Comma,		DIK_COMMA },
		{ KeyType::Period,		DIK_PERIOD },
		{ KeyType::Slash,		DIK_SLASH },
	};
	constexpr XInputKeyPair xInputKeyMap[] = {
		{ KeyType::DpadUp,			XINPUT_GAMEPAD_DPAD_UP },
		{ KeyType::DpadDown,		XINPUT_GAMEPAD_DPAD_DOWN },
		{ KeyType::DpadLeft,		XINPUT_GAMEPAD_DPAD_LEFT },
		{ KeyType::DpadRight,		XINPUT_GAMEPAD_DPAD_RIGHT },

		//{ KeyType::LStick,		2D벡터 값 },
		//{ KeyType::RStick,		2D벡터 값 },
		{ KeyType::LThumb,			XINPUT_GAMEPAD_LEFT_THUMB },
		{ KeyType::RThumb,			XINPUT_GAMEPAD_RIGHT_THUMB },

		{ KeyType::GpadStart,		XINPUT_GAMEPAD_START },
		{ KeyType::GpadBack,		XINPUT_GAMEPAD_BACK },

		//{ KeyType::LTrigger,		수치 값 },
		//{ KeyType::RTrigger,		수치 값 },
		{ KeyType::LBumper,			XINPUT_GAMEPAD_LEFT_SHOULDER },
		{ KeyType::RBumper,			XINPUT_GAMEPAD_RIGHT_SHOULDER },

		{ KeyType::GpadA,			XINPUT_GAMEPAD_A },
		{ KeyType::GpadB,			XINPUT_GAMEPAD_B },
		{ KeyType::GpadX,			XINPUT_GAMEPAD_X },
		{ KeyType::GpadY,			XINPUT_GAMEPAD_Y },
	};

	constexpr std::array<KeyType::WinInputType, KEY_COUNT> BuildKeyToWinInputKeyCache()
	{
		std::array<KeyType::WinInputType, KEY_COUNT> cache{};
		for (auto& pair : winInputKeyMap)
		{
			cache[pair.first] = pair.second;
		}
		return cache;
	}
	constexpr std::array<KeyType::DInputType, KEY_COUNT> BuildKeyToDInputKeyCache()
	{
		std::array<KeyType::DInputType, KEY_COUNT> cache{};
		for (auto& pair : dInputKeyMap)
		{
			cache[pair.first] = pair.second;
		}
		return cache;
	}
	constexpr std::array<KeyType::XInputType, KEY_COUNT> BuildKeyToXInputKeyCache()
	{
		std::array<KeyType::XInputType, KEY_COUNT> cache{};
		for (auto& pair : xInputKeyMap)
		{
			cache[pair.first] = pair.second;
		}
		return cache;
	}
	constexpr std::array<KeyType::Type, WIN_INPUT_COUNT> BuildWinInputKeyToKeyCache()
	{
		std::array<KeyType::Type, WIN_INPUT_COUNT> cache{};
		for (auto& pair : winInputKeyMap)
		{
			cache[pair.second] = pair.first;
		}
		return cache;
	}
	constexpr std::array<KeyType::Type, DINPUT_COUNT> BuildDInputKeyToKeyCache()
	{
		std::array<KeyType::Type, DINPUT_COUNT> cache{};
		for (auto& pair : dInputKeyMap)
		{
			cache[pair.second] = pair.first;
		}
		return cache;
	}
	constexpr std::array<KeyType::Type, XINPUT_COUNT> BuildXInputKeyToKeyCache()
	{
		std::array<KeyType::Type, XINPUT_COUNT> cache{};
		for (auto& pair : xInputKeyMap)
		{
			cache[pair.second] = pair.first;
		}
		return cache;
	}
}

KeyType::Type ConvertWinInputToKeyType(KeyType::WinInputType winKey)
{
	static auto cache = KeyMapStatic::BuildWinInputKeyToKeyCache();
	return cache[winKey];
}

KeyType::Type ConvertDInputToKeyType(KeyType::DInputType dInputKey)
{
	static auto cache = KeyMapStatic::BuildDInputKeyToKeyCache();
	return cache[dInputKey];
}

KeyType::Type ConvertXInputToKeyType(KeyType::XInputType xInputKey)
{
	static auto cache = KeyMapStatic::BuildXInputKeyToKeyCache();
	return cache[xInputKey];
}

KeyType::WinInputType ConvertKeyToWinInputType(KeyType::Type key)
{
	static auto cache = KeyMapStatic::BuildKeyToWinInputKeyCache();
	return cache[key];
}

KeyType::DInputType ConvertKeyToDInputType(KeyType::Type key)
{
	static auto cache = KeyMapStatic::BuildKeyToDInputKeyCache();
	return cache[key];
}

KeyType::XInputType ConvertKeyToXInputType(KeyType::Type key)
{
	static auto cache = KeyMapStatic::BuildKeyToXInputKeyCache();
	return cache[key];
}

bool IsActiveButtonState(KeyState::Type type)
{
	if (type == KeyState::Press || type == KeyState::Hold)
	{
		return true;
	}
	return false;
}
