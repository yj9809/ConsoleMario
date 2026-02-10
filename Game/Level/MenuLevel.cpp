#include "MenuLevel.h"
#include "Manager/ScreenManager.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Level/GameLevel.h"

#define X_POSITION(x) (60 - (x / 2))

MenuLevel::MenuLevel()
{
	// 메뉴 아이템 생성.
	items.emplace_back(new MenuItem(
		"Game Start",
		[]()
		{
			// 게임 레벨로 전환.
			ScreenManager::Get().currentScreenType = ScreenType::Game;
			ScreenManager::Get().ToggleMenu((int)ScreenType::Game);
			ScreenManager::Get().GetGameLevel()->Init();
		}
	));

	items.emplace_back(new MenuItem(
		"Resume Game",
		[]()
		{
			// 메뉴 토글 함수 호출.
			ScreenManager::Get().ToggleMenu((int)ScreenType::Game);
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

	items.emplace_back(new MenuItem(
		"PRESS ENTER...",
		[]()
		{
			if (ScreenManager::Get().currentScreenType == ScreenType::MapClear)
			{
				ScreenManager::Get().GetGameLevel()->SetClearFlag(true);
			}
			else if (ScreenManager::Get().currentScreenType == ScreenType::GameClear)
			{
				ScreenManager::Get().QuitEngine();
				return;
			}

			ScreenManager::Get().currentScreenType = ScreenType::Game;
			ScreenManager::Get().ToggleMenu((int)ScreenType::Game);
		}
	));

	items.emplace_back(new MenuItem(
		"Title Screen",
		[]()
		{
			ScreenManager::Get().currentScreenType = ScreenType::Title_Menu;
			ScreenManager::Get().ToggleMenu((int)ScreenType::Title_Menu);
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

void MenuLevel::Menu()
{
	// 입력 처리 (방향키 위/아래키, 엔터키, ESC키).
	// 배열 길이.
	static int length = static_cast<int>(displayItems.size());

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
		displayItems[currentIndex]->onSelected();
	}

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		// 메뉴 토글.
		ScreenManager::Get().ToggleMenu((int)ScreenType::Game);

		// 인덱스 초기화.
		currentIndex = 0;
	}
}

void MenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	displayItems.clear();

	if (ScreenManager::Get().currentScreenType == ScreenType::Title_Menu)
	{
		displayItems.emplace_back(items[0]);
		displayItems.emplace_back(items[2]);
	}
	else if (ScreenManager::Get().currentScreenType == ScreenType::Game)
	{
		displayItems.emplace_back(items[1]);
		displayItems.emplace_back(items[2]);
	}
	else if (ScreenManager::Get().currentScreenType == ScreenType::Respawn)
	{
		displayItems.emplace_back(items[3]);
	}
	else if (ScreenManager::Get().currentScreenType == ScreenType::MapClear ||
		ScreenManager::Get().currentScreenType == ScreenType::GameClear)
	{
		displayItems.emplace_back(items[3]);
	}
	else if(ScreenManager::Get().currentScreenType == ScreenType::GameOver)
	{
		displayItems.emplace_back(items[4]);
		displayItems.emplace_back(items[2]);
	}

	if (currentIndex >= (int)displayItems.size()) currentIndex = 0;

	Renderer::Get().SetCameraPosition(0, 120);

	Menu();
}

void MenuLevel::Draw()
{
	if (ScreenManager::Get().currentScreenType == ScreenType::Title_Menu)
	{
		Renderer::Get().Submit(
			"   ******                                      **           ****     ****                  **         \n"
			"  **    **                                     **           ** **   ** **                             \n"
			" **         ******  *******   ******  ******   **  *****    **  ** **  **  ******   ****** **  ****** \n"
			" **        **    **  **   ** **      **    **  ** **   **   **   ***   **       **   **  * ** **    **\n"
			" **        **    **  **   **  *****  **    **  ** *******   **    *    **  *******   **    ** **    **\n"
			"  **    ** **    **  **   **      ** **    **  ** **        **         ** **    **   **    ** **    **\n"
			"   ******   ******  ***   ** ******   ******  ***  ******   **         **  ******** ***    **  ****** \n"
			, Vector2(X_POSITION(100), 10), Color::WhiteBright, true);
		Renderer::Get().Submit(
				"\n"
				"    ////                                      /            /  /       /                   //          \n"
				"       //                                     /            /  //      /                               \n"
				"/            ////  //  ///     ////    ////   /     ///    /   //     /   //////   //  // /     ////  \n"
				"/         /     /   /    /  //      /     /   /  /         /    //    /             /   / /  /     /  \n"
				"//        /     /   /    /   /////  /     /   /  /  ////   /     /    /     ////    /     /  /     /  \n"
				" //       //             /          //           //        /          /  //        /      /  //       \n"
				"  //////   //////  ///   // //////   //////  ///  //////   //         //  //////// ///    //  //////  \n"
				, Vector2(X_POSITION(100), 10), Color::Gray, true);
	}
	else if (ScreenManager::Get().currentScreenType == ScreenType::Game)
	{
		Renderer::Get().Submit(
			" ****     **** ******** ****     ** **     **\n"
			" ** **   ** ** **       ** **    ** **     **\n"
			" **  ** **  ** **       **  **   ** **     **\n"
			" **   ***   ** *******  **   **  ** **     **\n"
			" **    *    ** **       **    ** ** **     **\n"
			" **         ** **       **     **** **     **\n"
			" **         ** ******** **      ***  ******* \n",
			Vector2(X_POSITION(42), 5), Color::WhiteBright, true);

		Renderer::Get().Submit(
			"\n"
			"/  /       /  /  ///// /  /     /  /      /  \n"
			"/  //      /  /        /  //    /  /      /  \n"
			"/   //     /  /        /   //   /  /      /  \n"
			"/    //    /  /  ////  /    //  /  /      /  \n"
			"/     /    /  /        /     //    /      /  \n"
			"/          /  /        /      //   //        \n"
			"//         // //////// //      ///  ///////  \n",
			Vector2(X_POSITION(42), 5), Color::Gray, true);
	}
	else if (ScreenManager::Get().currentScreenType == ScreenType::Respawn)
	{
		Renderer::Get().Submit(
			" *******   ********  ******** *******    **     **       ** ****     **\n"
			" **    **  **       **        **    **  ****    **       ** ** **    **\n"
			" **    **  **       **        **    ** **  **   **   *   ** **  **   **\n"
			" *******   *******  ********* ******* **    **  **  ***  ** **   **  **\n"
			" **   **   **              ** **     ********** ** ** ** ** **    ** **\n"
			" **    **  **              ** **     **      ** ****   **** **     ****\n"
			" **     ** ******** ********  **     **      ** **       ** **      ***\n",
			Vector2(X_POSITION(71), 5), Color::WhiteBright, true);
		Renderer::Get().Submit(
			"\n"
			"/  ////   /  /////    ////// /  ////           /        /  /  /     /  \n"
			"/     /   /        /         /     /     //    /        /  /  //    /  \n"
			"/         /        /         /            //   /        /  /   //   /  \n"
			"/  ///    /  ////  ////////  /  ////           /     /  /  /    //  /  \n"
			"/    //   /               /  /      /  //////  /     //    /     //    \n"
			"/     //  /                  /      /       /  /  /   ///  /      //   \n"
			"//     // //////// ////////  //     //      // //       // //      /// \n",
			Vector2(X_POSITION(71), 5), Color::Gray, true);

		Renderer::Get().Submit(
			"LIFE - 1...",
			Vector2(X_POSITION(12), 15),
			Color::WhiteBright);

	}
	else if (ScreenManager::Get().currentScreenType == ScreenType::GameOver)
	{
		Renderer::Get().Submit(
			"   ********      **     ****     **** ********     *******   **      ** ******** *******           \n"
			"  **      **    ****    ** **   ** ** **          **     **  **      ** **       **    **          \n"
			" **            **  **   **  ** **  ** **         **       ** **      ** **       **    **          \n"
			" **           **    **  **   ***   ** *******    **       **  **    **  *******  *******           \n"
			" **    ***** ********** **    *    ** **         **       **   **  **   **       **   **           \n"
			"  **      ** **      ** **         ** **          **     **     ****    **       **    **  ** ** **\n"
			"   ********  **      ** **         ** ********     *******       **     ******** **     ** ** ** **\n",
			Vector2(X_POSITION(100), 10), Color::WhiteBright, true);
		Renderer::Get().Submit(
			"\n"
			"    //////             /  /       /  /  /////       /////   /       /  /  ///// /  ////            \n"
			"         //      //    /  //      /  /                  //  /       /  /        /     /            \n"
			"/                 //   /   //     /  /          /        /  //         /        /                  \n"
			"/                      /    //    /  /  ////    /        /   //        /  ////  /  ///             \n"
			"//    ////  /  //////  /     /    /  /          //            //       /        /    //            \n"
			" //         /       /  /          /  /           //            //      /        /     //  /  /  /  \n"
			"  ////////  //      // //         // ////////     ///////       //     //////// //     // // // // \n",
			Vector2(X_POSITION(100), 10), Color::Gray, true);
	}
	else if (ScreenManager::Get().currentScreenType == ScreenType::MapClear || 
		ScreenManager::Get().currentScreenType == ScreenType::GameClear)
	{
		Renderer::Get().Submit(
			"   ******  **       ********     **     *******  \n"
			"  **    ** **       **          ****    **    ** \n"
			" **        **       **         **  **   **    ** \n"
			" **        **       *******   **    **  *******  \n"
			" **        **       **       ********** **   **  \n"
			"  **    ** **       **       **      ** **    ** \n"
			"  **    ** **       **       **      ** **    ** \n"
			"   ******  ******** ******** **      ** **     **\n",
			Vector2(X_POSITION(48), 5), Color::WhiteBright, true);
		Renderer::Get().Submit(
			"\n"
			"    ////  /        /  /////            /  ////   \n"
			"       // /        /             //    /     /   \n"
			"/         /        /              //   /         \n"
			"/         /        /  ////             /  ///    \n"
			"//        /        /        /  //////  /    //   \n"
			"//        /        /        /  //////  /    //   \n"
			" //       /        /        /       /  /     //  \n"
			"  //////  //////// //////// //      // //     // \n",
			Vector2(X_POSITION(48), 5), Color::Gray, true);
		if (ScreenManager::Get().currentScreenType == ScreenType::MapClear)
		{
			Renderer::Get().Submit(
				"Next Level",
				Vector2(X_POSITION(10), 15),
				Color::WhiteBright);
		}
		else 
		{
			Renderer::Get().Submit(
				"Press Enter to quit the game.",
				Vector2(X_POSITION(30), 15),
				Color::WhiteBright);
		}
		
	}

	int maxLen = 0;
	for (auto* item : displayItems)
	{
		maxLen = max(maxLen, (int)strlen(item->text));
	}

	int startX = X_POSITION(maxLen);

	// 메뉴 아이템 출력.
	for (int ix = 0; ix < static_cast<int>(displayItems.size()); ++ix)
	{
		// 아이템 색상 확인 (선택됐는지 여부).
		Color textColor =
			(ix == currentIndex) ? selectedColor : unselectedColor;
		int length = static_cast<int>(strlen(displayItems[ix]->text));
		Renderer::Get().Submit(
			displayItems[ix]->text,
			Vector2(startX, 20 + (ix * 2)),
			textColor
		);
	}
}
