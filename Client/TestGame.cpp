#include "pch.h"
#include "TestGame.h"

#include "Manager/InputManager.h"

void TestGame::Init()
{
	Game::Init();
	INPUT_MANAGER->Bind(KeyType::Pos, false, false, false, nullptr, OnCallInputBinding::Binder::BindLambda([](const InputValue* value, KeyState::Type state) {
		if (state == KeyState::Trigger)
		{
			OutputDebugStringW(L"마우스 이동 중\n");
		}
		else
		{
			OutputDebugStringW(L"마우스 정지\n");
		}
		}));
}

void TestGame::Update()
{
	Game::Update();
}

void TestGame::End()
{
	Game::End();
}
