#pragma once

#include "../../Library/GameObject/GameObject.h"


// --- 敵の基本処理 ---
class BaseEnemyBehavior : public Behavior 
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};

