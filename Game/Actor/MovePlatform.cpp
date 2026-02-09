#include "MovePlatform.h"
#include "Actor/Player.h"
#include "Level/GameLevel.h"

#include <iostream>

MovePlatform::PlayerPosition::PlayerPosition(const Player& player, const Vector2& platformPosition, int width)
{
	pLeft = player.GetPosition().x;
	pRight = pLeft + player.GetWidth() - 1;
	pBottom = player.GetPosition().y + player.GetHeight();
	platLeft = platformPosition.x;
	platRight = platLeft + width - 1;
	platTop = platformPosition.y;
}

bool MovePlatform::PlayerPosition::IsOnTop()
{
	return (pBottom == platTop) && (pRight >= platLeft) && (pLeft <= platRight);
}

MovePlatform::MovePlatform(const Vector2& position, const Vector2& endPosition)
	: super("=======\n=======", position, Color::Brick), startPosition(position), endPosition(endPosition)
{
	this->endPosition.x -= width - 1;
}

void MovePlatform::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	Vector2 preVector = GetPosition();
	
	Player* player = GetOwner()->As<GameLevel>()->GetPlayer();
	PlayerPosition pPosition(*player, preVector, width);

	t += moveSpeed * deltaTime * moveDirection;

	if (t > 1.0f)
	{
		t = 1.0f;
		moveDirection *= -1;
	}
	else if(t < 0.0f)
	{
		t = 0.0f;
		moveDirection *= -1;
	}

	float x = startPosition.x + (endPosition.x - startPosition.x) * t;
	float y = startPosition.y + (endPosition.y - startPosition.y) * t;

	if (t <= 0.0f) 
		SetPosition(startPosition);
	else if (t >= 1.0f) 
		SetPosition(endPosition);
	else 
		SetPosition(Vector2((int)std::round(x), (int)std::round(y)));

	Vector2 delta = GetPosition() - preVector;

	// 플레이어가 플랫폼 위에 올라타 있는지 확인.
	if (pPosition.IsOnTop())
	{
		player->AddPlatformMove(delta);
	}
}