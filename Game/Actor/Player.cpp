#include "Player.h"
#include "Engine/Engine.h"
#include "Core/Input.h"

#include "Interface/ICanPlayerMove.h"

#define RIGHT_KEY (Input::Get().GetKey(VK_RIGHT))
#define LEFT_KEY (Input::Get().GetKey(VK_LEFT))
#define SPACE_DOWN (Input::Get().GetKeyDown(VK_SPACE))

Player::Player()
	: super("MM", Vector2(0, 21), Color::Red), yPosition(position.y)
{
	sortingOrder = 10;
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	Move(deltaTime);

	if (!isJumping && SPACE_DOWN) 
	{
		isJumping = true;

		// 점프 시작 시 속도 설정 (전제 조건: JumpSpeed > 0).
		// Console 특성 상 상승은 음수 방향이므로 음수 값 할당.
		velocityY = -jumpSpeed;
	}
	Jump(deltaTime);
}

void Player::Move(float deltaTime)
{
	if (RIGHT_KEY && !LEFT_KEY)
	{
		if (lastDir) {
			SetWeight(weight, deltaTime);
		}
		else {
			weight = 0;
			lastDir = true;
		}

		MoveRight(deltaTime);
	}
	else if (LEFT_KEY && !RIGHT_KEY)
	{
		if (!lastDir)
		{
			SetWeight(weight, deltaTime);
		}
		else
		{
			weight = 0;
			lastDir = false;
		}
		MoveLeft(deltaTime);
	}
	else
	{
		weight = 0.0f;
	}
}

void Player::MoveRight(float deltaTime)
{
	xPosition += (moveSpeed + weight) * deltaTime;

	static ICanPlayerMove* canPlayerMove = dynamic_cast<ICanPlayerMove*>(GetOwner());

	Vector2 nextPosition = Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition));
	if (!canPlayerMove->CanMove(Vector2(position.x + width / 2, position.y), nextPosition))
	{
		xPosition = position.x;
	}

	if (xPosition > Engine::Get().GetWidth() - width)
	{
		xPosition = static_cast<float>(Engine::Get().GetWidth() - width);
	}

	SetPosition(nextPosition);
}

void Player::MoveLeft(float deltaTime)
{
	xPosition -= (moveSpeed + weight) * deltaTime;

	if (xPosition < 0.0f)
	{
		xPosition = 0.0f;
	}

	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
}

void Player::Jump(float deltaTime)
{
	if (!isJumping)
		return;

	static ICanPlayerMove* canPlayerMove = dynamic_cast<ICanPlayerMove*>(GetOwner());

	// 프레임마다 중력(가속도)을 속도에 누적한다.
	// velocityY는 시간이 지날수록 증가하며(아래 방향), 상승 중(음수)에는 0으로 수렴하고
	// 0을 지나 양수가 되면 자연스럽게 하강으로 전환된다.
	velocityY += gravity * deltaTime;

	// 위치 업데이트.
	yPosition += velocityY * deltaTime;

	Vector2 nextPosition = Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition));

	// Todo: 충돌 로직이 완성되지 않았기 때문에 임시로 땅 높이를 20.0f로 고정.
	// 지면에 닿으면 위치/속도를 보정하고 점프 상태를 해제한다.
	if (!canPlayerMove->CanMove(position, nextPosition))
	{
		if (velocityY > 0)
		{
			// 땅에 착지했으므로 위치 보정.
			yPosition = position.y;
			// 속도 초기화.
			velocityY = 0.0f;
			// 착지 후 점프 플래그 초기화.
			isJumping = false;
		}
		else
		{
			// 천장에 부딪혔으므로 위치 보정.
			yPosition = position.y;
			// 속도 초기화.
			velocityY = 0.0f;
		}
	}

	// 위치 업데이트.
	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
}
