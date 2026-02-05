#pragma once

#include "Level/Level.h"
#include "Interface/ICanPlayerMove.h"


using namespace Wanted;

class GameLevel : public Level, public ICanPlayerMove
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	~GameLevel();

	virtual void Draw() override;

	virtual bool CanMove(const Vector2& playerPosition, const Vector2& nextPosition) override;

	virtual bool IsOnGround(const Vector2& playerDownPosition) override;
private:
	void LoadMap(const char* mapFile);
};

