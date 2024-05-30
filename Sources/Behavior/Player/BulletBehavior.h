#pragma once

#include "../../Library/GameObject/GameObject.h"

#include "../../Component/Component.h"


// --- �e�ۂ̏��� ---
class BaseBulletBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override;

private:
	void AddExplosion(const Vector3& position, CharacterType type, float attack, float radius);
};


class BulletExplosionBehavior final : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override;
};
