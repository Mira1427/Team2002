#include "ItemUIBehavior.h"

#include "../../Component/Component.h"


void BaseItemUIBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
		obj->state_++;

	case 1:

		Update(obj);

		break;
	}
}


void ColorItemUIBehavior::Update(GameObject* obj)
{
	PlayerControllerComponent* controller = obj->parent_->GetComponent<PlayerControllerComponent>();
	SpriteRendererComponent* renderer = obj->GetComponent<SpriteRendererComponent>();

	Vector4 colors[2] = { {0.3f, 0.3f, 0.3f, 1.0f}, Vector4::White_, };
	renderer->color_ = colors[static_cast<size_t>(controller->hasSwapColor_)];
}


void GaugeItemUIBehavior::Update(GameObject* obj)
{
	PlayerControllerComponent* controller = obj->parent_->GetComponent<PlayerControllerComponent>();
	SpriteRendererComponent* renderer = obj->GetComponent<SpriteRendererComponent>();

	Vector4 colors[2] = { {0.2f, 0.2f, 0.2f, 1.0f}, Vector4::White_, };
	renderer->color_ = colors[static_cast<size_t>(controller->hasSwapGauge_)];
}
