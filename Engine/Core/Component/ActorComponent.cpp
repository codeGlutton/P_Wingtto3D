#include "pch.h"
#include "ActorComponent.h"

#include "Core/World/Actor.h"

ActorComponent::ActorComponent()
{
	std::shared_ptr<ActorComponent> selfPtr = std::static_pointer_cast<ActorComponent>(shared_from_this());
	_mUpdateContext = std::make_shared<UpdateTargetContext>(std::static_pointer_cast<IUpdatable>(selfPtr), UpdatePhase::PrePhysics);
}

ActorComponent::~ActorComponent()
{
}

void ActorComponent::PostCreate()
{
	Super::PostCreate();
}

void ActorComponent::PostLoad()
{
	Super::PostLoad();
}

void ActorComponent::InitializeComponent()
{

}

void ActorComponent::BeginPlay()
{
	TIME_MANAGER->NotifyToAddTarget(_mUpdateContext);
}

void ActorComponent::Update(float deltaTime)
{

}

void ActorComponent::FixedUpdate()
{

}

void ActorComponent::EndPlay()
{
	TIME_MANAGER->NotifyToRemoveTarget(_mUpdateContext);
}

void ActorComponent::BeginDestroy()
{
	Super::BeginDestroy();
}
