#include "EnemySpawner.h"
#include "Level/Level.h"
#include "Level/GameLevel.h"
#include "Actor/Enemy.h"

EnemySpawner::EnemySpawner(const std::vector<Vector2>& newEnemyPosition)
	: enemyPositions(newEnemyPosition)
{
	
}

void EnemySpawner::BeginPlay()
{
	super::BeginPlay();

	Spawn();
}

// Todo: 추후 플레이어가 적을 처치할 경우 배열에서 제거하기 위한 함수.
void EnemySpawner::RemoveEnemy(Actor* targetEnemy)
{
	auto target = std::find(enemies.begin(), enemies.end(), targetEnemy);
	if (target != enemies.end())
	{
		enemies.erase(target);
	}
}

void EnemySpawner::Spawn()
{
	// 플레이어가 죽을 경우 남은 적을 처리하기 위해 
	// 배열 내 모든 적 삭제.
	if (enemies.size() > 0)
	{
		for (Actor* enemy : enemies)
		{
			enemy->Destroy();
		}

		enemies.clear();
	}

	// 저장된 위치 값 기준으로 적 생성.
	for (Vector2& position : enemyPositions)
	{
		Actor* enemy = GetOwner()->AddNewActorReturn(new Enemy(position));;
		
		enemies.emplace_back(enemy);
	}
}
