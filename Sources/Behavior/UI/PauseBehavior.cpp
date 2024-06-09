#include "PauseBehavior.h"

#include "../../Library/Library/Library.h"

#include "../../Library/Scene/SceneManager.h"

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
		renderer->color_.a = 0.9f;
	}
		obj->state_++;

	case 1:

		break;
	}
}


void PauseEraser::Execute(GameObject* obj)
{
	Scene* currentScene = SceneManager::Instance().GetCurrentScene();
	Scene* nextScene = SceneManager::Instance().GetNextScene();

	if (currentScene != nextScene)
		obj->Destroy();


	if (!EventManager::Instance().paused_)
		obj->Destroy();
}
