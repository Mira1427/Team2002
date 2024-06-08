#include "PlayerBehavior.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/Input/InputManager.h"

#include "../../Library/GameObject/BehaviorManager.h"
#include "../../Library/GameObject/EraserManager.h"

#include "../../Library/Graphics/ModelManager.h"

#include "../../Library/Library/Library.h"

#include "../../Component/Component.h"

#include "../../EventManager.h"
#include "../../ParameterManager.h"


// ===== プレイヤーの基本の行動処理 ======================================================================================================================================================
void BasePlayerBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:
		obj->state_++;

	case 1:
	{
		PlayerControllerComponent* controller = obj->parent_->GetComponent<PlayerControllerComponent>();	// 親のコントローラーコンポーネントの取得
		PlayerComponent* player = obj->GetComponent<PlayerComponent>();										// プレイヤーコンポーネントの取得

		Rotate(obj, player, controller, 90.0f);

		break;
	}

	case 2:
	{
		AnimatorComponent* animator = obj->GetComponent<AnimatorComponent>();								// アニメーションコンポーネントの取得
		PlayerControllerComponent* controller = obj->parent_->GetComponent<PlayerControllerComponent>();	// 親のコントローラーコンポーネントの取得
		PlayerComponent* player = obj->GetComponent<PlayerComponent>();										// プレイヤーコンポーネントの取得


		// --- 回転処理 ---
		Rotate(obj, player, controller, 90.0f);


		// --- 射撃処理 ---
		if (!controller->shotLaser_)
			Shot(obj, player, controller);


		// --- アニメーションの更新 ---
		if (animator)
			animator->Update(elapsedTime);
	}

		break;
	}
}


// --- 回転の処理 ---
void BasePlayerBehavior::Rotate(GameObject* obj, PlayerComponent* player, PlayerControllerComponent* controller, float angleOffset)
{
	float theta = DirectX::XMConvertToRadians(player->angleOffset_ + obj->parent_->transform_->rotation_.y + angleOffset);
	obj->transform_->position_.x = cosf(-theta) * controller->range_;
	obj->transform_->position_.z = sinf(-theta) * controller->range_;

	obj->transform_->rotation_.y = obj->parent_->transform_->rotation_.y + player->angleOffset_ + 90.0f;
}


// --- 射撃処理 ---
void BasePlayerBehavior::Shot(GameObject* obj, PlayerComponent* player, PlayerControllerComponent* controller)
{	
	InputManager& input = InputManager::Instance();

	// --- 射撃フラグ ---
	bool shootable[2] =
	{
		static_cast<bool>(input.down(0) & Input::LEFT_ATTACK),
		static_cast<bool>(input.down(0) & Input::RIGHT_ATTACK),
	};

	size_t playerIndex = player->playerNum_;



	// --- 左右のキーを押してなかったら ---
	switch (input.state(0) & (Input::LEFT | Input::RIGHT))
	{
	case Input::LEFT:
	case Input::RIGHT: break;

	default:

		// --- 射撃処理 ---
		if (shootable[playerIndex] && controller->bullet_[playerIndex] >= controller->bulletCost_)
		{
			float g = controller->attackGauge_;	// -1.0 ~ 1.0
			g = (g + 1.0f) / 2.0f;	// 0.0 ~ 1.0
			float a = RootsLib::Math::Lerp(controller->minAttackAmount_, controller->maxAttackAmount_, g);
			g = 1 - g;
			float r = RootsLib::Math::Lerp(controller->minRangeAmount_, controller->maxRangeAmount_, g);

			// --- 弾丸の追加 ---
			AddBullet(obj, 0.02f, a, r, 125.0f);

			// --- 弾薬の減算 ---
			controller->bullet_[playerIndex] -= controller->bulletCost_;
			controller->bullet_[playerIndex] = (std::max)(controller->bullet_[playerIndex], 0.0f);
		}
	}
}


// --- 弾丸の追加 ---
void BasePlayerBehavior::AddBullet(GameObject* parent, const float scaling, const float attackAmount, const float radius, const float speed)
{
	Vector3 position = parent->transform_->position_;
	position.y += 7.5f;

	GameObject* bullet = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		position,
		BehaviorManager::Instance().GetBehavior("Bullet")
	);

	bullet->name_ = u8"弾";
	bullet->eraser_ = EraserManager::Instance().GetEraser("Scene");

	bullet->transform_->scaling_ *= scaling;

	RigidBodyComponent* rigidBody = bullet->AddComponent<RigidBodyComponent>();
	rigidBody->velocity_ = Vector3::Normalize(Vector3::Zero_ - bullet->transform_->position_) * speed;

	InstancedMeshComponent* renderer = bullet->AddComponent<InstancedMeshComponent>();
	renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Bullet.fbx", 1000, nullptr, true);

	BulletComponent* bulletComp = bullet->AddComponent<BulletComponent>();
	bulletComp->attack_ = attackAmount;
	bulletComp->radius_ = radius;
	bulletComp->type_ = parent->GetComponent<PlayerComponent>()->type_;

	SphereCollider* collider = bullet->AddCollider<SphereCollider>();
	collider->radius_ = 1.0f;
}



// ===== プレイヤーの操作処理 ======================================================================================================================================================
void PlayerControllerBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:

		obj->state_++;
		break;

	case 1:
	{
		PlayerControllerComponent* controller = obj->GetComponent<PlayerControllerComponent>();	// コントローラーコンポーネントの取得

		float rotateSpeed = 22.5f * elapsedTime;
		obj->transform_->rotation_.y += rotateSpeed; // 回転

		break;
	}

	case 2:
	{
		InputManager& input = InputManager::Instance();

		PlayerControllerComponent* controller = obj->GetComponent<PlayerControllerComponent>();	// コントローラーコンポーネントの取得


		// --- TODO : タイプの反転 ---
		if (input.down(0) & Input::UP && controller->hasSwapColor_)
		{
			PlayerComponent* player1 = obj->child_[0]->GetComponent<PlayerComponent>();
			PlayerComponent* player2 = obj->child_[1]->GetComponent<PlayerComponent>();
			std::swap(player1->type_, player2->type_);	// タイプの入れ替え

			// --- モデルの差し替え ---
			static const char* fileNames[2] = { "./Data/Model/InstancedMesh/Player/White_Train.fbx", "./Data/Model/InstancedMesh/Player/Black_Train.fbx" };
			MeshRendererComponent* renderer = obj->child_[0]->GetComponent<MeshRendererComponent>();
			renderer->model_ = ModelManager::Instance().GetModel(fileNames[static_cast<size_t>(player1->type_)]);
			renderer = obj->child_[1]->GetComponent<MeshRendererComponent>();
			renderer->model_ = ModelManager::Instance().GetModel(fileNames[static_cast<size_t>(player2->type_)]);

			std::swap(controller->bullet_[0], controller->bullet_[1]);	// 弾薬の入れ替え

			controller->hasSwapColor_ = false;
		}


		// --- TODO : ゲージの反転 ---
		if (input.down(0) & Input::DOWN && controller->hasSwapGauge_)
		{
			controller->attackGauge_ *= -1.0f;
			controller->hasSwapGauge_ = false;
		}


		ShotLaser(obj, controller);

		Rotate(obj, controller, elapsedTime);	// 回転処理
	}

	break;
	}
}


// --- 回転処理 ---
void PlayerControllerBehavior::Rotate(GameObject* obj, PlayerControllerComponent* controller, float elapsedTime) const
{	
	InputManager& input = InputManager::Instance();


	// --- 中心を原点に回転 ---
	float rotateSpeed = controller->rotateSpeed_ * elapsedTime;

	// --- 回転処理 ---
	switch (input.state(0) & (Input::LEFT | Input::RIGHT))
	{
	case Input::LEFT:
		obj->transform_->rotation_.y += rotateSpeed;								// 回転
		controller->bullet_[0] += controller->addBulletValue_ * elapsedTime;		// 弾薬の増加
		controller->bullet_[1] += controller->addBulletValue_ * elapsedTime;		//
		controller->attackGauge_ -= controller->addAttackGaugeValue_ * elapsedTime;	// 攻撃ゲージの減少
		controller->attackGauge_ = (std::max)(controller->attackGauge_, -1.0f);		// 攻撃ゲージの制限
		break;

	case Input::RIGHT:
		obj->transform_->rotation_.y += -rotateSpeed;								// 回転
		controller->bullet_[0] += controller->addBulletValue_ * elapsedTime;		// 弾薬の増加
		controller->bullet_[1] += controller->addBulletValue_ * elapsedTime;		//
		controller->attackGauge_ += controller->addAttackGaugeValue_ * elapsedTime;	// 攻撃ゲージの増加
		controller->attackGauge_ = (std::min)(controller->attackGauge_, 1.0f);		// 攻撃ゲージの制限
		break;
	}


	// --- 角度を 0 ~ 360 に制限 ---
	if (obj->transform_->rotation_.y > 360.0f) obj->transform_->rotation_.y -= 360.0f;
	if (obj->transform_->rotation_.y < 0.0f) obj->transform_->rotation_.y += 360.0f;

}


void PlayerControllerBehavior::ShotLaser(GameObject* obj, PlayerControllerComponent* controller)
{
	InputManager& input = InputManager::Instance();

	controller->shotLaser_ = false;

	switch (input.state(0) & (Input::LEFT | Input::RIGHT))
	{
	case Input::LEFT:
	case Input::RIGHT: break;

	default:

		// --- レーザーの追加 ---
		if (
			(((input.state(0) & Input::LEFT_ATTACK) && (input.down(0) & Input::RIGHT_ATTACK)) ||
				((input.state(0) & Input::RIGHT_ATTACK) && (input.down(0) & Input::LEFT_ATTACK))) &&
			(controller->bullet_[0] > controller->maxBulletValue_ * 0.5f) &&
			(controller->bullet_[1] > controller->maxBulletValue_ * 0.5f)
			)
		{
			controller->bullet_[0] -= controller->maxBulletValue_ * 0.5f;
			controller->bullet_[1] -= controller->maxBulletValue_ * 0.5f;
			controller->bullet_[0] = (std::max)(controller->bullet_[0], 0.0f);
			controller->bullet_[1] = (std::max)(controller->bullet_[1], 0.0f);

			controller->shotLaser_ = true;


			AddLaser(obj->child_[0], controller->laserAttackAmount_, controller->laserSize_);	// レーザー追加


			Vector3 offset = { 0.0f, 7.5f, 0.0f };
			float atan = atan2(0.0f - obj->child_[0]->transform_->position_.z, 0.0f - obj->child_[0]->transform_->position_.x);
			ParameterManager::Instance().laserEffect_->play(obj->child_[0]->transform_->position_ + offset, { 1.0f, 1.0f, 5.0f }, { 0.0f, -atan + DirectX::XMConvertToRadians(-90.0f), 0.0f });
			atan = atan2(0.0f - obj->child_[1]->transform_->position_.z, 0.0f - obj->child_[1]->transform_->position_.x);
			ParameterManager::Instance().laserEffect_->play(obj->child_[1]->transform_->position_ + offset, { 1.0f, 1.0f, 5.0f }, { 0.0f, -atan + DirectX::XMConvertToRadians(-90.0f), 0.0f });
		}
	}

}


void PlayerControllerBehavior::AddLaser(GameObject* parent, const float attackAmount, const Vector3& size)
{
	GameObject* laser = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3::Zero_,
		BehaviorManager::Instance().GetBehavior("Laser")
	);

	laser->name_ = u8"レーザー";
	laser->eraser_ = EraserManager::Instance().GetEraser("Scene");
	laser->parent_ = parent;

	BoxCollider* collider = laser->AddCollider<BoxCollider>();
	collider->size_ = size;
}



// --- プレイヤーの弾薬ゲージの処理 ---
void PlayerBulletGaugeBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:

	{
		obj->transform_->scaling_.y *= -1.0f;

		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();
		renderer->size_.x = 30.0f;
		renderer->writeDepth_ = true;
		renderer->testDepth_ = true;
	}

		obj->state_++;
		break;

	case 1:
	{
		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();						// 描画コンポーネント取得
		PlayerControllerComponent*	controller = obj->parent_->parent_->GetComponent<PlayerControllerComponent>();	// コントローラーコンポーネント取得
		PlayerComponent*			player = obj->parent_->GetComponent<PlayerComponent>();							// プレイヤーコンポーネント取得


		controller->bullet_[player->playerNum_] = (std::min)(controller->bullet_[player->playerNum_], controller->maxBulletValue_);	// 値の制限

		renderer->size_.y = controller->bullet_[player->playerNum_];

		Vector4 colors[2] = { Vector4::White_, Vector4::Black_ };
		renderer->color_ = colors[static_cast<size_t>(player->type_)];

		break;
	}

	}
}
