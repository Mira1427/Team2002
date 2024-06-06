#pragma once

#include "../../Library/GameObject/GameObject.h"

#include "../Component/Component.h"

class GameCameraBehavior final : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	void ShakeCamera(GameObject* obj, CameraComponent* camera, CameraShakeComponent* shake, float elapsedTime);
};

