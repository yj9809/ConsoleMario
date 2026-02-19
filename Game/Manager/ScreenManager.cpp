#include "ScreenManager.h"
#include "Util/Util.h"
#include "Level/GameLevel.h"
#include "Level/MenuLevel.h"
#include "Actor/Actor.h"

#include <iostream>

ScreenManager* ScreenManager::instance = nullptr;

ScreenManager::ScreenManager()
{
	instance = this;

	collisionSystem = new CollisionSystem();

	menuLevel = new MenuLevel();
	gameLevel = new GameLevel();

	levels.emplace_back(menuLevel);
	levels.emplace_back(gameLevel);

	SetTitleScreen();
}

ScreenManager::~ScreenManager()
{
	mainLevel = nullptr;

	for (Level* level : levels)
	{
		SafeDelete(level);
	}

	SafeDelete(collisionSystem);

	levels.clear();
}

ScreenManager& ScreenManager::Get()
{
	// 예외 처리.
	if (!instance)
	{
		std::cerr << "Game::Get() - instance is null\n";
		__debugbreak();
	}

	// 정적 변수 반환.
	return *instance;
}

bool ScreenManager::ConsumeGameClearRequest()
{
	if (requestGameClear)
	{
		requestGameClear = false;
		return true;
	}
	return false;
}

void ScreenManager::SetTitleScreen()
{
	menuLevel->SetMenuType(MenuLevel::MenuType::Title);
	ToggleMenu(0);
}

void ScreenManager::SetGameStartScreen()
{
	gameLevel->Init();
	ToggleMenu(1);
}

void ScreenManager::SetInGameScreen()
{
	ToggleMenu(1);
}

void ScreenManager::SetInGameMenuScreen()
{
	menuLevel->SetMenuType(MenuLevel::MenuType::InGame);
	ToggleMenu(0);
}

void ScreenManager::SetRespawnScreen()
{
	menuLevel->SetMenuType(MenuLevel::MenuType::Respawn);
	ToggleMenu(0);
}

void ScreenManager::SetMapClearScreen()
{
	menuLevel->SetMenuType(MenuLevel::MenuType::MapClear);
	ToggleMenu(0);
}

void ScreenManager::SetGameClearScreen()
{
	menuLevel->SetMenuType(MenuLevel::MenuType::GameClear);
	ToggleMenu(0);
}

void ScreenManager::SetGameOverScreen()
{
	menuLevel->SetMenuType(MenuLevel::MenuType::GameOver);
	ToggleMenu(0);
}

void ScreenManager::ToggleMenu(int num)
{
	// 메인 레벨 변경.
	mainLevel = levels[num];
}

