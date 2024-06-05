#include "EventManager.h"

#include "../../External/ImGui/imgui.h"

#include "../Library/Scene/SceneTitle.h"
#include "../Library/Scene/SceneGame.h"
#include "../Library/Scene/SceneClear.h"
#include "../Library/Scene/SceneOver.h"
#include "../Library/Scene/SceneLoading.h"

#include "../Library/Input/InputManager.h"

#include "../Library/GameObject/GameObject.h"
#include "../Library/GameObject/BehaviorManager.h"
#include "../Library/GameObject/EraserManager.h"

#include "../Library/Library/CameraManager.h"
#include "../Library/Library/Library.h"

#include "Component/Component.h"



void EventManager::Initialize()
{
	messages_.clear();

	for (GameObject* obj : stages_)
		obj = nullptr;

	paused_ = false;

	button_.state_ = {};
	button_.eventIndex_ = 0;
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



void EventManager::UpdateButton()
{
	auto& input = InputManager::Instance();

	switch (button_.state_)
	{
	case ButtonState::TITLE:
		UpdateTitleEvent();
		break;


	case ButtonState::GAME:
		UpdateGameEvent();
		break;


	case ButtonState::PAUSE:
		UpdatePauseEvent();
		break;


	case ButtonState::OVER:
		if (input.down(0) & Input::CONFIRM)
		{
			TranslateMessage(EventMessage::TO_TITLE_SCENE);
			button_.state_ = ButtonState::TITLE;
		}
		break;


	case ButtonState::CLEAR:
		if (input.down(0) & Input::CONFIRM)
		{
			TranslateMessage(EventMessage::TO_TITLE_SCENE);
			button_.state_ = ButtonState::TITLE;
		}
		break;
	}
}


void EventManager::UpdateDebugGui()
{
#ifdef USE_IMGUI

	int b = static_cast<int>(button_.state_);
	ImGui::InputInt(u8"ボタンのステート", &b);
	button_.state_ = static_cast<ButtonState>(b);

	ImGui::InputInt(u8"イベントのインデックス", &button_.eventIndex_);

#endif
}



void EventManager::UpdateTitleEvent()
{
	auto& input = InputManager::Instance();

	if (input.down(0) & Input::UP)
		button_.eventIndex_--;

	else if (input.down(0) & Input::DOWN)
		button_.eventIndex_++;

	button_.eventIndex_ = RootsLib::Math::Clamp(button_.eventIndex_, 0, static_cast<int>(TitleEvent::MAX) - 1);

	switch (button_.eventIndex_)
	{
	case static_cast<int>(TitleEvent::START):
	{
		if (input.down(0) & Input::CONFIRM)
		{
			TranslateMessage(EventMessage::TO_GAME_SCENE);
			button_.state_ = ButtonState::GAME;
			button_.eventIndex_ = 0;
		}
		break;
	}

	case static_cast<int>(TitleEvent::END):
		break;
	}
}


void EventManager::UpdateGameEvent()
{
	auto& input = InputManager::Instance();

	if (input.down(0) & Input::PAUSE)
	{
		paused_ = true;
		button_.state_ = ButtonState::PAUSE;

		GameObject* overlay = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(),
			BehaviorManager::Instance().GetBehavior("PauseOverlay")
		);

		overlay->name_ = u8"ポーズのオーバーレイ";
		overlay->eraser_ = EraserManager::Instance().GetEraser("Pause");

		overlay->AddComponent<PrimitiveRendererComponent>();
	}
}


void EventManager::UpdatePauseEvent()
{
	auto& input = InputManager::Instance();

	if (input.down(0) & Input::UP)
		button_.eventIndex_--;

	else if (input.down(0) & Input::DOWN)
		button_.eventIndex_++;

	button_.eventIndex_ = RootsLib::Math::Clamp(button_.eventIndex_, 0, static_cast<int>(PauseEvent::MAX) - 1);


	if (input.down(0) & Input::ESCAPE)
	{
		paused_ = false;
		button_.state_ = ButtonState::GAME;
		button_.eventIndex_ = 0;
		return;
	}


	switch (button_.eventIndex_)
	{
	case static_cast<int>(PauseEvent::CONTINUE):

		if (input.down(0) & Input::CONFIRM)
		{
			paused_ = false;
			button_.state_ = ButtonState::GAME;
			button_.eventIndex_ = 0;
		}

		break;

	case static_cast<int>(PauseEvent::END):

		if (input.down(0) & Input::CONFIRM)
		{
			TranslateMessage(EventMessage::TO_TITLE_SCENE);
			button_.state_ = ButtonState::TITLE;
			button_.eventIndex_ = 0;
			CameraManager::Instance().currentCamera_ = CameraManager::Instance().debugCamera_;
		}

		break;
	}
}


// --- ウェーブ
void EventManager::AddWaveCutIn()
{
	auto* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("WaveCutIn")
	);

	obj->AddComponent<PrimitiveRendererComponent>();
	obj->AddComponent<UIComponent>();
}
