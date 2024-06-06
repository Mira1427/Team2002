#include "LifeGaugeBehavior.h"

#include "../../Component/Component.h"

#include "../../Library/Library/Library.h"

#include "../../EventManager.h"


void LifeGaugeBehavior::Execute(GameObject* obj, float elapsedTime)
{
	int life = EventManager::Instance().stages_[static_cast<size_t>(obj->state_)]->GetComponent<StageComponent>()->life_;

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
