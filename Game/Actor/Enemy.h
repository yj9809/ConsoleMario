#pragma once

#include "Actor/Actor.h"

#include "Interface/ICanPlayerMove.h"

using namespace Wanted;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor)

public:
	Enemy(const Vector2& postion);

private:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void Move(float deltaTime);

	bool WallCheck(int x, int y);

	bool GroundCheck(int x, int y);

private:
	ICanPlayerMove* canPlayerMove = nullptr;

	float moveSpeed = 10.0f;

	int moveDirection = 1;

	float xPosition = 0.0f;
};

