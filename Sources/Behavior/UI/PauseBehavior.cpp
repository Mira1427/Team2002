#include "PauseBehavior.h"

#include "../../Library/Library/Library.h"

#include "../../EventManager.h"

void PauseOverlayBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
	{
		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();
		renderer->writeDepth_ = true;
		renderer->testDepth_ = true;
		renderer->size_  = { RootsLib::Window::GetWidth(), RootsLib::Window::GetHeight() };
		renderer->color_ = Vector4::Black_;
		renderer->color_.a = 0.3f;
	}
		obj->state_++;

	case 1:

		break;
	}
}


void PauseEraser::Execute(GameObject* obj)
{
	if (!EventManager::Instance().paused_)
		obj->Destroy();
}
