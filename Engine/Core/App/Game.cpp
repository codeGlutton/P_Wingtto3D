#include "pch.h"
#include "Game.h"

#include "Manager/PathManager.h"
#include "Manager/ResourceManager.h"

#include "Core/Resource/Texture.h"
#include "Core/Resource/Font.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init()
{
	AppModeBase::Init();
}

void Game::Update()
{
	AppModeBase::Update();
}

void Game::End()
{
	AppModeBase::End();
}

void Game::RegisterDefaultResources()
{
	AppModeBase::RegisterDefaultResources();

	const std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();

	// 텍스처
	{
		std::wstring resourcePath = packagePath + L"\\Texture\\T_White";
		std::shared_ptr<Texture2D> whiteTex = RESOURCE_MANAGER->LoadOrGetResource<Texture2D>(resourcePath);

		whiteTex->GetProxy();
		_mDefaultResources.push_back(whiteTex);
	}

	// 기본 폰트
	{
		const std::wstring resourcePath = packagePath + L"\\Font\\F_Cafe24Decobox_Regular";
		std::shared_ptr<Font> defaultFont = RESOURCE_MANAGER->LoadOrGetResource<Font>(resourcePath);

		_mDefaultResources.push_back(defaultFont);
	}
}

void Game::UnregisterDefaultResources()
{
	AppModeBase::UnregisterDefaultResources();
}

