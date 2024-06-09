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

	Vector4 colors[2] = { {0.1f, 0.1f, 0.1f, 1.0f}, Vector4::White_, };
	renderer->color_ = colors[static_cast<size_t>(controller->hasSwapColor_)];
}


void GaugeItemUIBehavior::Update(GameObject* obj)
{
	PlayerControllerComponent* controller = obj->parent_->GetComponent<PlayerControllerComponent>();
	SpriteRendererComponent* renderer = obj->GetComponent<SpriteRendererComponent>();

	Vector4 colors[2] = { {0.1f, 0.1f, 0.1f, 1.0f}, Vector4::White_, };
	renderer->color_ = colors[static_cast<size_t>(controller->hasSwapGauge_)];
}


void ItemUIBehavior::Execute(GameObject* obj, float elapsedTime)
{
	obj->timer_ -= elapsedTime;

	auto* renderer = obj->GetComponent<SpriteRendererComponent>();

	if (obj->timer_ < 5.0f)
	{
		float cos = cosf(obj->timer_ * 3.0f);
		if (cos < 0.0f) cos *= -1.0f;

		renderer->color_.a = cos;
	}

	else if (obj->timer_ < 10.0f)
	{
		float cos = cosf(obj->timer_ * 2.0f);
		if (cos < 0.0f) cos *= -1.0f;

		renderer->color_.a = cos;
	}


	if (obj->timer_ < 0.0f)
	{
		obj->timer_ = 0.0f;
		obj->Destroy();
	}
}
