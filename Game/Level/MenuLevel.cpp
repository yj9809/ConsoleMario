#include "MenuLevel.h"
#include "Manager/ScreenManager.h"
#include "Core/Input.h"
#include "Render/Renderer.h"

MenuLevel::MenuLevel()
{
	// 메뉴 아이템 생성.
	items.emplace_back(new MenuItem(
		"Resume Game",
		[]()
		{
			// 메뉴 토글 함수 호출.
			ScreenManager::Get().ToggleMenu();
		}
	));

	items.emplace_back(new MenuItem(
		"Quit Game",
		[]()
		{
			// 게임 종료.
			ScreenManager::Get().QuitEngine();
		}
	));
}

MenuLevel::~MenuLevel()
{
	// 메뉴 아이템 제거.
	for (MenuItem*& item : items)
	{
		SafeDelete(item);
	}

	items.clear();
}

void MenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	Renderer::Get().SetCameraPosition(0, 30);

	// 입력 처리 (방향키 위/아래키, 엔터키, ESC키).
	// 배열 길이.
	static int length = static_cast<int>(items.size());
	if (Input::Get().GetKeyDown(VK_UP))
	{
		// 인덱스 돌리기 (위 방향으로).
		currentIndex = (currentIndex - 1 + length) % length;
	}

	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		// 인덱스 돌리기.
		currentIndex = (currentIndex + 1) % length;
	}

	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		// 메뉴 아이템이 저장한 함수 포인터 호출.
		items[currentIndex]->onSelected();
	}

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		// 메뉴 토글.
		ScreenManager::Get().ToggleMenu();

		// 인덱스 초기화.
		currentIndex = 0;
	}
}

void MenuLevel::Draw()
{
	Renderer::Get().Submit("Console Mario", Vector2::Zero);

	// 메뉴 아이템 출력.
	for (int ix = 0; ix < static_cast<int>(items.size()); ++ix)
	{
		// 아이템 색상 확인 (선택됐는지 여부).
		Color textColor =
			(ix == currentIndex) ? selectedColor : unselectedColor;

		Renderer::Get().Submit(
			items[ix]->text,
			Vector2(0, 2 + ix),
			textColor
		);
	}
}
