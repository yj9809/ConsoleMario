#pragma once

#include "Actor/Actor.h"
#include "Level/Level.h"
#include "Util/Util.h"

#include "Interface/ICanPlayerMove.h"
using namespace Wanted;

class Player : public Actor
{
	enum class State
	{
		Idle,
		Crash,
		Jumping,
		Falling,
		Clear,
	};

	RTTI_DECLARATIONS(Player, Actor)

public:
	Player();

	inline void SetClear() { currentState = State::Clear; }

	void ClearMove(float deltaTime);

	inline State GetState() { return currentState; }

private:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void Move(float deltaTime);
	void MoveRight(float deltaTime);
	void MoveLeft(float deltaTime);

	void Jump(float deltaTime);
	void Fall();

	// 리스폰 함수.
	void RespawnAt(const Vector2& pos);

	// 가속도 설정 함수.
	void SetWeight(float& weight, float deltaTime);

private:
	ICanPlayerMove* canPlayerMove = nullptr;

	State currentState = State::Idle;

	bool isCheck = false;

	// 이동 속도 변수
	float moveSpeed = 20.0f;

	// 가속도 변수.
	float weight = 0.0f;

	// 마지막 이동 방향 변수.
	// true : 오른쪽, false : 왼쪽
	bool lastDir = true;

	float xPosition = 0.0f;

	// 수직 속도 변수.
	float velocityY = 0.0f;
	// 점프 속도 변수.
	float jumpSpeed = 40.0f;
	// 중력 변수.
	float gravity = 100.0f;

	float yPosition = 0.0f;

	// 낙하 중력 변수.
	float fallGravity = 200.0f;

	// 바닥 체크 플래그.
	bool isGround = false;
};


