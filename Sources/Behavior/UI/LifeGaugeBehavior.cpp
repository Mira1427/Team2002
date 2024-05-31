#include "LifeGaugeBehavior.h"

#include "../../Component/Component.h"

#include "../../Library/Library/Library.h"

#include "../../EventManager.h"


void LifeGaugeBehavior::Execute(GameObject* obj, float elapsedTime)
{
	float life = EventManager::Instance().stages_[static_cast<size_t>(obj->state_)]->GetComponent<StageComponent>()->life_;

	PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();
	renderer->size_.y = RootsLib::Math::Lerp(0.0f, RootsLib::Window::GetHeight() / 4.0f, life / 5.0f/*ç≈ëÂÉâÉCÉt*/);
}
