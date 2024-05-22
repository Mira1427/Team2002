#pragma once


class GameObject;

class CameraManager
{
private:
	CameraManager();
	~CameraManager() {}

public:
	static CameraManager& Instance()
	{
		static CameraManager instance;
		return instance;
	}

	GameObject* currentCamera_;
};

