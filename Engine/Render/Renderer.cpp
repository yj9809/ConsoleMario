#include "Renderer.h"
#include "ScreenBuffer.h"
#include "Util/Util.h"

namespace Wanted
{
	Renderer::Frame::Frame(int bufferCount)
	{
		// 배열 생성 및 초기화.
		charInfoArray = new CHAR_INFO[bufferCount];
		memset(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);

		sortingOrderArray = new int[bufferCount];
		memset(sortingOrderArray, 0, sizeof(int) * bufferCount);
	}

	Renderer::Frame::~Frame()
	{
		SafeDeleteArray(charInfoArray);
		SafeDeleteArray(sortingOrderArray);
	}

	void Renderer::Frame::Clear(const Vector2& screenSize)
	{
		// 2차원 배열로 다루는 1차원 배열을 순회하면서
		// 빈 문자(' ')를 설정.
		const int width = screenSize.x;
		const int height = screenSize.y;

		for(int y = 0; y < height; ++y)
		{
			for(int x = 0; x < width; ++x)
			{
				// 배열 인덱스 구하기.
				const int index = (y * width) + x;

				// 글자 값 및 속성 설정.
				CHAR_INFO& info = charInfoArray[index];
				info.Char.AsciiChar = ' ';
				info.Attributes = 0;

				// 그리기 우선순위 초기화.
				sortingOrderArray[index] = -1;
			}
		}
	}

	// -------------------- Frame -------------------- //
	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2& screenSize)
		: screenSize(screenSize)
	{
		instance = this;

		// 프레임 객체 생성.
		const int bufferCount = screenSize.x * screenSize.y;
		frame = new Frame(bufferCount);

		// 프레임 초기화.
		frame->Clear(screenSize);

		// 이중 버퍼 객체 생성.
		screenBuffers[0] = new ScreenBuffer(screenSize);
		screenBuffers[0]->Clear();

		screenBuffers[1] = new ScreenBuffer(screenSize);
		screenBuffers[1]->Clear();

		// 활성화 버퍼 설정.
		Present();
	}

	Renderer::~Renderer()
	{
		SafeDelete(frame);
		for (ScreenBuffer*& buffer : screenBuffers)
		{
			SafeDelete(buffer);
		}
	}

	void Renderer::Draw()
	{
		// 화면 지우기.
		Clear();

		// 전제조건: 레벨의 모든 액터가 렌더러에 Submit을 완료한 상태.
		// 렌더 큐 순회하면서 프레임 채우기.
		for (const RenderCommand& command : renderQueue)
		{
			// 화면에 그릴 텍스트가 없으면 건너뜀.
			if (!command.text)
			{
				continue;
			}

			int localX = 0;
			int localY = 0;

			for (const char* p = command.text; *p != '\0'; ++p)
			{
				const char ch = *p;

				// 줄바꿈 처리
				if (ch == '\n')
				{
					localY++;
					localX = 0;
					continue;
				}

				const int drawX = command.position.x + localX;
				const int drawY = command.position.y + localY;

				// 다음 문자로 이동
				localX++;

				// 화면 범위 밖이면 스킵
				if (drawX < 0 || drawX >= screenSize.x ||
					drawY < 0 || drawY >= screenSize.y)
				{
					continue;
				}

				const int index = (drawY * screenSize.x) + drawX;

				// 그리기 우선순위 비교.
				if (frame->sortingOrderArray[index] > command.sortingOrder)
				{
					continue;
				}

				// 데이터 기록.
				frame->charInfoArray[index].Char.AsciiChar = ch;
				frame->charInfoArray[index].Attributes = (WORD)command.color;

				// 우선순위 업데이트.
				frame->sortingOrderArray[index] = command.sortingOrder;
			}
		}

		// 그리기.
		GetCurrentBuffer()->Draw(frame->charInfoArray);

		// 버퍼 교환.
		Present();

		// 렌더 큐 비우기.
		renderQueue.clear();
	}

	Renderer& Renderer::Get()
	{
		if (!instance)
		{
			MessageBoxA(nullptr, "Renderer::Get() - instance is null.\n", "Error", MB_OK);
			__debugbreak();
		}

		return *instance;
	}

	void Renderer::Clear()
	{
		// 화면 지우기.
		// 1. 프레임(2차원 배열 데이터) 지우기.
		frame->Clear(screenSize);

		// 2. 콘솔 버퍼 지우기.
		GetCurrentBuffer()->Clear();
	}

	void Renderer::Submit(const char* text, const Vector2& position, Color color, int sortingOrder)
	{
		// 렌더 데이터 생성 후 큐에 추가.
		RenderCommand command = {};
		command.text = text;
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;

		renderQueue.emplace_back(command);
	}

	void Renderer::Present()
	{
		// 버퍼 교환.
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());

		// 인덱스 교체.
		currentBufferIndex = 1 - currentBufferIndex;
	}

	ScreenBuffer* Renderer::GetCurrentBuffer()
	{
		return screenBuffers[currentBufferIndex];
	}
}