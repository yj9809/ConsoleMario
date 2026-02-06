#pragma once

#include "Engine/Engine.h"

#include <vector>

using namespace Wanted;

enum class ScreenType
{
	Title,
	Game,
	Respawn,
	GameOver,
	Clear
};

class ScreenManager : public Engine
{
public:
	ScreenManager();
	~ScreenManager();

	void ToggleMenu();

	static ScreenManager& Get();
private:
	std::vector<Level*> levels;

	ScreenType currentScreenType = ScreenType::Game;

	static ScreenManager* instance;
};

