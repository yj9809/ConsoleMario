#include "CameraManager.h"

#include <algorithm>

CameraManager::CameraManager(int cameraWidth, int worldWidth)
	: cameraWidth(cameraWidth), worldWidth(worldWidth)
{

}

void CameraManager::Update(int playerXPosition)
{
	// 카메라 폭이 0 이하일 경우 처리하지 않음.
    if (cameraWidth <= 0)
    {
        cameraXPosition = 0;
        return;
    }

	// 플레이어 위치가 카메라 중앙을 넘어서면 카메라 위치 갱신.
    const int half = cameraWidth / 2;
	// 원하는 카메라 X 위치 계산.
    const int desired = playerXPosition - half;

    // 왼쪽으로는 갱신되지 않는 규칙을 적용.
	// desired가 현재 카메라 위치보다 크면 갱신.
    if (desired > cameraXPosition)
    {
        cameraXPosition = desired;
    }

	// 카메라가 월드 경계를 벗어나지 않도록 조정.
    const int maxX = std::max(0, worldWidth - cameraWidth);
    if (cameraXPosition < 0) cameraXPosition = 0;
    if (cameraXPosition > maxX) cameraXPosition = maxX;
}

void CameraManager::ResetToSpawn(int spawnX)
{
	// 스폰 지점이 화면 가운데 오도록 카메라 위치 설정.
    cameraXPosition = spawnX - (cameraWidth / 2);

	// 카메라가 월드 경계를 벗어나지 않도록 조정.
    const int maxX = std::max(0, worldWidth - cameraWidth);
    if (cameraXPosition < 0) cameraXPosition = 0;
	if (cameraXPosition > maxX) cameraXPosition = maxX;
}
