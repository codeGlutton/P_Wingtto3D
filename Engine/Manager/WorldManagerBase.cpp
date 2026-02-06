#include "pch.h"
#include "WorldManagerBase.h"

void WorldManagerBase::Init(std::shared_ptr<World> world)
{
	_mWorld = world;
}

void WorldManagerBase::Update(float deltaTime)
{
}

void WorldManagerBase::Destroy()
{
}

