#pragma once

#include "../../Library/GameObject/GameObject.h"


// --- �G�̃X�|�i�[�̏��� ---
class EnemySpawnerBehavior final : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override;
};