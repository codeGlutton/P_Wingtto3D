#pragma once

#include "Input/InputAction.h"
#include "Core/Object.h"

struct InputMapping
{
	GEN_STRUCT_REFLECTION(InputMapping)

public:
	bool operator==(const InputMapping& other) const
	{
		return mAction == other.mAction &&
			mBindings == other.mBindings;
	}

public:
	PROPERTY(mAction)
	SubClass<InputAction> mAction;
	PROPERTY(mBindings)
	std::vector<InputBinding> mBindings;
};

class InputMappingContext : public Object
{
	GEN_REFLECTION(InputMappingContext)

	friend class WorldGameInputManager;

protected:
	PROPERTY(_mMappings)
	std::vector<InputMapping> _mMappings;
};
