#include "LifeGaugeBehavior.h"

#include "../../Component/Component.h"

#include "../../Library/Library/Library.h"

#include "../../EventManager.h"


void LifeGaugeBehavior::Execute(GameObject* obj, float elapsedTime)
{
	int life = EventManager::Instance().stages_[static_cast<size_t>(obj->state_)]->GetComponent<StageComponent>()->life_;

	// --- 背景の位置に合わせて移動 ---
	obj->transform_->position_ = obj->parent_->transform_->position_ + Vector3{ 55.0f, 40.0f, 0.0f };

	SpriteRendererComponent* renderer = obj->GetComponent<SpriteRendererComponent>();
	if (life <= 0)
	{
		renderer->isVisible_ = false;
	}

	else
	{
		renderer->texPos_.x = renderer->texSize_.x * (life - 1);
	}
}
