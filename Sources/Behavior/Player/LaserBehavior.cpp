#include "LaserBehavior.h"

#include "../../Component/Component.h"

#include "../../EventManager.h"


void LaserBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:

	{
		auto* player = obj->parent_;
		float atan = atan2(player->transform_->position_.z, player->transform_->position_.x);

		obj->transform_->rotation_.y = -atan + DirectX::XMConvertToRadians(90.0f);
	}

		obj->state_++;
		break;

	case 1:

		obj->state_++;
	break;
#if 0
	case 2:

		obj->state_++;
	break;
#endif
	case 2:

		obj->timer_ += elapsedTime;

		if (obj->timer_ > 0.25f)
		{
			obj->timer_ = 0.0f;
			obj->state_++;
		}

		break;

	case 3:

		obj->state_++;

		break;
	}

	if (obj->state_ >= 3 && !GameObjectManager::Instance().showCollision_)
		obj->Destroy();
}


void LaserBehavior::Hit(GameObject* src, GameObject* dst, float elapsedTime)
{
	if (dst->type_ == ObjectType::ENEMY)
	{
		if (src->state_ == 3)
		{
			EnemyComponent* enemy = dst->GetComponent<EnemyComponent>();
			enemy->life_ -= 3.0f;

			// --- Ž€–Sˆ— ---
			if (enemy->life_ <= 0.0f)
				dst->Destroy();
		}
	}
}
