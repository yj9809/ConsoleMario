#include <iostream>

#include "GameLevel.h"
#include "Actor/Ground.h"
#include "Actor/Wall.h"
#include "Actor/Player.h"
#include "Actor/FakeWall.h"

GameLevel::GameLevel()
{
	LoadMap("1-1.txt");

	AddNewActor(new Player());
}

GameLevel::~GameLevel()
{
}

void GameLevel::Draw()
{
    super::Draw();
}

bool GameLevel::CanMove(const Vector2& playerPosition, const Vector2& nextPosition)
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

	for(Actor* const wall : walls)
	{
		if (wall->GetPosition() == nextPosition)
		{
			return false;
		}
	}

	return true;
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

	int index = 0;

	Wanted::Vector2 position;

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
			AddNewActor(new Ground(position));
		}
		++position.x;
	}

	delete[] buffer;
	fclose(file);
}
