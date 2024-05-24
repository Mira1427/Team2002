#include "BulletBehavior.h"


// --- 弾丸の処理 ---
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
	// --- スポナーに当たったら ---
	if (dst->type_ == ObjectType::SPAWNER)
	{
		// --- 弾を削除 ---
		src->Destroy();
	}

	// --- 敵に当たったら ---
	else if (dst->type_ == ObjectType::ENEMY)
	{
		src->Destroy();	// 弾を削除

		EnemyComponent* enemy = dst->GetComponent<EnemyComponent>();
		enemy->life_ -= 1.0f;

		// --- 死亡処理 ---
		if (enemy->life_ <= 0.0f)
			dst->Destroy();
	}
}
