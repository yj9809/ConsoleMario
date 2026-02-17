#include "Enemy.h"
#include "Manager/ScreenManager.h"
#include "Level/GameLevel.h"

#define RiGHT_POSITION_X static_cast<int>(xPosition + width - 1)
#define LEFT_POSITION_X static_cast<int>(xPosition)
#define WALL_CHECK_POSITION_Y static_cast<int>(position.y + ((height - 1) / 2))
#define NEXT_GROUND_CHECK_POSITION_Y static_cast<int>(position.y + height)

Enemy::Enemy(const Vector2& position)
	: super("EEE\nEEE\nE E", position, Color::Blue), xPosition(static_cast<float>(position.x))
{
	sortingOrder = 7;
	auto& cs = ScreenManager::Get().GetCollisionSystem();
	component = CollisionComponent(false, CollisionLayer::Enemy, GetLayerMask(CollisionLayer::Player) | GetLayerMask(CollisionLayer::Platform), width, height);
	collisionPosition.x = position.x;
	collisionPosition.y = position.y;
	component.OnEnable(cs, &collisionPosition);
	cs.SetListener(component.GetColliderID(), this, nullptr);
}

Enemy::~Enemy()
{
	auto& cs = ScreenManager::Get().GetCollisionSystem();
	cs.ClearListener(component.GetColliderID());
	component.OnDisable(cs);

	super::OnDestroy();
}

void Enemy::BeginPlay()
{
	super::BeginPlay();

	if (!canPlayerMove)
	{
		canPlayerMove = dynamic_cast<ICanPlayerMove*>(GetOwner());
	}
	yPosition = position.y;
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (isDestroyed)
	{
		
		DestroyMotion(deltaTime);
		return;
	}
	Move(deltaTime);
}

void Enemy::Move(float deltaTime)
{	
	int wallY = static_cast<int>(position.y + ((height - 1) / 2));
	int groundY = static_cast<int>(position.y + height);

	if (!WallCheck(RiGHT_POSITION_X, WALL_CHECK_POSITION_Y) || !WallCheck(LEFT_POSITION_X, WALL_CHECK_POSITION_Y)
		|| !GroundCheck(RiGHT_POSITION_X, NEXT_GROUND_CHECK_POSITION_Y) || !GroundCheck(LEFT_POSITION_X, NEXT_GROUND_CHECK_POSITION_Y))
	{
		moveDirection *= -1;
	}
	
	xPosition += (moveSpeed * deltaTime) * moveDirection;

	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(position.y)));
	collisionPosition.x = position.x;
	collisionPosition.y = position.y;
}

void Enemy::DestroyMotion(float deltaTime)
{
	velocityY += gravity * deltaTime;
	yPosition += velocityY * deltaTime;
	xPosition += velocityX * deltaTime;

	if(position.y > ScreenManager::Get().GetHeight())
	{
		Destroy();
	}

	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));
	collisionPosition.x = static_cast<int>(xPosition);
	collisionPosition.y = static_cast<int>(position.y);
}

bool Enemy::WallCheck(int x, int y)
{
	Vector2 nextUpPosition = Vector2(x, y);
	Vector2 nextDownPosition = Vector2(x, y);

	if (!canPlayerMove->CanMove(nextUpPosition) || !canPlayerMove->CanMove(nextDownPosition))
	{
		return false;
	}

	return true;
}

bool Enemy::GroundCheck(int x, int y)
{
	Vector2 nextGroundPosition = Vector2(x, y);
	if (canPlayerMove->IsNextToGround(nextGroundPosition))
	{
		return true;
	}
	return false;
}
