#pragma once

#include "Actor/Actor.h"

#include <vector>

using namespace Wanted;

class EnemySpawner : public Actor
{
	RTTI_DECLARATIONS(EnemySpawner, Actor)

public:
	EnemySpawner(const std::vector<Vector2>& newEnemyPosition);

	virtual void BeginPlay() override;

	void RemoveEnemy(Actor* targetEnemy);

	void Spawn();

private:
	std::vector<Vector2> enemyPositions;
	
	std::vector<Actor*> enemies;
};

