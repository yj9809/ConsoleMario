#pragma once

#include "Actor/Actor.h"
#include "Level/Level.h"
#include "Util/Util.h"

#include "Interface/ICanPlayerMove.h"

using namespace Wanted;

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

public:
	enum class State
	{
		Idle,
		Crash,
		Jumping,
		Falling,
		Clear,
		Death
	};

public:
	Player();

	inline void SetClear() { currentState = State::Clear; }

	void ClearMove(float deltaTime);

	inline void SetDeath(State state, bool left)
	{
		currentState = state;

		// 기본값 설정.
		deathVelocityX = 20.0f;
		deathVelocityY = -15.0f;
		deathGravity = 50.0f;

		// 방향 전환.
		// 플레이어가 적 왼쪽에서 충돌했으면 왼쪽으로 날아가고,
		// 오른쪽에서 충돌했으면 오른쪽으로 날아가도록 설정.
		SetDeathVelocityX(left);

		// 현재 좌표 동기화.
		xPosition = position.x;
		yPosition = position.y;
	}

	inline void SetDeathVelocityX(bool left)
	{
		if (left)
		{
			deathVelocityX = -Util::Abs(deathVelocityX);
		}
		else
		{
			deathVelocityX = Util::Abs(deathVelocityX);
		}
	}
	inline State GetState() { return currentState; }

	inline void ResetPosition()
	{
		currentState = State::Idle;
		SetPosition(Vector2::SpawnPoint); 
		xPosition = position.x; 
		yPosition = position.y;
	}

	// 리스폰 함수.
	void RespawnAt(const Vector2& pos);

	// 플랫폼 이동 동기화 함수.
	void AddPlatformMove(const Vector2& delta);

private:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void Move(float deltaTime);
	void MoveRight(float deltaTime);
	void MoveLeft(float deltaTime);

	void Jump(float deltaTime);
	void Fall();

	void DeathMotion(float deltaTime);
	
	// 가속도 설정 함수.
	void SetWeight(float& weight, float deltaTime);

	inline int GetLife() const;
	

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

	// 사망 모션 변수.
	float deathGravity = 0.0f;
	float deathVelocityX = 0.0f;
	float deathVelocityY = -0.0f;

	// 바닥 체크 플래그.
	bool isGround = false;
};


