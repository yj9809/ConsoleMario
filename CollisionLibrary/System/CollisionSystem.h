#pragma once

#include "Util/CollisionTypes.h"

#include <vector>
#include <unordered_map>

class CollisionComponent;

// todo: 기본 구현은 완료하였고, 추가로 충돌 처리를 하면 된다.
class CollisionSystem
{
	using CollisionCallBack = void(*)(void* user, const CollisionEvent& e);
	struct CollisionListener
	{
		void* user = nullptr;
		CollisionCallBack callback = nullptr;
	};
private:
	struct CollisionData
	{
	public:
		// ID는 0이 유지되면 미할당 상태.
		CollisionID id = 0;
		bool enabled = true;
		
		const Position* positionPtr = nullptr;

		CollisionLayer layer = CollisionLayer::None;
		CollisionListener listener;
		LayerMask mask = 0;
		bool trigger = false;
		CollisionBox box{};
	};

	struct PairKey
	{
		CollisionID id;
		CollisionID otherID;

		bool operator==(const PairKey& other) const
		{
			return id == other.id && otherID == other.otherID;
		}
	};

	struct PairKeyHash
	{
		std::size_t operator()(const PairKey& key) const
		{
			return static_cast<std::size_t>((static_cast<std::uint64_t>(key.id) << 32) | static_cast<std::uint64_t>(key.otherID));
		}
	};

public:
	// CollisionSystem 등록 후 CollisionID 반환.
	// 성공 시: CollisionComponent 내부에 ID를 설정하고, 같은 값이 반환.
	// 실패 시: 0을 반환하고, CollisionID는 미할당 상태로 유지.
	// Component가 이미 할당된 상태인 경우 0을 반환.
	// 외부에서 ID를 관리하지 않고 Component 클래스 GetCollisionID()로 확인 가능.
	CollisionID Register(CollisionComponent& component, const Position* position);

	// 등록된 Component를 CollisionSystem에서 해제.
	// 시스템에 등록되어 있는 데이터는 enabled = false 처리.
	// CollisionComponent의 ID는 0으로 처리.
	void Unregister(CollisionComponent& component);

	// 등록된 CollisionComponent 설정 갱신.
	// 등록되지 않은 컴포넌트는 아무 동작도 하지 않음.
	void UpdateCollisionSetting(const CollisionComponent& component);	

	// 이벤트용
	std::vector<CollisionEvent> Step();

	// 쿼리용
	bool CanMove(const CollisionComponent& component, const Position& nextPos) const;

	void SetListener(CollisionID id, void* user, CollisionCallBack callback);
	void* GetListener(CollisionID id);
	void ClearListener(CollisionID id);

	CollisionLayer GetLayer(CollisionID id);

private:
	// CollisionID로 시스템 내부 데이터를 조회.
	// 유효하지 않을 경우 nullptr 반환.
	CollisionData* GetData(CollisionID id);

	// 내부 구현용: CollisionID 기반
	void UpdateCollisionSetting(CollisionID id, const CollisionComponent& component);

	bool CanCollide(const CollisionData& a, const CollisionData& b) const;
	bool CanCollide(const CollisionComponent& a, const CollisionData& b) const;
	bool IsOverlap(const Position& aPos, const CollisionBox& aBox, const Position& bPos, const CollisionBox& bBox) const;

	Position GetPosition(const CollisionData& data) const;

	PairKey MakePairKey(CollisionID id1, CollisionID id2) const;

private:
	// 등록된 CollisionComponent 목록.
	std::vector<CollisionData> collisionDatas;

	std::unordered_map<PairKey, bool, PairKeyHash> previousPairs;
};

