#include "EventManager.h"

#include "../Library/Scene/SceneTitle.h"
#include "../Library/Scene/SceneGame.h"
#include "../Library/Scene/SceneClear.h"
#include "../Library/Scene/SceneOver.h"
#include "../Library/Scene/SceneLoading.h"


void EventManager::Initialize()
{
	messages_.clear();

	for (GameObject* obj : stages_)
		obj = nullptr;

	paused_ = false;
}

void EventManager::Update(float elapsedTime)
{
	for (auto& msg : messages_)
	{
		switch (msg)
		{
		case EventMessage::TO_TITLE_SCENE:
			SceneManager::Instance().SetNextScene(std::make_shared<SceneLoading>(std::make_shared<SceneTitle>()));
			break;

		case EventMessage::TO_GAME_SCENE:
			SceneManager::Instance().SetNextScene(std::make_shared<SceneLoading>(std::make_shared<SceneGame>()));
			break;

		case EventMessage::TO_CLEAR_SCENE:
			SceneManager::Instance().SetNextScene(std::make_shared<SceneLoading>(std::make_shared<SceneClear>()));
			break;

		case EventMessage::TO_OVER_SCENE:
			SceneManager::Instance().SetNextScene(std::make_shared<SceneLoading>(std::make_shared<SceneOver>()));
			break;
		}
	}

	messages_.clear();
}
