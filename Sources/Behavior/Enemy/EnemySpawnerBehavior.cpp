#include "EnemySpawnerBehavior.h"

#include "../../Library/GameObject/BehaviorManager.h"
#include "../../Library/GameObject/EraserManager.h"

#include "../../Library/Scene/SceneGame.h"

#include "../../Library/Graphics/ModelManager.h"

#include "../../Component/Component.h"

#include "../../EventManager.h"


// --- 敵のスポナーの処理 ---
void EnemySpawnerBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	if (EventManager::Instance().button_.state_ == ButtonState::TITLE)
		obj->Destroy();

	EnemySpawnerComponent* spawner = obj->GetComponent<EnemySpawnerComponent>();	// スポナーのコンポーネントの取得

	switch (obj->state_)
	{
	case 0:	// Wave1_set

		// デバッグ用ステート
		obj->state_ = 2;

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 10;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		//obj->state_++;
		break;

	case 1:	// グレーの敵襲撃

		// --- タイマーがゼロになったら ---
		if (obj->timer_ < 0.0f)
		{
			// --- 敵の追加 ---
			Wave1Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;

	case 2:	// Wave1_2_set

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 15;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 3:	// 白・黒・グレーの敵ランダム襲撃

		if (obj->timer_ < 0.0f)
		{
			Wave1_2Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;

	case 4:	// Wave2_set

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 4;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 5:	// 敵の分散

		if (obj->timer_ < 0.0f)
		{
			Wave2Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;

	case 6:	// Wave3_set

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 4;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 7:	// アイテム使用させる襲撃

		if (obj->timer_ < 0.0f)
		{
			Wave3Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;

	case 8:	// Wave4_set

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 50;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 9:	// ビーム使用させる襲撃

		if (obj->timer_ < 0.0f)
		{
			Wave4Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;

	case 10: // Wave5_set

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 4;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 11:

		if (obj->timer_ < 0.0f)
		{
			Wave5Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;

	case 12: // Wave6_set

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 6;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 13:

		if (obj->timer_ < 0.0f)
		{
			Wave6Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;

	case 14: // Wave7_set

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 100;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 15:

		if (obj->timer_ < 0.0f)
		{
			Wave7Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;

	case 16: // Wave8_set

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 6;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 17:

		if (obj->timer_ < 0.0f)
		{
			Wave8Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;

	case 18: // Wave9_set

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 10;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 19:

		if (obj->timer_ < 0.0f)
		{
			Wave9Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;

	case 20: // Wave10_set

		obj->timer_ = 5.0f;

		// 敵の数
		spawner->spawnCount_ = 5;

		// --- カットインの追加 ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 21:

		if (obj->timer_ < 0.0f)
		{
			Wave10Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;
	}
}


// Wave1の敵
void EnemySpawnerBehavior::Wave1Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	Enemy(obj, spawner, 3.0f, 2, rand() % 360);

	// --- タイマーをリセット ---
	obj->timer_ = spawner->spawnSpeed_;
}

// Wave1_2の敵
void EnemySpawnerBehavior::Wave1_2Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	Enemy(obj, spawner, 3.0f, rand() % 3, rand() % 360);

	// --- タイマーをリセット ---
	obj->timer_ = spawner->spawnSpeed_;
}

// Wave2の敵
void EnemySpawnerBehavior::Wave2Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	switch (static_cast<int>(spawner->spawnCount_))
	{
	case 4:
		Enemy(obj, spawner, 3.0f, 0, 0, 1);
		Enemy(obj, spawner, 3.0f, 0, 30);
		Enemy(obj, spawner, 3.0f, 0, 60);

		break;

	case 3:
		Enemy(obj, spawner, 3.0f, 0, 90, 0);
		Enemy(obj, spawner, 3.0f, 0, 120);
		Enemy(obj, spawner, 3.0f, 0, 150);

		break;

	case 2:
		Enemy(obj, spawner, 3.0f, 0, 180, 1);
		Enemy(obj, spawner, 3.0f, 0, 210);
		Enemy(obj, spawner, 3.0f, 0, 240);

		break;

	case 1:
		Enemy(obj, spawner, 3.0f, 0, 270, 0);
		Enemy(obj, spawner, 3.0f, 0, 300);
		Enemy(obj, spawner, 3.0f, 0, 330);

		break;

	default:
		break;
	}

	// --- タイマーをリセット ---
	obj->timer_ = spawner->spawnSpeed_;
}

// Wave3の敵
void EnemySpawnerBehavior::Wave3Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	switch (static_cast<int>(spawner->spawnCount_))
	{
	case 4:	// 黒の敵グループ
		Enemy(obj, spawner, 3.0f, 1, 30, 1);
		Enemy(obj, spawner, 3.0f, 1, 60);
		Enemy(obj, spawner, 3.0f, 1, 90);
		Enemy(obj, spawner, 3.0f, 1, 120);
		Enemy(obj, spawner, 3.0f, 1, 150);

		break;

	case 3:	// 白の敵グループ
		Enemy(obj, spawner, 3.0f, 0, 210, 0);
		Enemy(obj, spawner, 3.0f, 0, 240);
		Enemy(obj, spawner, 3.0f, 0, 270);
		Enemy(obj, spawner, 3.0f, 0, 300);
		Enemy(obj, spawner, 3.0f, 0, 330);

		break;

	case 2:	// 黒の敵グループ
		Enemy(obj, spawner, 3.0f, 0, 30, 1);
		Enemy(obj, spawner, 3.0f, 0, 60);
		Enemy(obj, spawner, 3.0f, 0, 90);
		Enemy(obj, spawner, 3.0f, 0, 120);
		Enemy(obj, spawner, 3.0f, 0, 150);

		break;

	case 1:	// 白の敵グループ
		Enemy(obj, spawner, 3.0f, 1, 210, 0);
		Enemy(obj, spawner, 3.0f, 1, 240);
		Enemy(obj, spawner, 3.0f, 1, 270);
		Enemy(obj, spawner, 3.0f, 1, 300);
		Enemy(obj, spawner, 3.0f, 1, 330);

		break;

	default:
		break;
	}

	// --- タイマーをリセット ---
	obj->timer_ = spawner->spawnSpeed_;
}

// ビーム使用させる襲撃
void EnemySpawnerBehavior::Wave4Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	Enemy(obj, spawner, 10.0f, rand() % 3, rand() % 360);

	// --- タイマーをリセット ---
	const float reset_Time = 1.0f;
	obj->timer_ = reset_Time;
}

// 白と黒の敵を分散（グレー高HPを時々）
void EnemySpawnerBehavior::Wave5Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	switch (static_cast<int>(spawner->spawnCount_))
	{
	case 4:	// 黒の敵グループ
		Enemy(obj, spawner, 3.0f, 2, 0, 0);
		Enemy(obj, spawner, 3.0f, 1, 60);
		Enemy(obj, spawner, 3.0f, 1, 120);
		Enemy(obj, spawner, 3.0f, 1, 180);
		Enemy(obj, spawner, 3.0f, 1, 240);
		Enemy(obj, spawner, 3.0f, 1, 300);

		break;

	case 3:	// 白の敵グループ
		Enemy(obj, spawner, 3.0f, 0, 30, 1);
		Enemy(obj, spawner, 3.0f, 2, 90);
		Enemy(obj, spawner, 3.0f, 0, 150);
		Enemy(obj, spawner, 3.0f, 0, 210);
		Enemy(obj, spawner, 3.0f, 0, 270);
		Enemy(obj, spawner, 3.0f, 0, 330);

		break;

	case 2:	// 黒の敵グループ
		Enemy(obj, spawner, 3.0f, 1, 0, 0);
		Enemy(obj, spawner, 3.0f, 1, 60);
		Enemy(obj, spawner, 3.0f, 1, 120);
		Enemy(obj, spawner, 3.0f, 2, 180);
		Enemy(obj, spawner, 3.0f, 1, 240);
		Enemy(obj, spawner, 3.0f, 1, 300);

		break;

	case 1:	// 白の敵グループ
		Enemy(obj, spawner, 3.0f, 0, 30, 1);
		Enemy(obj, spawner, 3.0f, 0, 90);
		Enemy(obj, spawner, 3.0f, 0, 150);
		Enemy(obj, spawner, 3.0f, 0, 210);
		Enemy(obj, spawner, 3.0f, 2, 270);
		Enemy(obj, spawner, 3.0f, 0, 330);

		break;

	default:
		break;
	}

	// --- タイマーをリセット ---
	const float reset_Time = 7.0f;
	obj->timer_ = reset_Time;
}

// 白と黒の高HP（グレーを分散）
void EnemySpawnerBehavior::Wave6Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	switch (static_cast<int>(spawner->spawnCount_))
	{
	case 6:

		Enemy(obj, spawner, 10.0f, 1, 120, 0);
		Enemy(obj, spawner, 10.0f, 0, 300);

		break;

	case 5:

		Enemy(obj, spawner, 10.0f, 1, 30, 1);
		Enemy(obj, spawner, 10.0f, 0, 210);

		break;

	case 4:

		Enemy(obj, spawner, 10.0f, 1, 90, 0);
		Enemy(obj, spawner, 10.0f, 0, 270);

		break;

	case 3:

		Enemy(obj, spawner, 10.0f, 1, 0, 1);
		Enemy(obj, spawner, 10.0f, 0, 180);

		break;

	case 2:

		Enemy(obj, spawner, 10.0f, 1, 60, 0);
		Enemy(obj, spawner, 10.0f, 0, 240);

		break;

	case 1:

		Enemy(obj, spawner, 10.0f, 1, 150, 1);
		Enemy(obj, spawner, 10.0f, 0, 330);

		break;

	default:
		break;
	}

	// --- タイマーをリセット ---
	const float reset_Time = 7.0f;
	obj->timer_ = reset_Time;
}

// グレーをめっちゃ分散
void EnemySpawnerBehavior::Wave7Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	Enemy(obj, spawner, 1.0f, 2, rand() % 360);

	// --- タイマーをリセット ---
	const float reset_Time = 0.3f;
	obj->timer_ = reset_Time;
}

// グレーの高HPを分散
void EnemySpawnerBehavior::Wave8Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	static int enemy_angle = 0;

	if (spawner->spawnCount_ == 1)
	{
		enemy_angle += 45;
		spawner->spawnCount_ = 6;
	}

	if (enemy_angle == 135)
	{
		spawner->spawnCount_ = 0;
		return;
	}

	Enemy(obj, spawner, 10.0f, 2, enemy_angle);
	Enemy(obj, spawner, 10.0f, 2, enemy_angle + 180);


	// --- タイマーをリセット ---
	const float reset_Time = 2.0f;
	obj->timer_ = reset_Time;
}

// 白と黒の高HPを分散
void EnemySpawnerBehavior::Wave9Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	if (spawner->spawnCount_ >= 6)
	{
		Enemy(obj, spawner, 10.0f, 0, rand() % 180);
		Enemy(obj, spawner, 10.0f, 1, rand() % 180 + 180);
	}
	else
	{
		Enemy(obj, spawner, 10.0f, 0, rand() % 180 + 180);
		Enemy(obj, spawner, 10.0f, 1, rand() % 180);
	}


	// --- タイマーをリセット ---
	const float reset_Time = 7.0f;
	obj->timer_ = reset_Time;
}

// ボス登場
void EnemySpawnerBehavior::Wave10Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	// --- タイマーをリセット ---
	obj->timer_ = spawner->spawnSpeed_;
}

// 敵設定
void EnemySpawnerBehavior::Enemy(GameObject* obj, EnemySpawnerComponent* spawner, float life, int color, int angle, int item)
{
	// --- オブジェクトの追加 ---
	GameObject* enemy = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("BaseEnemy")
	);

	static int count = 0;
	enemy->name_ = u8"敵##" + std::to_string(count++);
	enemy->type_ = ObjectType::ENEMY;
	enemy->eraser_ = EraserManager::Instance().GetEraser("Scene");

	enemy->transform_->scaling_ *= 0.2f;


	SphereCollider* collider = enemy->AddCollider<SphereCollider>();
	collider->radius_ = 3.0f;
	collider->offset_.y = 5.0f;


	// --- 敵コンポーネントの追加 ---
	EnemyComponent* enemyComp = enemy->AddComponent<EnemyComponent>();
	enemyComp->life_ = life;	// ライフの設定
	enemyComp->itemType_ = item;

	size_t colorIndex = color;	// 白 黒 グレー
	enemyComp->type_ = static_cast<CharacterType>(colorIndex);


	// --- 描画コンポーネント追加 ---
	// --- モデルの決定 ---
	if (colorIndex == 0/*白*/)
	{
		enemy->transform_->position_.y = 5.0f;
		collider->offset_.y = 0.0f;
		enemy->behavior_ = BehaviorManager::Instance().GetBehavior("FlyEnemy");

		InstancedMeshComponent* renderer = enemy->AddComponent<InstancedMeshComponent>();
		renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Enemy/enemy4_white.fbx", 100);
	}

	else if (colorIndex == 1/*黒*/)
	{
		MeshRendererComponent* renderer = enemy->AddComponent<MeshRendererComponent>();
		renderer->model_ = ModelManager::Instance().GetModel("./Data/Model/SkeletalMesh/Enemy/enemy_1walk.fbx");

		collider->radius_ = 7.0f;
	}

	else /*グレー*/
	{
		size_t index = rand() % 2;
		static const char* fileNames[2] = { "./Data/Model/SkeletalMesh/Enemy/enemy_2walk.fbx", "./Data/Model/InstancedMesh/Enemy/enemy3_gray.fbx" };

		if (index == 0)
		{
			MeshRendererComponent* renderer = enemy->AddComponent<MeshRendererComponent>();
			renderer->model_ = ModelManager::Instance().GetModel(fileNames[index]);

			collider->radius_ = 7.0f;
		}

		else
		{
			enemy->transform_->position_.y = 5.0f;
			collider->offset_.y = 0.0f;
			enemy->behavior_ = BehaviorManager::Instance().GetBehavior("FlyEnemy");

			InstancedMeshComponent* renderer = enemy->AddComponent<InstancedMeshComponent>();
			renderer->model_ = ModelManager::Instance().GetInstancedMesh(fileNames[index], 100);
		}
	}


	// --- アニメーションコンポーネント追加 ---
	AnimatorComponent* animator = enemy->AddComponent<AnimatorComponent>();
	animator->isPlay_ = true;
	animator->isLoop_ = true;
	animator->timeScale_ = 30.0f;


	// --- 剛体コンポーネントの追加 ---
	RigidBodyComponent* rigidBody = enemy->AddComponent<RigidBodyComponent>();

	// --- 方向を決定 ---
	float theta = static_cast<float>(rand() % 360);
	theta = DirectX::XMConvertToRadians(theta);
	rigidBody->velocity_.x = cosf(theta);
	rigidBody->velocity_.z = sinf(theta);
	rigidBody->velocity_ *= 3.0f;
}
