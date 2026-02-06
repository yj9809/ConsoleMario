#include "Player.h"
#include "Engine/Engine.h"
#include "Core/Input.h"
#include "Level/GameLevel.h"

#define RIGHT_KEY (Input::Get().GetKey(VK_RIGHT))
#define LEFT_KEY (Input::Get().GetKey(VK_LEFT))
#define SPACE_DOWN (Input::Get().GetKeyDown(VK_SPACE))
#define ESC_DOWN (Input::Get().GetKeyDown(VK_ESCAPE))

Player::Player()
	: super(" P \n/|\\\n/ \\", Vector2::SpawnPoint, Color::Red), yPosition(position.y)
{
	sortingOrder = 10;
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if(currentState == State::Clear)
	{
		ClearMove(deltaTime);
		return;
	}

	if (!canPlayerMove)
	{
		canPlayerMove = dynamic_cast<ICanPlayerMove*>(GetOwner());
	}

	Move(deltaTime);

	Fall();

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

	Vector2 nextUpPosition = Vector2(static_cast<int>(xPosition + width - 1), static_cast<int>(yPosition));
	Vector2 nextDownPosition = Vector2(static_cast<int>(xPosition + width - 1), static_cast<int>(yPosition + height - 1));
	if (!canPlayerMove->CanMove(position, nextUpPosition) || !canPlayerMove->CanMove(position, nextDownPosition))
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

	Vector2 nextUpPosition = Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition));
	Vector2 nextDownPosition = Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition + height - 1));
	if (!canPlayerMove->CanMove(position, nextUpPosition) || !canPlayerMove->CanMove(position, nextDownPosition))
	{
		xPosition = position.x;
	}

	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
}

void Player::Jump(float deltaTime)
{
	if (isGround && SPACE_DOWN)
	{
		currentState = State::Jumping;
		// 점프 시작 시 속도 설정 (전제 조건: JumpSpeed > 0).
		// Console 특성 상 상승은 음수 방향이므로 음수 값 할당.
		velocityY = -jumpSpeed;
	}

	if (currentState == State::Idle)
		return;

	// 프레임마다 중력(가속도)을 속도에 누적한다.
	// velocityY는 시간이 지날수록 증가하며(아래 방향), 상승 중(음수)에는 0으로 수렴하고
	// 0을 지나 양수가 되면 자연스럽게 하강으로 전환된다.
	if (currentState == State::Jumping)
	{
		velocityY += gravity * deltaTime;
	}
	else
	{
		velocityY += fallGravity * deltaTime;
	}


	// 상승에서 하강으로 전환.
	if (currentState == State::Jumping && velocityY >= 0.0f)
	{
		currentState = State::Falling;
	}

	// 위치 업데이트.
	yPosition += velocityY * deltaTime;

	// 충돌 체크를 위한 다음 프레임의 좌측/우측 위치 계산.
	Vector2 headLeft = Vector2((int)xPosition, (int)(yPosition));
	Vector2 headRight = Vector2((int)(xPosition + width - 1), (int)(yPosition));
	Vector2 pootLeft = Vector2((int)xPosition, (int)(yPosition + height - 1));
	Vector2 pootRigth = Vector2((int)(xPosition + width - 1), (int)(yPosition + height - 1));

	bool hitCeiling = !canPlayerMove->CanMove(position, headLeft) || !canPlayerMove->CanMove(Vector2(position.x + width - 1, position.y), headRight)
		|| !canPlayerMove->CanMove(position, pootLeft) || !canPlayerMove->CanMove(position, pootRigth);

	// 지면에 닿으면 위치/속도를 보정하고 점프 상태를 해제한다.
	if (hitCeiling)
	{
		if (velocityY > 0.0f)
		{
			// 땅에 착지했으므로 위치 보정.
			yPosition = position.y;
			// 속도 초기화.
			velocityY = 0.0f;
			// 착지 후 점프 플래그 초기화.
			currentState = State::Idle;
		}
		else
		{
			// 천장에 부딪혔으므로 위치 보정.
			yPosition = position.y;
			// 속도 초기화.
			velocityY = 0.0f;
			// 천장에 부딪혔으므로 하강 상태로 전환.
			currentState = State::Falling;
		}
	}

	// 위치 업데이트.
	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
}

void Player::Fall()
{
	// 바닥 체크.
	Vector2 LeftDownPosition = Vector2(xPosition, (yPosition + height));
	Vector2 RightDownPosition = Vector2(xPosition + width - 1, (yPosition + height));
	isGround = canPlayerMove->IsOnGround(LeftDownPosition) || canPlayerMove->IsOnGround(RightDownPosition);
	if (!isGround && currentState == State::Idle)
	{ 
		// 바닥에서 벗어났으므로 낙하 상태로 전환.
		currentState = State::Falling;
		velocityY = 0.0f;
	}

	// 플레이어 포지션이 화면 아래로 벗어났는지 체크.
	if (position.y > Engine::Get().GetHeight())
	{
		// 라이프가 남아있는지 체크.
		if (GetOwner()->As<GameLevel>()->GetLife() > 0)
		{
			// 라이프 감소 및 초기 위치로 리스폰.
			GetOwner()->As<GameLevel>()->SetLife();
			RespawnAt(Vector2::SpawnPoint);
		}
		else
		{
			QuitGame();
		}
	}
}

// Todo: 현재는 낙사만 체크하지만, 추후 몬스터와 충돌 시 리스폰 처리도 이 함수에서 담당할 수 있다.
inline void Player::RespawnAt(const Vector2& pos)
{
	xPosition = pos.x;
	yPosition = pos.y;
	SetPosition(Vector2::SpawnPoint);

	currentState = State::Idle;
}

inline void Player::SetWeight(float& weight, float deltaTime)
{
	// 시간이 지날수록 가속도 증가.
	weight += deltaTime * 5.0f;
	// 가속도 최대치 설정.
	Util::Clamp(weight, 0.0f, 25.0f);
}

void Player::ClearMove(float deltaTime)
{
	if (yPosition < 27.0f) 
	{
		yPosition += moveSpeed * deltaTime;
	}
	else
	{
		xPosition += moveSpeed * deltaTime;
	}

	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
}

