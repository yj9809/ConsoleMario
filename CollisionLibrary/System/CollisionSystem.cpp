#include "CollisionSystem.h"
#include "Component/CollisionComponent.h"

CollisionID CollisionSystem::Register(CollisionComponent& component, const Position* position)
{
	// 이미 등록된 CollisionComponent인 경우 0 반환.
	if (component.GetColliderID() != 0)
	{
		return 0;
	}

	// 레이어가 None이거나 충돌 박스의 크기가 0인 경우 등록하지 않고 0 반환.
	const CollisionBox& box = component.GetBox();
	if (component.GetCollisionLayer() == CollisionLayer::None || box.width == 0 || box.height == 0)
	{
		// ColliderID를 0으로 설정하여 미할당 상태로 유지.
		component.SetColliderID(0);
		return 0;
	}

	// 새로운 CollisionID는 Index + 1로 할당 (0은 미할당 상태를 나타내므로).
	CollisionID newID = static_cast<CollisionID>(collisionDatas.size() + 1);

	// CollisionData를 생성 및 배열에 추가.
	CollisionData newData;
	newData.id = newID;
	newData.positionPtr = position;
	newData.layer = component.GetCollisionLayer();
	newData.mask = component.GetCollisionMask();
	newData.trigger = component.IsTrigger();
	newData.box = component.GetBox();

	collisionDatas.emplace_back(newData);

	// CollisionComponent에 새로 할당된 CollisionID 설정.
	component.SetColliderID(newID);

	return newID;
}

void CollisionSystem::Unregister(CollisionComponent& component)
{
	CollisionID id = component.GetColliderID();
	if (id != 0)
	{
		CollisionData* data = GetData(id);
		if (data)
		{
			// CollisionData를 비활성화하여 사실상 삭제 처리 (ID는 유지).
			data->enabled = false;
		}
		// CollisionComponent의 ColliderID도 초기화하여 미할당 상태로 설정.
		component.SetColliderID(0);
	}
}

void CollisionSystem::UpdateCollisionSetting(const CollisionComponent& component)
{
	CollisionID id = component.GetColliderID();
	if (id == 0) return;
	UpdateCollisionSetting(id, component);
}

std::vector<CollisionEvent> CollisionSystem::Step()
{
	auto Dispatch = [this](const CollisionEvent& e)
		{
			CollisionData* data = GetData(e.id);
			if (!data || !data->listener.callback)
				return;
			data->listener.callback(data->listener.user, e);
		};

	std::vector<CollisionEvent> events;
	std::unordered_map<PairKey, bool, PairKeyHash> currentPairs;
	for (int i = 0; i < collisionDatas.size(); ++i)
	{
		auto& data1 = collisionDatas[i];
		if (!data1.enabled || !data1.positionPtr)
			continue;

		for (int j = i + 1; j < collisionDatas.size(); ++j)
		{
			auto& data2 = collisionDatas[j];
			if (!data2.enabled || !data2.positionPtr)
				continue;
			if (!CanCollide(data1, data2))
				continue;
			if (!IsOverlap(GetPosition(data1), data1.box, GetPosition(data2), data2.box))
				continue;

			PairKey pairKey = MakePairKey(data1.id, data2.id);
			bool isTrigger = data1.trigger || data2.trigger;

			currentPairs.emplace(pairKey, isTrigger);

			if (previousPairs.find(pairKey) == previousPairs.end())
			{
				CollisionEvent e1{ EventType::Enter, data1.id, data2.id, isTrigger };
				events.push_back(e1);
				Dispatch(e1);

				CollisionEvent e2{ EventType::Enter, data2.id, data1.id, isTrigger };
				events.push_back(e2);
				Dispatch(e2);
			}
			else
			{
				CollisionEvent e1{ EventType::Stay, data1.id, data2.id, isTrigger };
				events.push_back(e1);
				Dispatch(e1);

				CollisionEvent e2{ EventType::Stay, data2.id, data1.id, isTrigger };
				events.push_back(e2);
				Dispatch(e2);
			}
		}
	}
	for (const auto& [key, wasTrigger] : previousPairs)
	{
		if (currentPairs.find(key) != currentPairs.end())
			continue;

		CollisionEvent e1{ EventType::Exit, key.otherID, key.id, wasTrigger };
		events.push_back(e1);
		Dispatch(e1);

		CollisionEvent e2{ EventType::Exit, key.id, key.otherID, wasTrigger };
		events.push_back(e2);
		Dispatch(e2);
	}
	previousPairs.swap(currentPairs);
	return events;
}

bool CollisionSystem::CanMove(const CollisionComponent& component, const Position& nextPos) const
{
	const CollisionBox& box = component.GetBox();
	if (component.GetCollisionLayer() == CollisionLayer::None)
		return true;

	const CollisionID selfID = component.GetColliderID();

	for (const auto& other : collisionDatas)
	{
		if (!other.enabled || !other.positionPtr)
			continue;
		if (selfID != 0 && other.id == selfID)
			continue;

		if (component.IsTrigger() || other.trigger)
			continue;

		if (!CanCollide(component, other))
			continue;

		if (IsOverlap(nextPos, box, GetPosition(other), other.box))
			return false;
	}
	return true;
}

void CollisionSystem::SetListener(CollisionID id, void* user, CollisionCallBack callback)
{
	CollisionData* data = GetData(id);
	if (!data)
		return;

	data->listener.user = user;
	data->listener.callback = callback;
}

void* CollisionSystem::GetListener(CollisionID id)
{
	CollisionData* data = GetData(id);
	if (!data)
		return nullptr;
	return data->listener.user;
}

void CollisionSystem::ClearListener(CollisionID id)
{
	CollisionData* data = GetData(id);
	if (!data)
		return;

	data->listener.user = nullptr;
	data->listener.callback = nullptr;
}

CollisionLayer CollisionSystem::GetLayer(CollisionID id)
{
	CollisionData* data = GetData(id);
	return data ? data->layer : CollisionLayer::None;
}

void CollisionSystem::UpdateCollisionSetting(CollisionID id, const CollisionComponent& component)
{
	CollisionData* data = GetData(id);
	if (data)
	{
		data->layer = component.GetCollisionLayer();
		data->mask = component.GetCollisionMask();
		data->trigger = component.IsTrigger();
		data->box = component.GetBox();
	}
}

bool CollisionSystem::CanCollide(const CollisionData& data1, const CollisionData& data2) const
{
	if (data1.layer == CollisionLayer::None || data2.layer == CollisionLayer::None)
		return false;

	return (data1.mask & GetLayerMask(data2.layer)) || (data2.mask & GetLayerMask(data1.layer));
}

bool CollisionSystem::CanCollide(const CollisionComponent& data1, const CollisionData& data2) const
{
	if (data1.GetCollisionLayer() == CollisionLayer::None || data2.layer == CollisionLayer::None)
		return false;

	return (data1.GetCollisionMask() & GetLayerMask(data2.layer)) || (data2.mask & GetLayerMask(data1.GetCollisionLayer()));
}

bool CollisionSystem::IsOverlap(const Position& data1Pos, const CollisionBox& data1Box, const Position& data2Pos, const CollisionBox& data2Box) const
{
	const int leftData1 = data1Pos.x;
	const int rightData1 = data1Pos.x + static_cast<int>(data1Box.width);
	const int topData1 = data1Pos.y;
	const int bottomData1 = data1Pos.y + static_cast<int>(data1Box.height);

	const int leftData2 = data2Pos.x;
	const int rightData2 = data2Pos.x + static_cast<int>(data2Box.width);
	const int topData2 = data2Pos.y;
	const int bottomData2 = data2Pos.y + static_cast<int>(data2Box.height);

	return (leftData1 < rightData2 && rightData1 > leftData2 && topData1 < bottomData2 && bottomData1 > topData2);
}

Position CollisionSystem::GetPosition(const CollisionData& data) const
{
	if (data.positionPtr)
	{
		return *(data.positionPtr);
	}
	return { 0, 0 };
}

CollisionSystem::PairKey CollisionSystem::MakePairKey(CollisionID id1, CollisionID id2) const
{
	return (id1 < id2) ? PairKey{ id1, id2 } : PairKey{ id2, id1 };
}

CollisionSystem::CollisionData* CollisionSystem::GetData(CollisionID id)
{
	if (id == 0 || id > static_cast<std::uint32_t>(collisionDatas.size()))
	{
		// 유효하지 않은 ID이므로 nullptr 반환.
		return nullptr;
	}

	CollisionData& data = collisionDatas[id - 1];
	if (!data.enabled)
	{
		// 해당 ID는 비활성화된 데이터이므로 nullptr 반환.
		return nullptr;
	}

	return &data;
}
