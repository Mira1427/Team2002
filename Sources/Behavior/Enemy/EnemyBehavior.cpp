#include "EnemyBehavior.h"

#include "../../Library/Scene/SceneManager.h"
#include "../../Library/Scene/SceneOver.h"

#include "../../Sources/Component/Component.h"

#include "../../Library/Library/CameraManager.h"

#include "../../Sources/EventManager.h"
#include "../../Sources/ParameterManager.h"


// --- 敵の基本処理 ---
void BaseEnemyBehavior::Execute(GameObject* obj, float elapsedTime)
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
		RigidBodyComponent* rigidBody = obj->GetComponent<RigidBodyComponent>();	// 剛体コンポーネント取得
		SphereCollider* collider = obj->GetComponent<SphereCollider>();		// 球の判定コンポーネント取得
		EnemyComponent* enemy = obj->GetComponent<EnemyComponent>();		// 敵コンポーネントの取得


		// --- 範囲外に行った時の処理 ---
		HitTown(obj, 100.0f);

		// --- 回転処理 ---
		Rotate(obj, rigidBody);


		Update(obj, collider, elapsedTime);

		// --- 移動処理 ---
		obj->transform_->position_ += rigidBody->velocity_ * elapsedTime;

		AnimatorComponent* animator = obj->GetComponent<AnimatorComponent>();
		if (animator)
			animator->Update(elapsedTime);
	}

	break;
	}
}


void BaseEnemyBehavior::HitTown(GameObject* obj, const float range)
{
	Vector3 vec = obj->transform_->position_ - Vector3(0.0f, 0.0f, 0.0f);
	float length = vec.Length();

	if (length > range)
	{
		// --- 敵を消去 ---
		obj->Destroy();

		// --- 角度から街のn番目のライフを減らす ---
		float atan = atan2(vec.z, vec.x);
		atan = DirectX::XMConvertToDegrees(atan) + 180.0f;
		atan += 45.0f;	// 水平から45度回転させる
		if (atan > 360.0f)
			atan -= 360.0f;

		size_t index = static_cast<size_t>(atan / 90.0f);	// インデックスの計算
		index = index == 4 ? 0 : index;
		StageComponent* stage = EventManager::Instance().stages_[index]->GetComponent<StageComponent>();
		stage->life_--;


		// --- 煙エフェクトの再生 ---
		ParameterManager::Instance().smokeEffect_->play(obj->transform_->position_, { 10.0f, 10.0f, 10.0f }, Vector3::Zero_);

		CameraManager::Instance().currentCamera_->timer_ = 0.5f;


		if (stage->life_ <= 0)
		{
			EventManager::Instance().TranslateMessage(EventMessage::TO_RESULT_SCENE);
		}

		stage->life_ = (std::max)(stage->life_, 0);
	}

}


void BaseEnemyBehavior::Rotate(GameObject* obj, RigidBodyComponent* rigidBody)
{
	Matrix R;
	R.MakeRotationFromQuaternion(obj->transform_->orientation_);
	Vector3 front = R.v_[2].xyz();

	Vector3 moveVec = Vector3::Normalize(rigidBody->velocity_);
	Vector3 cross = front.Cross(moveVec);
	float dot = front.Dot(moveVec);
	float angle = acosf(dot);
	Quaternion rot;
	rot.SetRotation(cross, angle);
	if (angle > 1e-8f)
		DirectX::XMStoreFloat4(&obj->transform_->orientation_.vec_, DirectX::XMQuaternionSlerp(obj->transform_->orientation_, obj->transform_->orientation_ * rot, 0.3f));
}


void FlyEnemyBehavior::Update(GameObject* obj, SphereCollider* collider, float elapsedTime)
{
	obj->timer_ += elapsedTime;

	collider->offset_.y = 3.0f - obj->transform_->position_.y;

	obj->transform_->position_.y = 5.0f/*Offset*/ + sinf(obj->timer_) * 2.5f;
}
