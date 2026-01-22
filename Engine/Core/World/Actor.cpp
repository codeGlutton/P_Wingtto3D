#include "pch.h"
#include "Actor.h"

Actor::Actor()
{
	std::shared_ptr<Actor> selfPtr = std::static_pointer_cast<Actor>(shared_from_this());
	_mUpdateContext = std::make_shared<UpdateTargetContext>(std::static_pointer_cast<IUpdatable>(selfPtr), UpdatePhase::PrePhysics);
}

Actor::~Actor()
{
}

void Actor::PostCreate()
{
	Super::PostCreate();
}

void Actor::PostLoad()
{
	Super::PostLoad();
}

void Actor::BeginPlay()
{
	TIME_MANAGER->NotifyToAddTarget(_mUpdateContext);
}

void Actor::Update(float deltaTime)
{
}

void Actor::EndPlay()
{
	TIME_MANAGER->NotifyToRemoveTarget(_mUpdateContext);
}

void Actor::BeginDestroy()
{
	Super::BeginDestroy();
}

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
