#pragma once
#include "Core/App/Game.h"

class TestGame : public Game
{
public:
	void Init() override;
	void Update() override;
	void End() override;
};

