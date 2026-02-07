#pragma once

#include "Level/Level.h"
#include "Interface/ICanPlayerMove.h"
#include "Actor/EnemySpawner.h"

#include <vector>

using namespace Wanted;

class GameLevel : public Level, public ICanPlayerMove
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	~GameLevel();

	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	void Spawn();

	void ProcessCollisionCoinAndPlayer();
	void ProcessCollisionGoalAndPlayer();

	virtual bool CanMove(const Vector2& nextPosition) override;

	virtual bool IsOnGround(const Vector2& playerDownPosition) override;

	virtual bool IsNextToGround(const Wanted::Vector2& enemyNextPosition) override;

	inline int GetLife() const { return life; }
	inline void SetLife() { life--; }

private:
	void LoadMap(const char* mapFile);

private:
	EnemySpawner* enemySpawn = nullptr;

	int score = 0;

	int life = 3;
};

