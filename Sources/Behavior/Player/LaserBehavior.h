#pragma once

#include "../../Library/GameObject/GameObject.h"

class LaserBehavior final : public Behavior
{
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override;
};

