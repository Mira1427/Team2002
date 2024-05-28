#include "BulletBehavior.h"

#include "../../EventManager.h"


// --- 弾丸の処理 ---
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

		BulletComponent* bullet = src->GetComponent<BulletComponent>();
		EnemyComponent* enemy   = dst->GetComponent<EnemyComponent>();

		if(bullet->type_ == enemy->type_ || enemy->type_ == CharactorType::GRAY)
		{
			enemy->life_ -= bullet->attack_;

			// --- 死亡処理 ---
			if (enemy->life_ <= 0.0f)
				dst->Destroy();
		}
	}
}
