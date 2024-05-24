#pragma once

#include "../../Library/GameObject/GameObject.h"

#include "../../Component/Component.h"


// --- íeä€ÇÃèàóù ---
class BaseBulletBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override;
};
