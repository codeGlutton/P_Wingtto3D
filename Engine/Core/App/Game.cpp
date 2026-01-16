#include "pch.h"
#include "Game.h"

#include "Manager/AppWindowManager.h"
#include "Core/App/AppWindow/GameAppWindow.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Create()
{
	_mMainWindow = std::make_unique<AppWindow>();
}

void Game::Init()
{
}

void Game::Update()
{
}

void Game::End()
{
}

void Game::CreateMainWindow()
{
	_mMainWindow = APP_WIN_MANAGER->CreateAppWindow<GameAppWindow>();
}
