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
		Start,
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

	// 콜리전을 활용한 플레이어 적 상호작용 이벤트 처리 함수,
	void OnPlayerHitEnemy(Enemy* enemy, bool stemped);

	void CameraResetToSpawn();

	int GetCameraXPosition();

	virtual bool CanMove(const Vector2& nextPosition) override;

	virtual bool IsOnGround(const Vector2& playerDownPosition) override;

	virtual bool IsNextToGround(const Wanted::Vector2& enemyNextPosition) override;

	void Init();

	// 플레이어 반환 함수
	inline Player* GetPlayer() const { return player; }

private:
	void LoadMap(const char* mapFile);

private:
	Spawner* enemySpawn = nullptr;

	CameraManager* cameraManager = nullptr;

	Map currentMap = Map::Start;

	Player* player = nullptr;

	int worldWidth = 0;

	int score = 0;

	int life = 3;

	// UI 문자열 버퍼.
	char uiLife[20] = {};
	char uiScore[20] = {};
};

