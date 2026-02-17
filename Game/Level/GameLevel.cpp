#include <iostream>

#include "GameLevel.h"
#include "Manager/ScreenManager.h"
#include "Render/Renderer.h"
#include "Actor/Goal.h"
#include "Actor/FakeGoal.h"
#include "Actor/Wall.h"
#include "Actor/FakeWall.h"
#include "Actor/Coin.h"
#include "Actor/Enemy.h"
#include "Actor/MovePlatform.h"
#include "Manager/ScreenManager.h"

GameLevel::GameLevel()
{
	player = AddNewActorReturn(new Player())->As<Player>();

	cameraManager = new CameraManager(120, worldWidth);
}

GameLevel::~GameLevel()
{
	SafeDelete(cameraManager);
}

void GameLevel::Tick(float deltaTime)
{
	if (ScreenManager::Get().currentScreenType == ScreenType::Start)
	{
		Init();
		ScreenManager::Get().currentScreenType = ScreenType::Game;
	}

	if (!clearFlag)
	{
		super::Tick(deltaTime);

		ScreenManager::Get().GetCollisionSystem().Step();
		ProcessCollisionCoinAndPlayer();
		ProcessCollisionGoalAndPlayer();
		//ProcessCollisionEnemyAndPlayer();

		cameraManager->Update(player->GetPosition().x);
		Renderer::Get().SetCameraPosition(cameraManager->GetCameraXPosition(), cameraManager->GetCameraWidth());
	}
	else
	{
		clearFlag = false;
		if(actors.size() > 0)
		{
			for (Actor* const actor : actors)
			{
				actor->Destroy();
			}
			//actors.clear();
			ProcessAddAndDestroyActors();
		}

		if (currentMap == Map::Start)
		{
			currentMap = Map::Map1;
			LoadMap("1-1.txt");
		}
		else if (currentMap == Map::Map1)
		{
			currentMap = Map::Map2;
			LoadMap("1-2.txt");
		}
		else if (currentMap == Map::Map2)
		{
			currentMap = Map::Map3;
			LoadMap("1-3.txt");
		}
		else if (currentMap == Map::Map3)
		{
			// Todo: 게임 클리어 처리.
			ScreenManager::Get().currentScreenType = ScreenType::GameClear;
			ScreenManager::Get().ToggleMenu(0);
		}

		if (cameraManager)
		{
			SafeDelete(cameraManager);
		}

		player = AddNewActorReturn(new Player())->As<Player>();

		cameraManager = new CameraManager(120, worldWidth);
	}
}

void GameLevel::Draw()
{
	super::Draw();

	sprintf_s(uiLife, 20, "LIFE: %d", life);
	Renderer::Get().Submit(uiLife, Vector2(0, 0), Color::White, 1000, true);
	sprintf_s(uiScore, 20, "SCORE: %d", score);
	Renderer::Get().Submit(uiScore, Vector2(0, 1), Color::White, 1000, true);
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
			if (player->GetState() != Player::State::Death)
			{
				coin->Destroy();
				score += 10;
				continue;
			}
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
	if (player->GetState() != Player::State::Death)
	{
		for (Actor* const goal : goals)
		{
			if (goal->TestIntersect(player) && player->GetState() != Player::State::Clear)
			{
				int y = player->GetPosition().y;
				score += 130 - y;
				player->As<Player>()->SetClear();
				continue;
			}
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
			if (!enemy->As<Enemy>()->GetIsDestroyed())
			{
				if (player->GetState() == Player::State::Falling)
				{
					enemySpawn->RemoveEnemy(enemy);
					enemy->As<Enemy>()->SetIsDestroyed();
					score += 50;
					continue;
				}
				else
				{
					player->SetDeath(Player::State::Death, player->GetPosition().x < enemy->GetPosition().x);
					CameraResetToSpawn();
				}
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
	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<Wall>() || actor->IsTypeOf<MovePlatform>())
		{
			const int ax = actor->GetPosition().x;
			const int ay = actor->GetPosition().y;
			const int aw = actor->GetWidth();
			const int ah = 1; // MovePlatform은 2줄인데, Actor에 높이 getter가 없으면 1로만 체크

			const bool insideX = (nextPosition.x >= ax) && (nextPosition.x <= ax + aw - 1);
			const bool insideY = (nextPosition.y >= ay) && (nextPosition.y <= ay + ah - 1);

			if (insideX && insideY)
				return false;
		}
	}
	return true;
}

bool GameLevel::IsOnGround(const Vector2& playerDownPosition)
{
	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<Wall>() || actor->IsTypeOf<MovePlatform>())
		{
			int gx = actor->GetPosition().x;
			int gy = actor->GetPosition().y;
			int gw = actor->GetWidth();

			bool isOn = (playerDownPosition.y == gy) &&
				(playerDownPosition.x >= gx) &&
				(playerDownPosition.x <= gx + gw - 1);

			if (isOn)
			{
				return true;
			}
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

	std::vector<Vector2> startPositions;
	std::vector<Vector2> endPositions;

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
		case 'M':
			startPositions.emplace_back(position);
			break;
		case 'N':
			endPositions.emplace_back(position);
			break;
		}
		++position.x;
	}

	worldWidth = maxWidth;

	// 저장된 적 위치 배열이 없을 경우 스포너 생성 안함.
	if (enemyPositions.size() > 0)
	{
		// 적 스포너 액터 추가.
		enemySpawn = AddNewActorReturn(new Spawner(enemyPositions))->As<Spawner>();
	}

	for (int ix = 0; ix < startPositions.size(); ix++)
	{
		AddNewActor(new MovePlatform(startPositions[ix], endPositions[ix]));
	}

	delete[] buffer;
	fclose(file);
}
