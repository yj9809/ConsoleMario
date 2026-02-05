#include "Player.h"
#include "Engine/Engine.h"
#include "Core/Input.h"

#define RIGHT_KEY (Input::Get().GetKey(VK_RIGHT))
#define LEFT_KEY (Input::Get().GetKey(VK_LEFT))
#define SPACE_DOWN (Input::Get().GetKeyDown(VK_SPACE))

Player::Player()
	: super("MMMMM", Vector2(0, 21), Color::Red), yPosition(position.y)
{
	sortingOrder = 10;
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (!canPlayerMove)
	{
		canPlayerMove = dynamic_cast<ICanPlayerMove*>(GetOwner());
	}

	Move(deltaTime);

	Fall();

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

	Vector2 nextPosition = Vector2(static_cast<int>(xPosition + width - 1), static_cast<int>(yPosition));
	if (!canPlayerMove->CanMove(Vector2(position.x + width - 1, position.y), nextPosition))
	{
		xPosition = position.x;
	}

	if (xPosition + width > Engine::Get().GetWidth())
	{
		xPosition = static_cast<float>(Engine::Get().GetWidth() - width);
	}
	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
}

void Player::MoveLeft(float deltaTime)
{
	xPosition -= (moveSpeed + weight) * deltaTime;

	if (xPosition < 0.0f)
	{
		xPosition = 0.0f;
	}

	Vector2 nextPosition = Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition));
	if (!canPlayerMove->CanMove(Vector2(position.x, static_cast<int>(yPosition)), nextPosition))
	{
		xPosition = position.x;
	}

	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
}

void Player::Jump(float deltaTime)
{
	if (!isJumping)
		return;

	// 프레임마다 중력(가속도)을 속도에 누적한다.
	// velocityY는 시간이 지날수록 증가하며(아래 방향), 상승 중(음수)에는 0으로 수렴하고
	// 0을 지나 양수가 되면 자연스럽게 하강으로 전환된다.
	velocityY += gravity * deltaTime;

	// 위치 업데이트.
	yPosition += velocityY * deltaTime;

	// 충돌 체크를 위한 다음 프레임의 좌측/우측 위치 계산.
	Vector2 nextLeftPosition = Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition));
	Vector2 nextRightPosition = Vector2(static_cast<int>(xPosition + width - 1), static_cast<int>(yPosition));

	// 지면에 닿으면 위치/속도를 보정하고 점프 상태를 해제한다.
	if (!canPlayerMove->CanMove(position, nextLeftPosition) || !canPlayerMove->CanMove(Vector2(position.x + width - 1, position.y), nextRightPosition))
	{
		if (velocityY > 0.0f)
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

void Player::Fall()
{
	if (isJumping)
		return;

	Vector2 LeftDownPosition = Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition + 1));
	Vector2 RightDownPosition = Vector2(static_cast<int>(xPosition + width - 1), static_cast<int>(yPosition + 1));
	bool isGround = canPlayerMove->IsOnGround(LeftDownPosition) || canPlayerMove->IsOnGround(RightDownPosition);
	if (!isGround)
	{
		isJumping = true;
		velocityY = 0.0f;
		yPosition = position.y;
	}
}
