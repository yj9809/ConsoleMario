#include "Player.h"
#include "Manager/ScreenManager.h"
#include "Core/Input.h"
#include "Level/GameLevel.h"
#include "Actor/Enemy.h"

#define RIGHT_KEY (Input::Get().GetKey(VK_RIGHT))
#define LEFT_KEY (Input::Get().GetKey(VK_LEFT))
#define SPACE_DOWN (Input::Get().GetKeyDown(VK_SPACE))


#define HEAD_RIGHT Vector2((int)(xPosition + width - 1), (int)(yPosition))
#define HEAD_LEFT Vector2((int)(xPosition), (int)(yPosition))
#define POOT_RIGHT Vector2((int)(xPosition + width - 1), (int)(yPosition + height - 1))
#define POOT_LEFT Vector2((int)(xPosition), (int)(yPosition + height - 1))

Player::Player()
	: super(" P \n/|\\\n/ \\", Vector2::SpawnPoint, Color::Red), yPosition(position.y)
{
	sortingOrder = 10;
}

Player::~Player()
{
}

void Player::BeginPlay()
{
	if (!canPlayerMove)
	{
		canPlayerMove = dynamic_cast<ICanPlayerMove*>(GetOwner());
	}

	auto& cs = ScreenManager::Get().GetCollisionSystem();
	auto mask = collisionComponent.MakeMask({ CollisionLayer::Enemy, CollisionLayer::Item });

	collisionComponent = CollisionComponent(
		false,
		CollisionLayer::Player,
		mask,
		this,
		&Player::GetPosThunk,
		width,
		height
	);

	collisionComponent.OnEnable(cs, &collisionPosition);
	cs.SetListener(collisionComponent.GetColliderID(), this, &Player::OnCollisionThunk);
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if(currentState == State::Clear)
	{
		ClearMove(deltaTime);
		return;
	}

	if(currentState == State::Death)
	{
		// 사망 모션 처리.
		DeathMotion(deltaTime);
		return;
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

	Vector2 nextUpPosition = HEAD_RIGHT;
	Vector2 nextDownPosition = POOT_RIGHT;
	if (!canPlayerMove->CanMove(nextUpPosition) || !canPlayerMove->CanMove(nextDownPosition))
	{
		xPosition = (float)position.x;
	}

	if (xPosition + width > ScreenManager::Get().GetWidth())
	{
		xPosition = static_cast<float>(ScreenManager::Get().GetWidth() - width);
	}
	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
	SyncCollisionPosition();
}

void Player::MoveLeft(float deltaTime)
{
	xPosition -= (moveSpeed + weight) * deltaTime;

	float cameraX = static_cast<float>(GetOwner()->As<GameLevel>()->GetCameraXPosition());
	if (xPosition < cameraX)
	{
		xPosition = cameraX;
	}

	Vector2 nextUpPosition = HEAD_LEFT;
	Vector2 nextDownPosition = POOT_LEFT;
	if (!canPlayerMove->CanMove(nextUpPosition) || !canPlayerMove->CanMove(nextDownPosition))
	{
		xPosition = (float)position.x;
	}

	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
	SyncCollisionPosition();
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

	bool hitCeiling = !canPlayerMove->CanMove(headLeft) || !canPlayerMove->CanMove(headRight)
		|| !canPlayerMove->CanMove(pootLeft) || !canPlayerMove->CanMove(pootRigth);

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
	SyncCollisionPosition();
}

void Player::Fall()
{
	// 바닥 체크.
	Vector2 LeftDownPosition = Vector2((int)xPosition, (int)(yPosition + height));
	Vector2 RightDownPosition = Vector2((int)(xPosition + width - 1), (int)(yPosition + height));

	isGround = canPlayerMove->IsOnGround(LeftDownPosition) || canPlayerMove->IsOnGround(RightDownPosition);

	if (!isGround && currentState == State::Idle)
	{ 
		// 바닥에서 벗어났으므로 낙하 상태로 전환.
		currentState = State::Falling;
		velocityY = 0.0f;
	}

	// 플레이어 포지션이 화면 아래로 벗어났는지 체크.
	if (position.y > ScreenManager::Get().GetHeight())
	{
		currentState = State::Death;
	}
}

void Player::DeathMotion(float deltaTime)
{
	deathVelocityY += deathGravity * deltaTime;
	yPosition += deathVelocityY * deltaTime;
	xPosition += deathVelocityX * deltaTime;

	if (position.y > ScreenManager::Get().GetHeight())
	{
		currentState = State::Death;
	}

	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
	SyncCollisionPosition();
}

void Player::ResetPosition()
{
	currentState = State::Idle;
	SetPosition(Vector2::SpawnPoint);
	xPosition = (float)position.x;
	yPosition = (float)position.y;
}

void Player::AddPlatformMove(const Vector2& delta)
{
	xPosition += delta.x;
	yPosition += delta.y;
	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
	SyncCollisionPosition();
}

inline void Player::SetWeight(float& weight, float deltaTime)
{
	// 시간이 지날수록 가속도 증가.
	weight += deltaTime * 5.0f;
	// 가속도 최대치 설정.
	Util::Clamp(weight, 0.0f, 15.0f);
}

void Player::SyncCollisionPosition()
{
	collisionPosition.x = position.x;
	collisionPosition.y = position.y;
}

void Player::OnCollisionThunk(void* user, const CollisionEvent& e)
{
	auto* self = static_cast<Player*>(user);
	if (!self)
		return;

	// 이미 죽은 상태면 무시.
	if (self->GetState() == Player::State::Death)
		return;

	auto& cs = ScreenManager::Get().GetCollisionSystem();

	auto* other = static_cast<Actor*>(cs.GetListener(e.otherID));
	if (cs.GetLayer(e.otherID) == CollisionLayer::Enemy)
	{
		bool isFalling = self->currentState == State::Falling;
		auto* enemy = other->As<Enemy>();
		self->GetOwner()->As<GameLevel>()->OnPlayerHitEnemy(enemy, isFalling);
	}

	if (cs.GetLayer(e.otherID) == CollisionLayer::Item)
	{
		auto* coin = other->As<Coin>();
		self->GetOwner()->As<GameLevel>()->OnPlayerHitCoin(coin);
		return;
	}
}

void Player::GetPosThunk(void* user, float& outX, float& outY)
{
	auto* self = static_cast<Player*>(user);
	if (!self)
		return;
	outX = self->position.x;
	outY = self->position.y;
}

void Player::ClearMove(float deltaTime)
{
	if (yPosition < 26.0f) 
	{
		yPosition += moveSpeed * deltaTime;
	}
	else if (xPosition < ScreenManager::Get().GetWidth())
	{
		xPosition += moveSpeed * deltaTime;
	}
	else if(xPosition >= ScreenManager::Get().GetWidth())
	{
		currentState = State::Clear;
	}

	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
	SyncCollisionPosition();
}

void Player::SetDeath(State state, bool left)
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

