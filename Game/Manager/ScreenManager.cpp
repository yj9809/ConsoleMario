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
	// 변경할 인덱스 계산.
	// 현재 활성 레벨 인덱스가 1이면 -> 0으로.
	// 현재 활성 레벨 인덱스가 0이면 -> 1로.
	// 마법의 공식 - (1-x) -> OneMinus
	int stateIndex = (int)currentScreenType;	// static_cast.
	int nextState = 1 - stateIndex;	// one - x.
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
