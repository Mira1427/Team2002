#pragma once

#include "../../Library/GameObject/GameObject.h"

#include "../../Component/Component.h"


// --- �e�ۂ̏��� ---
class BaseBulletBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override;
};
