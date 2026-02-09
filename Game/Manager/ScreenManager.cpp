#include "ScreenManager.h"
#include "Util/Util.h"
#include "Level/GameLevel.h"
#include "Level/MenuLevel.h"

#include <iostream>

ScreenManager* ScreenManager::instance = nullptr;

ScreenManager::ScreenManager()
{
	instance = this;

	levels.emplace_back(new MenuLevel());
	levels.emplace_back(new GameLevel());

	currentScreenType = ScreenType::Title_Menu;

	mainLevel = levels[0];
}

ScreenManager::~ScreenManager()
{
	mainLevel = nullptr;

	for (Level* level : levels)
	{
		SafeDelete(level);
	}

	levels.clear();
}

void ScreenManager::ToggleMenu(int num)
{
	// 메인 레벨 변경.
	mainLevel = levels[num];
}

int ScreenManager::GetLife()
{
	return levels[0]->As<GameLevel>()->GetLife();
}

GameLevel* ScreenManager::GetGameLevel() const
{
	return levels[1]->As<GameLevel>();
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
