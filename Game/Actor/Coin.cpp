#include "Coin.h"
#include "Manager/ScreenManager.h"

Coin::Coin(const Vector2& position)
	: super(" C \nCCC\n C ", position, Color::Coin)
{
	auto& cs = ScreenManager::Get().GetCollisionSystem();
	auto mask = collisionComponent.MakeMask({ CollisionLayer::Player});

	collisionComponent = CollisionComponent(
		false,
		CollisionLayer::Item,
		mask,
		width,
		height
	);

	collisionPosition.x = position.x;
	collisionPosition.y = position.y;

	collisionComponent.OnEnable(cs, &collisionPosition);
	cs.SetListener(collisionComponent.GetColliderID(), this, nullptr);
}

void Coin::SetIsDestroyed()
{
	isDestroyed = true;
	auto& cs = ScreenManager::Get().GetCollisionSystem();
	cs.ClearListener(collisionComponent.GetColliderID());
	collisionComponent.OnDisable(cs);
}

void Coin::OnDestroy()
{
	auto& cs = ScreenManager::Get().GetCollisionSystem();
	if (collisionComponent.GetColliderID() != 0)
	{
		cs.ClearListener(collisionComponent.GetColliderID());
		collisionComponent.OnDisable(cs);
	}

	super::OnDestroy();
}