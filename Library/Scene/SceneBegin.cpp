#include "SceneBegin.h"

#include "SceneTitle.h"

#include "../../Sources/EventManager.h"

void SceneBegin::Initialize()
{
	EventManager::Instance().InitializeObjects();
}

void SceneBegin::Update(float elapsedTime)
{
	SceneManager::Instance().SetNextScene(std::make_shared<SceneTitle>());
}
