#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class Player;

class MovePlatform : public Actor
{
	struct PlayerPosition
	{
		PlayerPosition(const Player& player, const Vector2& platformPosition, int width);

	public:
		bool IsOnTop();


	private:
		int pLeft = 0;
		int pRight = 0;
		int pBottom = 0;
		int platLeft = 0;
		int platRight = 0;
		int platTop = 0;
	};

	RTTI_DECLARATIONS(MovePlatform, Actor)

public:

	MovePlatform(const Vector2& position, const Vector2& endPosition);

	virtual void Tick(float deltaTime) override;

private:
	Vector2 startPosition;

	Vector2 endPosition;

	float moveSpeed = 0.2f;

	int moveDirection = 1;

	float t = 0.0f;
};

