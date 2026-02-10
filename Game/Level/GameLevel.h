#pragma once

#include "Level/Level.h"
#include "Interface/ICanPlayerMove.h"
#include "Actor/Spawner.h"
#include	"Actor/Player.h"
#include  "Manager/CameraManager.h"

#include <vector>

using namespace Wanted;

class GameLevel : public Level, public ICanPlayerMove
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	enum class Map
	{
		Map1,
		Map2,
		Map3,
	};

public:
	GameLevel();
	~GameLevel();

	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	void Spawn();

	void ProcessCollisionCoinAndPlayer();
	void ProcessCollisionGoalAndPlayer();
	void ProcessCollisionEnemyAndPlayer();

	void CameraResetToSpawn();

	int GetCameraXPosition();

	virtual bool CanMove(const Vector2& nextPosition) override;

	virtual bool IsOnGround(const Vector2& playerDownPosition) override;

	virtual bool IsNextToGround(const Wanted::Vector2& enemyNextPosition) override;

	inline void Init()
	{
		cameraManager->ResetToSpawn(Vector2::SpawnPoint.x);
		player->ResetPosition();
		score = 0;
		life = 3;
		currentMap = Map::Map1;
	}
	inline int GetLife() const { return life; }
	inline void SetLife() { life--; }
	inline Map GetCurrentMap() const { return currentMap; }

	// 플레이어 반환 함수
	inline Player* GetPlayer() const { return player; }

	// 맵 클리어 플래그
	inline void SetClearFlag(bool flag) { clearFlag = flag; }

private:
	void LoadMap(const char* mapFile);

private:
	Spawner* enemySpawn = nullptr;

	CameraManager* cameraManager = nullptr;

	Map currentMap = Map::Map1;

	Player* player = nullptr;

	int worldWidth = 0;

	int score = 0;

	int life = 3;

	// UI 문자열 버퍼.
	char uiLife[20] = {};
	char uiScore[20] = {};

	// 맵 클리어 플래그.
	bool clearFlag = false;
};

