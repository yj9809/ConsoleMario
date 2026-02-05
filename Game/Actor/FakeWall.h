#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class FakeWall : public Actor
{
	RTTI_DECLARATIONS(FakeWall, Actor)

public:
	FakeWall(const Vector2& position);
};

