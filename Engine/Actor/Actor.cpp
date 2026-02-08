#include "Actor.h"
#include "Util/Util.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"

#include <iostream>
#include <Windows.h>

namespace Wanted
{
	Actor::Actor(
		const char* image,
		const Vector2& position,
		Color color)
		: position(position), color(color)
	{
		// 문자열 복사.
		int length = static_cast<int>(strlen(image));
		for (int p = 0; p < length; ++p)
		{
			if (image[p] == '\n')
			{
				width = 0;
				height++;
				continue;
			}
			width++;
		}
		this->image = new char[length + 1];
		strcpy_s(this->image, length + 1, image);
	}

	Actor::~Actor()
	{
		// 메모리 해제.
		SafeDeleteArray(image);
	}

	void Actor::BeginPlay()
	{
		// 이벤트를 받은 후에는 플래그 설정.
		hasBeganPlay = true;
	}

	void Actor::Tick(float deltaTime)
	{
	}

	void Actor::Draw()
	{
		//Renderer::Draw(position, color, image);

		// 렌더러에 데이터 제출.
		Renderer::Get().Submit(image, position, color, sortingOrder);
	}

	void Actor::Destroy()
	{
		// 삭제 플래그 설정.
		destroyRequested = true;

		// 삭제 이벤트 호출.
		OnDestroy();
	}

	void Actor::OnDestroy()
	{

	}

	void Actor::QuitGame()
	{
		// 엔진 종료 요청.
		Engine::Get().QuitEngine();
	}

	bool Actor::TestIntersect(const Actor* const other)
	{
		const int xMin = position.x;
		const int xMax = position.x + width - 1;

		const int yMin = position.y;
		const int yMax = position.y + height - 1;

		const int otherXMin = other->position.x;
		const int otherXMax = other->position.x + other->width - 1;

		const int otherYMin = other->position.y;
		const int otherYMax = other->position.y + other->height - 1;

		// 완전히 오른쪽에 있는 경우.
		if (otherXMin > xMax) return false;
		// 완전히 왼쪽에 있는 경우.
		if (otherXMax < xMin) return false;
		// 완전히 아래에 있는 경우.
		if (otherYMin > yMax) return false;
		// 완전히 위에 있는 경우.
		if (otherYMax < yMin) return false;

		return true;
	}

	void Actor::SetPosition(const Vector2& newPosition)
	{
		// 렌더러에 빈칸 그리기 요청.
		//Renderer::Draw(position, ' ');

		// 변경하려는 위치가 현재 위치와 같으면 건너뜀.
		if (position == newPosition)
		{
			return;
		}

		// 새로운 위치 설정.
		position = newPosition;
	}
}