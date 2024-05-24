#include "EnemyBehavior.h"

#include "../../Sources/Component/Component.h"

#include "../../Sources/EventManager.h"


// --- 敵の基本処理 ---
void BaseEnemyBehavior::Execute(GameObject* obj, float elapsedTime)
{
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
		Vector3 vec = obj->transform_->position_ - Vector3(0.0f, 0.0f, 0.0f);
		float length = vec.Length();

		if (length > 100.0f)
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
			StageComponent* stage = EventManager::Instance().stages_[index]->GetComponent<StageComponent>();
			stage->life_ -= 1.0f;

			stage->life_ = (std::max)(stage->life_, 0.0f);
		}


		// --- 移動処理 ---
		obj->transform_->position_ += rigidBody->velocity_ * elapsedTime;
	}

	break;
	}
}
