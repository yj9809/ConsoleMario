#pragma once

#include "Actor/Actor.h"
#include "Level/Level.h"
#include "Util/Util.h"

#include "Interface/ICanPlayerMove.h"
using namespace Wanted;

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

public :
	Player();

private:
	virtual void Tick(float deltaTime) override;

	void Move(float deltaTime);
	void MoveRight(float deltaTime);
	void MoveLeft(float deltaTime);

	void Jump(float deltaTime);
	void Fall();

	// 가속도 설정 함수.
	inline void SetWeight(float& weight, float deltaTime)
	{
		// 시간이 지날수록 가속도 증가.
		weight += deltaTime * 5.0f;
		// 가속도 최대치 설정.
		Util::Clamp(weight, 0.0f, 35.0f);
	}

private:
	ICanPlayerMove* canPlayerMove = nullptr;

	// 이동 속도 변수
	float moveSpeed = 25.0f;

	// 가속도 변수.
	float weight = 0.0f;

	// 마지막 이동 방향 변수.
	// true : 오른쪽, false : 왼쪽
	bool lastDir = true; 

	float xPosition = 0.0f;

	// 점프 가능여부 확인 플래그.
	bool isJumping = false;

	// 수직 속도 변수.
	float velocityY = 0.0f;
	// 점프 속도 변수.
	float jumpSpeed = 45.0f;
	// 중력 변수.
	float gravity = 100.0f;    

	float yPosition = 0.0f;
};

