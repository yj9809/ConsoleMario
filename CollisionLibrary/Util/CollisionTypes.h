#pragma once

#include <cstdint>

// 레이어 마스크와 충돌 ID를 나타내는 타입 정의.
using LayerMask = std::uint32_t;
using CollisionID = std::uint32_t;

// Layer의 값은 LayerMask 비트 인덱스로 사용.
// None은 레이어 없음을 나타내며, 비트 마스크 변환 시 0을 반환.
// Player(1) -> 1 << 1, Enemy(2) -> 1 << 2
enum class CollisionLayer : std::uint32_t
{
	None = 0,
	Player = 1,
	Enemy = 2,
	Item = 3,
	Platform = 4,
};

// Layer에 해당하는 단일 비트 마스크 생성.
// 충돌 검사 시 상대 레이어를 가져와 GetLayerMask로 변환한 후, 자신의 CollisionMask와 AND 연산하여 충돌 여부 판단.
// 32비트 이상 레이어는 지원하지 않으므로, 0을 반환하여 충돌 검사에서 무시한다 (32비트 마스크 범위 초과).
constexpr LayerMask GetLayerMask(CollisionLayer layer)
{
	const auto layerValue = static_cast<std::uint32_t>(layer);
	return (layer == CollisionLayer::None || layerValue >= 32u) ? LayerMask{ 0 } : LayerMask{ 1 } << layerValue;
}


// 충돌 영역을 나타내는 구조체.
struct CollisionBox
{
public:
	std::uint32_t width = 0;
	std::uint32_t height = 0;
}; 

struct Position
{
public:
	Position() = default;
	Position(int x, int y) { this->x = x, this->y = y; }

	int x = 0;
	int y = 0;
};

enum class EventType
{
	Enter,
	Stay,
	Exit
};

struct CollisionEvent
{
	EventType type;
	CollisionID id;
	CollisionID otherID;
	bool isTrigger;
};
