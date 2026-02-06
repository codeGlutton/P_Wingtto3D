#pragma once

#include "Core/World/World.h"

class WorldManagerBase
{
public:
	std::shared_ptr<World> GetWorld() const
	{
		return _mWorld.lock();
	}

	bool IsValid() const
	{
		return _mWorld.expired() == false;
	}

public:
	virtual void Init(std::shared_ptr<World> world);
	virtual void Update(float deltaTime);
	virtual void Destroy();

protected:
	std::weak_ptr<World> _mWorld;
};

