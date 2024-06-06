#pragma once

#include "../../Library/GameObject/GameObject.h"


// --- ìGÇÃäÓñ{èàóù ---
class BaseEnemyBehavior : public Behavior 
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	void HitTown(GameObject* obj, const float range);
	void Rotate(GameObject* obj, RigidBodyComponent* rigidBody);

	virtual void Update(GameObject* obj, SphereCollider* collider, float elapsedTime) {};
};


class FlyEnemyBehavior final : public BaseEnemyBehavior
{
private:
	void Update(GameObject* obj, SphereCollider* collider, float elapsedTime) override;
};

