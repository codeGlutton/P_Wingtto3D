#include "pch.h"
#include "Actor.h"

std::shared_ptr<ActorComponent> Actor::FindComponentByClass(const SubClass<ActorComponent> targetClass)
{
	std::shared_ptr<ActorComponent> targetComp = nullptr;
	const TypeInfo* typeInfo = targetClass.Get();
	
	if (typeInfo != nullptr)
	{
		for (const std::shared_ptr<ActorComponent>& comp : _mOwnedComps)
		{
			if (comp != nullptr && comp->IsA(targetClass))
			{
				targetComp = comp;
				break;
			}
		}
	}
	return targetComp;
}
