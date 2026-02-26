#include "CollisionComponent.h"
#include "System/CollisionSystem.h"

CollisionComponent::CollisionComponent(const bool isTrigger, const CollisionLayer layer, const LayerMask mask, void* positionUser, PositionProvider positionProvider, std::uint32_t width, std::uint32_t height)
	: isTrigger(isTrigger), collisionLayer(layer), mask(mask), box{width, height}, posUser(positionUser), posProvider(positionProvider)
{
}

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
