#pragma once

#include "Actor/Actor.h"

#include "Component/CollisionComponent.h"
#include "Interface/ICanPlayerMove.h"

using namespace Wanted;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor)

public:
	Enemy(const Vector2& postion);

	inline void SetIsDestroyed() { isDestroyed = true; }
	inline bool GetIsDestroyed() const { return isDestroyed; }

	~Enemy();
private:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void Move(float deltaTime);

	void DestroyMotion(float deltaTime);

	bool WallCheck(int x, int y);

	bool GroundCheck(int x, int y);
private:
	ICanPlayerMove* canPlayerMove = nullptr;

	float moveSpeed = 10.0f;

	int moveDirection = 1;

	// 수직 속도 변수.
	float velocityY = -15.0f;
	// 수평 속도 변수.
	float velocityX = 20.0f;
	// 중력 변수.
	float gravity = 50.0f;

	float xPosition = 0.0f;

	float yPosition = 0.0f;

	bool isDestroyed = false;

	CollisionComponent component;

	Position collisionPosition;
};

