#include "CollisionComponent.h"
#include "System/CollisionSystem.h"

void CollisionComponent::OnEnable(CollisionSystem& system, const Position* position)
{
	if (collisionID != 0)
		return;

	CollisionID id = system.Register(*this, position);

	if (id != 0)
	{
		collisionID = id;
	}
}

void CollisionComponent::OnDisable(CollisionSystem& system)
{
	if (collisionID == 0)
		return;
	system.Unregister(*this);
	collisionID = 0;
}
