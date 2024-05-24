#include "BulletBehavior.h"


// --- ’eŠÛ‚Ìˆ— ---
void BaseBulletBehavior::Execute(GameObject* obj, float elapsedTime)
{
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

		EnemyComponent* enemy = dst->GetComponent<EnemyComponent>();
		enemy->life_ -= 1.0f;

		// --- Ž€–Sˆ— ---
		if (enemy->life_ <= 0.0f)
			dst->Destroy();
	}
}
