#pragma once

#include "Actor/Actor.h"

#include <vector>

using namespace Wanted;

class Spawner : public Actor
{
	RTTI_DECLARATIONS(Spawner, Actor)

public:
	Spawner(const std::vector<Vector2>& newEnemyPosition);

	virtual void BeginPlay() override;

	void RemoveEnemy(Actor* targetEnemy);

	void Spawn();

private:
	std::vector<Vector2> enemyPositions;
	
	std::vector<Actor*> enemies;
};

