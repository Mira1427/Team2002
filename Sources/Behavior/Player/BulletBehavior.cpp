#include "BulletBehavior.h"

#include "../../EventManager.h"


// --- ’eŠÛ‚Ìˆ— ---
void BaseBulletBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:
		obj->state_++;
		break;

	case 1:

	{
		RigidBodyComponent* rigidBody = obj->GetComponent<RigidBodyComponent>();

		obj->transform_->position_ += rigidBody->velocity_ * elapsedTime;
	}

		break;
	}
}


void BaseBulletBehavior::Hit(GameObject* src, GameObject* dst, float elapsedTime)
{
	// --- ƒXƒ|ƒi[‚É“–‚½‚Á‚½‚ç ---
	if (dst->type_ == ObjectType::SPAWNER)
	{
		// --- ’e‚ðíœ ---
		src->Destroy();
	}

	// --- “G‚É“–‚½‚Á‚½‚ç ---
	else if (dst->type_ == ObjectType::ENEMY)
	{
		src->Destroy();	// ’e‚ðíœ

		BulletComponent* bullet = src->GetComponent<BulletComponent>();
		EnemyComponent* enemy   = dst->GetComponent<EnemyComponent>();
		enemy->life_ -= bullet->attack_;

		// --- Ž€–Sˆ— ---
		if (enemy->life_ <= 0.0f)
			dst->Destroy();
	}
}
