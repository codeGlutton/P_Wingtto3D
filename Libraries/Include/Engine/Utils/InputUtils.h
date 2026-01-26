#pragma once

#include "Input/InputInclude.h"
#include "Input/InputBinding.h"

KeyType::Type ConvertWinInputToKeyType(KeyType::WinInputType winKey);
KeyType::Type ConvertDInputToKeyType(KeyType::DInputType dInputKey);
KeyType::Type ConvertXInputToKeyType(KeyType::XInputType xInputKey);
KeyType::WinInputType ConvertKeyToWinInputType(KeyType::Type key);
KeyType::DInputType ConvertKeyToDInputType(KeyType::Type key);
KeyType::XInputType ConvertKeyToXInputType(KeyType::Type key);

bool IsActiveButtonState(KeyState::Type type);