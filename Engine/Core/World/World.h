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

private:
	// 소유를 위한 액터 맵
	std::unordered_map<std::wstring, std::shared_ptr<Actor>> _mActors;
	// 이번 틱에서 업데이트할 액터 배열
	std::vector<std::shared_ptr<Actor>> _mChangedActors;
};

