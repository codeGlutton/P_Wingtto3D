#pragma once

#include "Core/Object.h"
#include "Core/Component/ActorComponent.h"

class Actor : public Object
{
	GEN_REFLECTION(Actor)

public:
	std::shared_ptr<ActorComponent> FindComponentByClass(const SubClass<ActorComponent> targetClass);

private:
	std::shared_ptr<ActorComponent> _mRootComp;
	std::unordered_set<std::shared_ptr<ActorComponent>> _mOwnedComps;
};

