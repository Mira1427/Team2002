#pragma once

#include "../../Library/GameObject/GameObject.h"


// --- “G‚ÌŠî–{ˆ— ---
class BaseEnemyBehavior : public Behavior 
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	void HitTown(GameObject* obj, const float range);
};

