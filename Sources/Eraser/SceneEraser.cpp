#include "SceneEraser.h"

#include "../../Library/Scene/SceneGame.h"


void SceneEraser::Execute(GameObject* obj)
{
	Scene* currentScene = SceneManager::Instance().GetCurrentScene();
	Scene* nextScene = SceneManager::Instance().GetNextScene();

	if (currentScene != nextScene)
		obj->Destroy();
}
