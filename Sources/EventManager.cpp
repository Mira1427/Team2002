#include "EventManager.h"

#include "../../External/ImGui/imgui.h"

#include "../Library/Scene/SceneTitle.h"
#include "../Library/Scene/SceneGame.h"
#include "../Library/Scene/SceneClear.h"
#include "../Library/Scene/SceneOver.h"
#include "../Library/Scene/SceneResult.h"
#include "../Library/Scene/SceneLoading.h"

#include "../Library/Input/InputManager.h"

#include "../Library/GameObject/GameObject.h"
#include "../Library/GameObject/BehaviorManager.h"
#include "../Library/GameObject/EraserManager.h"

#include "../Library/Graphics/ModelManager.h"

#include "../Library/Library/CameraManager.h"
#include "../Library/Library/Library.h"

#include "Component/Component.h"



void EventManager::Initialize()
{
	messages_.clear();

	for (GameObject* obj : stages_)
		obj = NULL;

	enemySpawner_ = NULL;

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
			SceneManager::Instance().SetNextScene(std::make_shared<SceneTitle>());
			break;

		case EventMessage::TO_GAME_SCENE:
			SceneManager::Instance().SetNextScene(std::make_shared<SceneGame>());
			break;

		case EventMessage::TO_CLEAR_SCENE:
			SceneManager::Instance().SetNextScene(std::make_shared<SceneLoading>(std::make_shared<SceneClear>()));
			break;

		case EventMessage::TO_RESULT_SCENE:
			SceneManager::Instance().SetNextScene(std::make_shared<SceneResult>());
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

	// --- ウィンドウを閉じる ---
	if (input.down(0) & Input::ESCAPE)
		PostMessage(RootsLib::Window::GetHandle(), WM_CLOSE, 0, 0);


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
			paused_ = false;
			TranslateMessage(EventMessage::TO_TITLE_SCENE);
			button_.state_ = ButtonState::TITLE;
			button_.eventIndex_ = 0;
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


	int wave = enemySpawner_->state_ / 2;	// 現在のウェーブ

	SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
	Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/wave.png");
	renderer->texture_ = texture;
	renderer->texSize_.x = (wave != 10) ? 330.0f : texture->width_;
	renderer->texSize_.y = texture->height_ * 0.1f;
	renderer->texPos_.y = renderer->texSize_.y * (wave - 1);
	renderer->center_ = renderer->texSize_ * 0.5f;

	obj->AddComponent<UIComponent>();
}


void EventManager::InitializeObjects()
{
	// --- モデルの読み込み ---
	{
		ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Bullet.fbx", 100, true);
		ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Stage/Rail.fbx", 3, true);
		ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/b.fbx", 3, true);
		ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Stage/ground.fbx", 3, true);

		ModelManager::Instance().LoadModel(RootsLib::DX11::GetDevice(), "./Data/Model/SkeletalMesh/Enemy/enemy_1walk.fbx", true);
		ModelManager::Instance().LoadModel(RootsLib::DX11::GetDevice(), "./Data/Model/SkeletalMesh/Enemy/enemy_2walk.fbx", true);
		ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Enemy/enemy3_gray.fbx", 100, true);
		ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Enemy/enemy4_white.fbx", 100, true);

	}

	
	// --- カメラ ---
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(),
			BehaviorManager::Instance().GetBehavior("GameCamera")
		);

		obj->name_ = u8"ゲームカメラ";

		obj->AddComponent<CameraComponent>();

		obj->AddComponent<CameraShakeComponent>();

		CameraManager::Instance().currentCamera_ = obj;
	}


	// --- ステージ ---
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>()
		);

		obj->name_ = u8"地面";

		obj->transform_->position_.y = -0.56f;
		obj->transform_->scaling_.x = 7.24f;
		obj->transform_->scaling_.z = 7.24f;

		InstancedMeshComponent* renderer = obj->AddComponent<InstancedMeshComponent>();
		renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Stage/ground.fbx", 3);
	}

	// --- レール ---
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>()
		);

		obj->name_ = u8"レール";

		obj->transform_->position_.y = 0.38f;
		obj->transform_->scaling_ *= 0.3f;

		InstancedMeshComponent* renderer = obj->AddComponent<InstancedMeshComponent>();
		renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Stage/Rail.fbx", 3);
	}


	// --- 街 ---
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(),
			NULL
		);

		obj->name_ = u8"街";

		obj->transform_->scaling_ *= 1.55f;

		InstancedMeshComponent* renderer = obj->AddComponent<InstancedMeshComponent>();
		renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/b.fbx", 3);
	}


	// --- コントローラー ---
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(),
			BehaviorManager::Instance().GetBehavior("PlayerController")
		);

		controller_ = obj;

		obj->name_ = u8"プレイヤーのコントローラー";

		PlayerControllerComponent* controller = obj->AddComponent<PlayerControllerComponent>();

		controller->rotateSpeed_ = 45.0f;	// 回転速度
		controller->range_ = 94.0f;			// 中心からの距離

		controller->maxBulletValue_ = 180.0f;	// 弾薬の最大数
		controller->addBulletValue_ = 10.0f;	// 弾薬の増加量
		controller->bulletCost_ = 10.0f;		// 弾薬のコスト

		controller->addAttackGaugeValue_ = 0.5f;	// 攻撃ゲージの増加量

		controller->maxAttackAmount_ = 5.0f;	// 最大攻撃力
		controller->minAttackAmount_ = 1.0f;	// 最小攻撃力
		controller->maxRangeAmount_ = 10.0f;	// 最大範囲
		controller->minRangeAmount_ = 1.0f;		// 最小範囲
		controller->maxAttackGaugeHeight_ = 330.0f;

		controller->laserAttackAmount_ = 5.0f;
		controller->laserSize_ = { 10.0f, 10.0f, 200.0f };
	}


	// --- 自機 ---
	for(size_t i = 0; i < 2; i++)
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(),
			BehaviorManager::Instance().GetBehavior("BasePlayer")
		);

		obj->name_ = u8"キャラ" + std::to_string(i);
		obj->parent_ = controller_;
		controller_->child_.emplace_back(obj);

		obj->transform_->scaling_ *= 0.35f;

		// --- モデル描画コンポーネント追加 ---
		MeshRendererComponent* renderer = obj->AddComponent<MeshRendererComponent>();
		static const char* fileNames[2] = { "./Data/Model/InstancedMesh/Player/White_Train.fbx", "./Data/Model/InstancedMesh/Player/Black_Train.fbx" };
		renderer->model_ = ModelManager::Instance().LoadModel(RootsLib::DX11::GetDevice(), fileNames[i], true, false, nullptr);

		// --- アニメーションコンポーネントの追加 ---
		AnimatorComponent* animator = obj->AddComponent<AnimatorComponent>();
		animator->isPlay_ = true;
		animator->isLoop_ = true;
		animator->timeScale_ = 30.0f;

		PlayerComponent* player = obj->AddComponent<PlayerComponent>();
		player->angleOffset_ = i * 180.0f;
		player->playerNum_ = i;
		player->type_ = static_cast<CharacterType>(i);
	}
}
