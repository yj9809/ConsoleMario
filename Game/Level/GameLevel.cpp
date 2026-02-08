#include <iostream>

#include "GameLevel.h"
#include "Render/Renderer.h"
#include "Actor/Goal.h"
#include "Actor/FakeGoal.h"
#include "Actor/Wall.h"
#include "Actor/FakeWall.h"
#include "Actor/Coin.h"
#include "Actor/Enemy.h"

GameLevel::GameLevel()
{
	LoadMap("1-1.txt");

	player = AddNewActorReturn(new Player())->As<Player>();

	cameraManager = new CameraManager(120, worldWidth);
}

GameLevel::~GameLevel()
{
	SafeDelete(cameraManager);
}

void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	ProcessCollisionCoinAndPlayer();
	ProcessCollisionGoalAndPlayer();
	ProcessCollisionEnemyAndPlayer();

	cameraManager->Update(player->GetPosition().x);
	Renderer::Get().SetCameraPosition(cameraManager->GetCameraXPosition(), cameraManager->GetCameraWidth());
}

void GameLevel::Draw()
{
    super::Draw();

	char uiLife[20] = {};
	char uiScore[20] = {};

	sprintf_s(uiLife, 20, "LIFE: %d", life);
	Renderer::Get().SubmitUI(uiLife, Vector2(0, 0), Color::White, 1000);
	sprintf_s(uiScore, 20, "SCORE: %d", score);
	Renderer::Get().SubmitUI(uiScore, Vector2(0, 1), Color::White, 1000);
}

void GameLevel::Spawn()
{
	if (enemySpawn)
	{
		enemySpawn->Spawn();
	}
}

void GameLevel::ProcessCollisionCoinAndPlayer()
{
	std::vector<Actor*> coins;

	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<Coin>())
		{
			coins.push_back(actor);
			continue;
		}
	}

	if (!player || coins.size() == 0)
	{
		return;
	}

	for (Actor* const coin : coins)
	{
		if (coin->TestIntersect(player))
		{
			coin->Destroy();
			score++;
			continue;
		}
	}
}

void GameLevel::ProcessCollisionGoalAndPlayer()
{
	std::vector<Actor*> goals;

	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<Goal>())
		{
			goals.push_back(actor);
			continue;
		}
	}

	if (!player || goals.size() == 0)
	{
		return;
	}

	for (Actor* const goal : goals)
	{
		if (goal->TestIntersect(player))
		{
			int y = player->GetPosition().y;
			player->As<Player>()->SetClear();
			score += 130 - y;
			continue;
		}
	}
}

void GameLevel::ProcessCollisionEnemyAndPlayer()
{
	std::vector<Actor*> enemies;

	for (Actor* const actor : actors)
	{
		if(actor->IsTypeOf<Enemy>())
		{
			enemies.push_back(actor);
			continue;
		}
	}

	if (!player || enemies.size() == 0)
	{
		return;
	}

	for (Actor* const enemy : enemies)
	{
		if (player->TestIntersect(enemy))
		{
			if (player->GetState() == Player::State::Falling)
			{
				enemySpawn->RemoveEnemy(enemy);
				enemy->Destroy();
				score += 50;
				continue;
			}
			else 
			{
				player->RespawnAt(Vector2::SpawnPoint);
				CameraResetToSpawn();
			}
		}
	}
}

void GameLevel::CameraResetToSpawn()
{
	cameraManager->ResetToSpawn(Vector2::SpawnPoint.x);
}

int GameLevel::GetCameraXPosition()
{
	return cameraManager->GetCameraXPosition();
}

bool GameLevel::CanMove(const Vector2& nextPosition)
{
	// 레벨에 있는 벽 액터 수집.
	std::vector<Actor*> walls;

	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<Wall>())
		{
			walls.push_back(actor);
			continue;
		}
	}

	for (Actor* const wall : walls)
	{
		if (wall->GetPosition() == nextPosition)
		{
			return false;
		}
	}

	return true;
}

bool GameLevel::IsOnGround(const Vector2& playerDownPosition)
{
	// 레벨에 있는 땅 액터 수집.
	std::vector<Actor*> grounds;

	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<Wall>())
		{
			grounds.push_back(actor);
			continue;
		}
	}

	for (Actor* ground : grounds)
	{
		if (ground->GetPosition() == playerDownPosition)
		{
			return true;
		}
	}
	return false;
}

bool GameLevel::IsNextToGround(const Wanted::Vector2& enemyNextPosition)
{
	// 레벨에 있는 땅 액터 수집.
	std::vector<Actor*> grounds;

	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<Wall>())
		{
			grounds.push_back(actor);
			continue;
		}
	}

	for (Actor* ground : grounds)
	{
		if (ground->GetPosition() == enemyNextPosition)
		{
			return true;
		}
	}

	return false;
}

void GameLevel::LoadMap(const char* mapFile)
{
	char path[2048] = {};
	sprintf_s(path, 2048, "../Assets/Maps/%s", mapFile);

	FILE* file = nullptr;
	fopen_s(&file, path, "rt");

	if (!file)
	{
		std::cerr << "GameLevel::LoadMap - Failed to open map file.\n";

		__debugbreak();
	}

	fseek(file, 0, SEEK_END);

	size_t fileSize = ftell(file);

	rewind(file);

	char* buffer = new char[fileSize + 1];

	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	int maxWidth = 0;
	int index = 0;

	Wanted::Vector2 position;

	// 적 위치 저장 배열.
	std::vector<Vector2> enemyPositions;

	while (true)
	{
		if (index >= fileSize)
		{
			break;
		}

		char mapCharacter = buffer[index];
		++index;
		
		if (mapCharacter == '\n')
		{
			maxWidth = max(maxWidth, position.x);
			++position.y;
			position.x = 0;
			continue;
		}
		switch (mapCharacter)
		{
		case '#':
			AddNewActor(new Wall(position));
			break;
		case '=':
			AddNewActor(new FakeWall(position));
			break;
		case 'G':
			AddNewActor(new Goal(position));
			break;
		case 'g':
			AddNewActor(new FakeGoal(position));
			break;
		case 'C':
			AddNewActor(new Coin(position));
			break;
		case 'E':
			// 배열에 위치 값을 전부 저장.
			enemyPositions.emplace_back(position);
			break;
		}
		++position.x;
	}

	worldWidth = maxWidth;

	// 저장된 적 위치 배열이 없을 경우 스포너 생성 안함.
	if (enemyPositions.size() > 0)
	{
		// 적 스포너 액터 추가.
		enemySpawn = AddNewActorReturn(new EnemySpawner(enemyPositions))->As<EnemySpawner>();
	}

	delete[] buffer;
	fclose(file);
}
