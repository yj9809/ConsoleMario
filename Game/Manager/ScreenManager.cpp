#include "ScreenManager.h"
#include "Util/Util.h"
#include "Level/GameLevel.h"
#include "Level/MenuLevel.h"

#include <iostream>

ScreenManager* ScreenManager::instance = nullptr;

ScreenManager::ScreenManager()
{
	instance = this;

	levels.emplace_back(new GameLevel());
	levels.emplace_back(new MenuLevel());

	currentScreenType = ScreenType::Game;

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

void ScreenManager::ToggleMenu()
{
	int stateIndex = (int)currentScreenType;
	int nextState = 1 - stateIndex;
	currentScreenType = static_cast<ScreenType>(nextState);

	// 메인 레벨 변경.
	mainLevel = levels[static_cast<int>(currentScreenType)];
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
