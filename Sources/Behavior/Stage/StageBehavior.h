#pragma once

#include "../../Library/GameObject/GameObject.h"


// --- �X�e�[�W�̏��� ---
class StageBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};

