#pragma once

#include "Input/InputInclude.h"
#include "Input/InputBinding.h"
#include "Input/InputValue.h"
#include "Core/Object.h"

DECLARE_MULTICAST_DELEGATE_1_PARAM(OnTiggerInputAction, const InputValue*);

class InputAction : public Object
{
	GEN_REFLECTION(InputAction)

	friend class InputManager;

protected:
	PROPERTY(_mValueType)
	KeyValueType _mValueType;
};

