#include "AttackGaugeBehavior.h"

#include "../../Sources/Component/Component.h"

#include "../../Library/Library/Library.h"

#include "../../EventManager.h"


void AttackGaugeBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:
		obj->state_++;

	case 1:

	{
		PlayerControllerComponent*  controller	= obj->parent_->parent_->GetComponent<PlayerControllerComponent>();
		PrimitiveRendererComponent* renderer	= obj->GetComponent<PrimitiveRendererComponent>();

		renderer->size_.x = RootsLib::Math::Lerp(0.0f, 200.0f, controller->attackGauge_);
		renderer->size_.x = (std::max)(renderer->size_.x, 0.0f);

		obj->transform_->position_ = obj->parent_->transform_->position_;	// 親の座標の合わせる
	}

		break;
	}
}



void RangeGaugeBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:
		obj->state_++;

	case 1:

	{
		PlayerControllerComponent* controller = obj->parent_->parent_->GetComponent<PlayerControllerComponent>();
		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();

		renderer->size_.x = RootsLib::Math::Lerp(0.0f, 200.0f, controller->attackGauge_);
		renderer->size_.x = (std::min)(renderer->size_.x, 0.0f);

		obj->transform_->position_ = obj->parent_->transform_->position_;	// 親の座標の合わせる
	}

	break;
	}
}
