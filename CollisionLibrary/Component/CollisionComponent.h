#pragma once

#include "Util/CollisionTypes.h"


#include <cstdint>
#include <initializer_list>

class CollisionSystem;

class CollisionComponent
{
public:
	using PositionProvider = void(*)(void* user, float& outX, float& outY);

public:
	CollisionComponent() = default;
	CollisionComponent(
		const bool isTrigger, 
		const CollisionLayer layer,
		const LayerMask mask, 
		void* positionUser, 
		PositionProvider positionProvider,
		std::uint32_t width, 
		std::uint32_t height);

	void OnEnable(CollisionSystem& system, const Position* position);

	void SetPositionProvider(void* user, PositionProvider provider)
	{
		posUser = user;
		posProvider = provider;
	}

	void OnDisable(CollisionSystem& system);

	CollisionID GetColliderID() const { return collisionID; }
	void SetColliderID(CollisionID id) { collisionID = id; }

	// IsTrigger가 True인 경우 물리 충돌은 발생하지 않지만, 충돌 이벤트는 발생한다. (예: 아이템 획득, 트랩 발동 등)
	bool IsTrigger() const { return isTrigger; }
	void SetTrigger(bool trigger) { isTrigger = trigger; }

	CollisionLayer GetCollisionLayer() const { return collisionLayer; }
	void SetCollisionLayer(CollisionLayer layer) { collisionLayer = layer; }

	LayerMask GetCollisionMask() const { return mask; }
	void SetCollisionMask(LayerMask mask) { this->mask = mask; }

	void SetBox(std::uint32_t width, std::uint32_t height)
	{
		box.width = width;
		box.height = height;
	}
	const CollisionBox& GetBox() const { return box; }

	inline LayerMask MakeMask(std::initializer_list<CollisionLayer> layers)
	{
		LayerMask mask = 0;
		for (auto l : layers)
		{
			mask |= GetLayerMask(l);
		}
		return mask;
	}

private:
	// Collision을 구분하기 위한 ID (0이면 미할당)
	CollisionID collisionID = 0;

	// 충돌 판정을 위한 트리거.
	bool isTrigger = false;

	// 충돌 판정 시 레이어를 구분하기 위한 변수.
	CollisionLayer collisionLayer = CollisionLayer::None;

	// 충돌 판정 시 어떤 레이어와 충돌할지 구분하기 위한 변수.
	LayerMask mask = 0;

	// 충돌 판정을 위한 박스 정보.
	CollisionBox box{};

	void* posUser = nullptr;
	PositionProvider posProvider = nullptr;
};

