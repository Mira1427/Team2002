#pragma once

#include "../../Library/GameObject/GameObject.h"


// --- ステージの処理 ---
class StageBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};

