#pragma once

class CameraManager
{
public:
	CameraManager(int cameraWidth = 120, int worldWidth = 512);

	void Update(int playerXPosition);

	void ResetToSpawn(int spawnX);

	inline int GetCameraXPosition() const { return cameraXPosition; }
	inline int GetCameraWidth() const { return cameraWidth; }

private:
	int cameraWidth = 0;
	
	int worldWidth = 0;

	int cameraXPosition = 0;
};

