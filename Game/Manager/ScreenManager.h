#pragma once

#include "Engine/Engine.h"
#include "System/CollisionSystem.h"

#include <vector>

using namespace Wanted;

namespace Wanted { class Actor; }

class GameLevel;

enum class ScreenType
{
	Title_Menu,
	Start,
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

	ScreenType currentScreenType = ScreenType::Start;

	CollisionSystem& GetCollisionSystem() { return *collisionSystem; }

private:
	std::vector<Level*> levels;

	static ScreenManager* instance;

	static CollisionSystem* collisionSystem;
};

