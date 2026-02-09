#pragma once

#include "Engine/Engine.h"

#include <vector>

using namespace Wanted;

class GameLevel;

enum class ScreenType
{
	Title_Menu,
	Game,
	Respawn,
	GameOver,
	MapClear,
	GameClear
};

class ScreenManager : public Engine
{
public:
	ScreenManager();
	~ScreenManager();

	void ToggleMenu(int num);

	int GetLife();

	GameLevel* GetGameLevel() const;

	static ScreenManager& Get();

	ScreenType currentScreenType = ScreenType::Game;
private:
	std::vector<Level*> levels;

	static ScreenManager* instance;
};

