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

	{
		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();
		renderer->center_.x = renderer->size_.x * 0.5f;
	}

		obj->state_++;

	case 1:

	{
		PlayerControllerComponent*  controller	= obj->parent_->parent_->GetComponent<PlayerControllerComponent>();
		PrimitiveRendererComponent* renderer	= obj->GetComponent<PrimitiveRendererComponent>();

#if 1
		// --- ゲージを変動させない ---
		renderer->size_.y = -controller->maxAttackGaugeHeight_;
#else
		// --- ゲージを変動させる ---
		//renderer->size_.y = RootsLib::Math::Lerp(0.0f, controller->maxAttackGaugeHeight_, controller->attackGauge_);
		//renderer->size_.y = (std::max)(renderer->size_.y, 0.0f);
#endif

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

	{
		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();
		renderer->center_.x = renderer->size_.x * 0.5f;
	}

		obj->state_++;

	case 1:

	{
		PlayerControllerComponent* controller = obj->parent_->parent_->GetComponent<PlayerControllerComponent>();
		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();

#if 1
		renderer->size_.y = controller->maxAttackGaugeHeight_;
#else
		renderer->size_.y = RootsLib::Math::Lerp(0.0f, controller->maxAttackGaugeHeight_, controller->attackGauge_);
		renderer->size_.y = (std::min)(renderer->size_.y, 0.0f);
#endif

		obj->transform_->position_ = obj->parent_->transform_->position_;	// 親の座標の合わせる
	}

	break;
	}
}



void AttackGaugeBarBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:

	{
		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();
		renderer->size_.x = 60.0f;
		renderer->size_.y = 20.0f;
		renderer->center_ = renderer->size_ * 0.5f;

		obj->transform_->position_ = obj->parent_->transform_->position_;
	}

		obj->state_++;

	case 1:

	{
		PlayerControllerComponent* controller = obj->parent_->parent_->GetComponent<PlayerControllerComponent>();

		obj->transform_->position_ = obj->parent_->transform_->position_;

		float gauge = (controller->attackGauge_ + 1.0f) * 0.5f;
		float maxGauge = controller->maxAttackGaugeHeight_;
		obj->transform_->position_.y += RootsLib::Math::Lerp(-maxGauge, maxGauge, gauge);
	}

		break;
	}
}
