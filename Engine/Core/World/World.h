#pragma once

#include "Core/Object.h"

#include "Core/GUID.h"

class Level;
class Actor;

/**
 * 게임 로직 스레드에서 관리할 수 있는 현재 공간 객체
 */
class World : public Object
{
	GEN_REFLECTION(World)

public:
	//Actor* CreateActor();

private:
	std::shared_ptr<Level> _mMainLevel;
	std::unordered_map<ObjectGUID, std::shared_ptr<Actor>> _mActors;
};

