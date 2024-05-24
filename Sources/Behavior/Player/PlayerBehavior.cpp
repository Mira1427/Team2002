#include "PlayerBehavior.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/Input/InputManager.h"

#include "../../Library/GameObject/Component.h"
#include "../../Library/GameObject/BehaviorManager.h"

#include "../../Library/Graphics/ModelManager.h"


// --- プレイヤーの基本の行動処理 ---
void BasePlayerBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:

		obj->state_++;
		break;

	case 1:

	{
		InputManager& input = InputManager::Instance();

		AnimatorComponent* animator = obj->GetComponent<AnimatorComponent>();								// アニメーションコンポーネントの取得
		PlayerControllerComponent* controller = obj->parent_->GetComponent<PlayerControllerComponent>();	// 親のコントローラーコンポーネントの取得
		PlayerComponent* player = obj->GetComponent<PlayerComponent>();										// プレイヤーコンポーネントの取得


		// --- 回転処理 ---
		float offset = 90.0f;
		float theta = DirectX::XMConvertToRadians(player->angleOffset_ + obj->parent_->transform_->rotation_.y + offset);
		obj->transform_->position_.x = cosf(-theta) * controller->range_;
		obj->transform_->position_.z = sinf(-theta) * controller->range_;

		obj->transform_->rotation_.y = obj->parent_->transform_->rotation_.y + player->angleOffset_;


		// --- 射撃フラグ ---
		bool shootable[2] =
		{
			static_cast<bool>(input.down(0) & input::LMB),
			static_cast<bool>(input.down(0) & input::RMB),
		};

		size_t playerIndex = player->playerNum_;

		switch (input.state(0) & (input::LEFT | input::RIGHT))
		{
		case input::LEFT: break;
		case input::RIGHT: break;

		default:

		// --- 射撃処理 ---
		if (shootable[playerIndex] && controller->bullet_[playerIndex] >= controller->bulletCost_)
		{
			// --- 弾丸の追加 ---
			AddBullet(obj);

			// --- 弾薬の減算 ---
			controller->bullet_[playerIndex] -= controller->bulletCost_;
			controller->bullet_[playerIndex] = (std::max)(controller->bullet_[playerIndex], 0.0f);
		}

		}


		// --- アニメーションの更新 ---
		animator->Update(elapsedTime);
	}

		break;
	}
}


// --- 弾丸の追加 ---
void BasePlayerBehavior::AddBullet(const GameObject* parent)
{
	GameObject* bullet = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		parent->transform_->position_,
		BehaviorManager::Instance().GetBehavior("Bullet")
	);

	bullet->transform_->scaling_ *= 0.01f;

	RigidBodyComponent* rigidBody = bullet->AddComponent<RigidBodyComponent>();
	rigidBody->velocity_ = Vector3::Zero_ - bullet->transform_->position_;

	InstancedMeshComponent* renderer = bullet->AddComponent<InstancedMeshComponent>();
	renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Bullet.fbx", 1000, nullptr, true);

	SphereCollider* collider = bullet->AddCollider<SphereCollider>();
	collider->radius_ = 1.0f;

}


// --- プレイヤーの操作処理 ---
void PlayerControllerBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:

		obj->state_++;
		break;

	case 1:

	{
		InputManager& input = InputManager::Instance();

		PlayerControllerComponent* controller = obj->GetComponent<PlayerControllerComponent>();	// コントローラーコンポーネントの取得

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
	switch (input.state(0) & (input::LEFT | input::RIGHT))
	{
	case input::LEFT:
		obj->transform_->rotation_.y += rotateSpeed;								// 回転
		controller->actionGauge_ -= elapsedTime;									// アクションゲージの減少
		controller->bullet_[0] += controller->addBulletValue_ * elapsedTime;		// 弾薬の増加
		controller->bullet_[1] += controller->addBulletValue_ * elapsedTime;		// 弾薬の増加
		break;

	case input::RIGHT:
		obj->transform_->rotation_.y += -rotateSpeed;								// 回転
		controller->actionGauge_ += elapsedTime;									// アクションゲージの増加
		controller->bullet_[0] += controller->addBulletValue_ * elapsedTime;		// 弾薬の増加
		controller->bullet_[1] += controller->addBulletValue_ * elapsedTime;		// 弾薬の増加
		break;
	}


	// --- 角度を 0 ~ 360 に制限 ---
	if (obj->transform_->rotation_.y > 360.0f) obj->transform_->rotation_.y -= 360.0f;
	if (obj->transform_->rotation_.y < 0.0f) obj->transform_->rotation_.y += 360.0f;

}


// --- プレイヤーの弾薬ゲージの処理 ---
void PlayerBulletGaugeBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
		obj->state_++;
		break;

	case 1:
	{
		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();						// 描画コンポーネント取得
		PlayerControllerComponent*	controller = obj->parent_->parent_->GetComponent<PlayerControllerComponent>();	// コントローラーコンポーネント取得
		PlayerComponent*			player = obj->parent_->GetComponent<PlayerComponent>();							// プレイヤーコンポーネント取得



		controller->bullet_[player->playerNum_] = (std::min)(controller->bullet_[player->playerNum_], controller->maxBulletValue_);	// 値の制限

		renderer->size_.x = controller->bullet_[player->playerNum_];

		break;
	}

	}
}
