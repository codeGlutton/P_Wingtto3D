#pragma once

#include "Input/InputAction.h"

class PickingEditorAction : public InputAction
{
	GEN_REFLECTION(PickingEditorAction)

public:
	PickingEditorAction();
};

class OptionEditorAction : public InputAction
{
	GEN_REFLECTION(OptionEditorAction)

public:
	OptionEditorAction();
};
