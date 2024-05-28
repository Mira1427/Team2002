#include "PlayerBehavior.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/Input/InputManager.h"

#include "../../Library/GameObject/BehaviorManager.h"
#include "../../Library/GameObject/EraserManager.h"

#include "../../Library/Graphics/ModelManager.h"

#include "../../Library/Library/Library.h"

#include "../../Component/Component.h"

#include "../../EventManager.h"


// ===== プレイヤーの基本の行動処理 ======================================================================================================================================================
void BasePlayerBehavior::Execute(GameObject* obj, float elapsedTime)
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
		AnimatorComponent* animator = obj->GetComponent<AnimatorComponent>();								// アニメーションコンポーネントの取得
		PlayerControllerComponent* controller = obj->parent_->GetComponent<PlayerControllerComponent>();	// 親のコントローラーコンポーネントの取得
		PlayerComponent* player = obj->GetComponent<PlayerComponent>();										// プレイヤーコンポーネントの取得


		// --- 回転処理 ---
		Rotate(obj, player, controller, 90.0f);


		// --- 射撃処理 ---
		Shot(obj, player, controller);


		// --- アニメーションの更新 ---
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

	obj->transform_->rotation_.y = obj->parent_->transform_->rotation_.y + player->angleOffset_;
}


// --- 射撃処理 ---
void BasePlayerBehavior::Shot(GameObject* obj, PlayerComponent* player, PlayerControllerComponent* controller)
{	
	InputManager& input = InputManager::Instance();

	// --- 射撃フラグ ---
	bool shootable[2] =
	{
		static_cast<bool>(input.down(0) & Input::LMB),
		static_cast<bool>(input.down(0) & Input::RMB),
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
			float g = controller->attackGauge_;
			float a = RootsLib::Math::Lerp(controller->minAttackAmount_, controller->maxAttackAmount_, (g > 0.0f) ?  g : 0.0f);
			float r = RootsLib::Math::Lerp(controller->minRangeAmount_, controller->maxRangeAmount_, (g < 0.0f) ? -g : 0.0f);

			// --- 弾丸の追加 ---
			AddBullet(obj, 0.02f, a, r);

			// --- 弾薬の減算 ---
			controller->bullet_[playerIndex] -= controller->bulletCost_;
			controller->bullet_[playerIndex] = (std::max)(controller->bullet_[playerIndex], 0.0f);
		}

	}

}


// --- 弾丸の追加 ---
void BasePlayerBehavior::AddBullet(GameObject* parent, const float scaling, const float attackAmount, const float radius)
{
	GameObject* bullet = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		parent->transform_->position_,
		BehaviorManager::Instance().GetBehavior("Bullet")
	);

	bullet->name_ = u8"弾";
	bullet->eraser_ = EraserManager::Instance().GetEraser("Scene");

	bullet->transform_->scaling_ *= scaling;

	RigidBodyComponent* rigidBody = bullet->AddComponent<RigidBodyComponent>();
	rigidBody->velocity_ = Vector3::Zero_ - bullet->transform_->position_;

	InstancedMeshComponent* renderer = bullet->AddComponent<InstancedMeshComponent>();
	renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Bullet.fbx", 1000, nullptr, true);

	BulletComponent* bulletComp = bullet->AddComponent<BulletComponent>();
	bulletComp->attack_ = attackAmount;
	bulletComp->type_ = parent->GetComponent<PlayerComponent>()->type_;

	SphereCollider* collider = bullet->AddCollider<SphereCollider>();
	collider->radius_ = radius;

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


// --- プレイヤーの弾薬ゲージの処理 ---
void PlayerBulletGaugeBehavior::Execute(GameObject* obj, float elapsedTime)
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
		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();						// 描画コンポーネント取得
		PlayerControllerComponent*	controller = obj->parent_->parent_->GetComponent<PlayerControllerComponent>();	// コントローラーコンポーネント取得
		PlayerComponent*			player = obj->parent_->GetComponent<PlayerComponent>();							// プレイヤーコンポーネント取得



		controller->bullet_[player->playerNum_] = (std::min)(controller->bullet_[player->playerNum_], controller->maxBulletValue_);	// 値の制限

		renderer->size_.x = controller->bullet_[player->playerNum_];

		break;
	}

	}
}
