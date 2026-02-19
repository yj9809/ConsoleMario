#pragma once

#include "Engine/Engine.h"
#include "System/CollisionSystem.h"

#include <vector>

using namespace Wanted;

namespace Wanted { class Actor; }

class GameLevel;
class MenuLevel;

class ScreenManager : public Engine
{
public:
	ScreenManager();
	~ScreenManager();

	static ScreenManager& Get();

	// 클리어 플래그 세팅 함수.
	inline void RequestGameClear() { requestGameClear = true; }
	bool ConsumeGameClearRequest();

	// 화면 전환 함수들.
	// 타이틀 화면으로 전환.
	void SetTitleScreen();
	// 타이틀에서 Start Game을 선택했을 때 게임 플레이 화면으로 전환.
	void SetGameStartScreen();
	// 메뉴나 Respawn, Clear, GameOver 화면에서 
	// Resume Game이나 Enter를 선택했을 때 게임 플레이 화면으로 전환.
	void SetInGameScreen();
	// 게임 플레이 중 ESC를 눌렀을 때 메뉴 화면으로 전환.
	void SetInGameMenuScreen();
	// 플레이어가 사망했을 때 리스폰 화면으로 전환.
	void SetRespawnScreen();
	// 맵 클리어 시 맵 클리어 화면으로 전환.
	void SetMapClearScreen();
	// 게임 클리어 시 게임 클리어 화면으로 전환.
	void SetGameClearScreen();
	// 게임 오버 시 게임 오버 화면으로 전환.
	void SetGameOverScreen();

	CollisionSystem& GetCollisionSystem() { return *collisionSystem; }

private:
	void ToggleMenu(int num);

private:
	std::vector<Level*> levels;

	static ScreenManager* instance;

	CollisionSystem* collisionSystem;

	// 레벨 포인터.
	MenuLevel* menuLevel = nullptr;
	GameLevel* gameLevel = nullptr;

	// 게임 클리어 요청 상태 플래그.
	bool requestGameClear = false;
};

