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

	case ButtonState::SCENE_BEGIN:
		if (input.down(0) & Input::CONFIRM)
		{
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
	ImGui::InputInt(u8"サブイベントのインデックス", &button_.subEventIndex_);

#endif
}



void EventManager::UpdateTitleEvent()
{
	auto& input = InputManager::Instance();

	if (input.down(0) & Input::LEFT)
		button_.subEventIndex_--;

	if (input.down(0) & Input::RIGHT)
		button_.subEventIndex_++;

	button_.subEventIndex_ = RootsLib::Math::Clamp(button_.subEventIndex_, 0, static_cast<int>(TutorialSelectEvent::MAX) - 1);


	switch (button_.eventIndex_)
	{
	case static_cast<int>(TitleEvent::START):
	{
		if (input.down(0) & Input::CONFIRM)
		{
			TranslateMessage(EventMessage::TO_GAME_SCENE);
			button_.state_ = ButtonState::GAME;
			button_.eventIndex_ = 0;
			button_.subEventIndex_ = 0;
		}
		break;
	}
	}
}


void EventManager::UpdateGameEvent()
{
	auto& input = InputManager::Instance();

	if (input.down(0) & Input::PAUSE)
	{
		paused_ = true;
		button_.state_ = ButtonState::PAUSE;

		// --- ポーズ背景 ---
		{
			GameObject* overlay = GameObjectManager::Instance().Add(
				std::make_shared<GameObject>(),
				Vector3(),
				BehaviorManager::Instance().GetBehavior("PauseOverlay")
			);

			overlay->name_ = u8"ポーズのオーバーレイ";
			overlay->eraser_ = EraserManager::Instance().GetEraser("Pause");

			overlay->AddComponent<PrimitiveRendererComponent>();
		}


		// --- 動画の枠 ---
		{
			GameObject* obj = GameObjectManager::Instance().Add(
				std::make_shared<GameObject>(),
				Vector3(30.0f, 70.0f, 0.0f),
				NULL
			);

			obj->state_ = static_cast<int>(ButtonState::PAUSE);
			obj->name_ = u8"ポーズの動画の枠";
			obj->eraser_ = EraserManager::Instance().GetEraser("Pause");

			obj->transform_->scaling_ *= 0.67f;

			UIComponent* ui = obj->AddComponent<UIComponent>();

			SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
			Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/movieframe_con.png");
			renderer->texture_ = texture;
			renderer->texSize_ = { texture->width_, texture->height_ };
		}


		// --- 説明の枠 ---
		{
			GameObject* obj = GameObjectManager::Instance().Add(
				std::make_shared<GameObject>(),
				Vector3(41.0f, 500.5f, 0.0f),
				NULL
			);

			obj->state_ = static_cast<int>(ButtonState::PAUSE);
			obj->name_ = u8"ポーズの説明の枠";
			obj->eraser_ = EraserManager::Instance().GetEraser("Pause");

			obj->transform_->scaling_ *= 0.67f;

			UIComponent* ui = obj->AddComponent<UIComponent>();

			SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
			Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/textbox_large.png");
			renderer->texture_ = texture;
			renderer->texSize_ = { texture->width_, texture->height_ };
		}


		// --- テキスト ---
		{
			GameObject* obj = GameObjectManager::Instance().Add(
				std::make_shared<GameObject>(),
				Vector3(770.0f, 100.0f, 0.0f),
				NULL
			);

			obj->state_ = static_cast<int>(ButtonState::PAUSE);
			obj->name_ = u8"ポーズのテキスト";
			obj->eraser_ = EraserManager::Instance().GetEraser("Pause");

			obj->transform_->scaling_ *= 0.67f;

			UIComponent* ui = obj->AddComponent<UIComponent>();
			ui->eventID_ = static_cast<int>(PauseEvent::CONTINUE);

			SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
			Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/pause.png");
			renderer->texture_ = texture;
			renderer->texSize_ = { texture->width_, texture->height_ };
		}


		// --- 続ける ---
		{
			GameObject* obj = GameObjectManager::Instance().Add(
				std::make_shared<GameObject>(),
				Vector3(790.0f, 325.0f, 0.0f),
				BehaviorManager::Instance().GetBehavior("PauseButton")
			);

			obj->state_ = static_cast<int>(ButtonState::PAUSE);
			obj->name_ = u8"ポーズの続けるボタン";
			obj->eraser_ = EraserManager::Instance().GetEraser("Pause");

			obj->transform_->scaling_ *= 0.67f;

			UIComponent* ui = obj->AddComponent<UIComponent>();
			ui->eventID_ = static_cast<int>(PauseEvent::CONTINUE);

			SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
			Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/backgame.png");
			renderer->texture_ = texture;
			renderer->texSize_ = { texture->width_, texture->height_ * 0.5f };
		}


		// --- タイトル ---
		{
			GameObject* obj = GameObjectManager::Instance().Add(
				std::make_shared<GameObject>(),
				Vector3(939.0f, 445.0f, 0.0f),
				BehaviorManager::Instance().GetBehavior("PauseButton")
			);

			obj->state_ = static_cast<int>(ButtonState::PAUSE);
			obj->name_ = u8"ポーズのタイトルボタン";
			obj->eraser_ = EraserManager::Instance().GetEraser("Pause");

			obj->transform_->scaling_ *= 0.67f;

			UIComponent* ui = obj->AddComponent<UIComponent>();
			ui->eventID_ = static_cast<int>(PauseEvent::END);

			SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
			Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/gotitle.png");
			renderer->texture_ = texture;
			renderer->texSize_ = { texture->width_, texture->height_ * 0.5f };
		}
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


void EventManager::UpdateSceneBeginEvent()
{
	auto& input = InputManager::Instance();

	if (input.down(0) & Input::CONFIRM)
	{

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

	obj->name_ = u8"ウェーブのカットイン";
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	obj->AddComponent<PrimitiveRendererComponent>();
	obj->AddComponent<UIComponent>();
}
