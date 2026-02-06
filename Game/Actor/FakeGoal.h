#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class FakeGoal : public Actor
{
	RTTI_DECLARATIONS(FakeGoal, Actor)

public:
	FakeGoal(const Vector2& position);
};

