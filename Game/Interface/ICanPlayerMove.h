#pragma once

#include "Math/Vector2.h"

// 플레이어가 이동할 위치를 판단해주는 인터페이스.
// C++ 인터페이스 = 순수 가상함수를 가지는 클래스.
class ICanPlayerMove
{
public:
	// 플레이어가 이동 가능한지 문의할 때 사용할 함수.
	// playerPosition: 플레이어의 현재 위치.
	// nextPosition: 플레이어가 이동하려는 위치.
	virtual bool CanMove(const Wanted::Vector2& nextPosition) = 0;

	// 플레이어가 바닥을 밟고 있는지 문의할 때 사용할 함수.
	// playerDownPosition: 플레이어의 발 밑 위치.
	virtual bool IsOnGround(const Wanted::Vector2& playerDownPosition) = 0;

	// 적 다음 위치에 바닥이 있는지 문의할 때 사용할 함수.
	virtual bool IsNextToGround(const Wanted::Vector2& enemyNextPosition) = 0;
};