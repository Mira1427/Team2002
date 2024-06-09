#pragma once
#include "../../Library/GameObject/GameObject.h"

class ResultUIBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {}
};

