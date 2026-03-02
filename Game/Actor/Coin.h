#pragma once

#include "Actor/Actor.h"
#include "Component/CollisionComponent.h"

using namespace Wanted;

class Coin : public Actor
{
	RTTI_DECLARATIONS(Coin, Actor)

public: 
	Coin(const Vector2& position);
	
	void SetIsDestroyed();
	inline bool GetIsDestroyed() const { return isDestroyed; }

private:
	virtual void OnDestroy() override;

	bool isDestroyed = false;

	CollisionComponent collisionComponent;

	Position collisionPosition;
};

