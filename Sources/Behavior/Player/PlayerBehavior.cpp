#include "PlayerBehavior.h"

#include "../../Library/GameObject/Component.h"

#include "../../Library/Input/InputManager.h"

#include "../../External/ImGui/imgui.h"


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
		AnimatorComponent* animator = obj->GetComponent<AnimatorComponent>();							// アニメーションコンポーネントの取得
		PlayerControllerComponent* pPlayer = obj->parent_->GetComponent<PlayerControllerComponent>();	// 親のプレイヤー操作コンポーネントの取得
		PlayerComponent* player = obj->GetComponent<PlayerComponent>();								// プレイヤーコンポーネントの取得


		float offset = 90.0f;
		float theta = DirectX::XMConvertToRadians(player->angleOffset_ + obj->parent_->transform_->rotation_.y + offset);
		obj->transform_->position_.x = cosf(-theta) * pPlayer->range_;
		obj->transform_->position_.z = sinf(-theta) * pPlayer->range_;

		obj->transform_->rotation_.y = obj->parent_->transform_->rotation_.y + player->angleOffset_;


		// --- アニメーションの更新 ---
		animator->Update(elapsedTime);
	}

		break;
	}
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
		InputManager& input = InputManager::instance();

		PlayerControllerComponent* player = obj->GetComponent<PlayerControllerComponent>();	// プレイヤーコンポーネントの取得


		// --- 中心を原点に回転 ---
		float rotateSpeed = player->rotateSpeed_ * elapsedTime;

		if (input.state(0) & input::LEFT)
		{
			obj->transform_->rotation_.y += rotateSpeed;
			player->actionGauge_ -= elapsedTime;
		}

		if (input.state(0) & input::RIGHT)
		{
			obj->transform_->rotation_.y += -rotateSpeed;
			player->actionGauge_ += elapsedTime;
		}

		if (obj->transform_->rotation_.y > 360.0f) obj->transform_->rotation_.y -= 360.0f;
		if (obj->transform_->rotation_.y < 0.0f) obj->transform_->rotation_.y += 360.0f;
	}

	break;
	}
}
