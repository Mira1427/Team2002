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

	switch (obj->state_)
	{
	case 0:

		obj->timer_ = 10.0f;

		obj->state_++;
		break;

	case 1:

		// --- タイマーがゼロになったら ---
		if (obj->timer_ < 0.0f)
		{
			// --- 敵の追加 ---
			AddEnemy(obj);
		}

		// --- タイマーの減算 ---
		obj->timer_ -= elapsedTime;

		break;
	}
}

void EnemySpawnerBehavior::Hit(GameObject* src, GameObject* dst, float elapsedTime)
{
}


// --- 敵の追加 ---
void EnemySpawnerBehavior::AddEnemy(GameObject* obj)
{
	EnemySpawnerComponent* spawner = obj->GetComponent<EnemySpawnerComponent>();	// スポナーのコンポーネントの取得

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


	// --- 敵コンポーネントの追加 ---
	EnemyComponent* enemyComp = enemy->AddComponent<EnemyComponent>();
	enemyComp->life_ = 3.0f;	// ライフの設定

	size_t colorIndex = rand() % 3;	// 白 黒 グレー
	Vector4 colors[3] = { {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.5f, 0.5f, 0.5f, 1.0f} };
	enemyComp->type_ = static_cast<CharacterType>(colorIndex);


	// --- 描画コンポーネント追加 ---
	//MeshRendererComponent* renderer = enemy->AddComponent<MeshRendererComponent>();
	//renderer->model_ = ModelManager::Instance().GetModel("./Data/Model/plantune.fbx");
	//renderer->color_ = colors[colorIndex];

#if 0
	InstancedMeshComponent* renderer = enemy->AddComponent<InstancedMeshComponent>();
	static const char* grayFileName = ((rand() % 2) == 0) ? "./Data/Model/InstancedMesh/Enemy/enemy2_gray.fbx" : "./Data/Model/InstancedMesh/Enemy/enemy3_gray.fbx";
	static const char* fileNames[3] = { "./Data/Model/InstancedMesh/Enemy/enemy4_white.fbx", "./Data/Model/InstancedMesh/Enemy/enemy1_black.fbx" , grayFileName };
	renderer->model_ = ModelManager::Instance().GetInstancedMesh(fileNames[colorIndex], 1000);
	//renderer->color_ = colors[colorIndex];
#else 
	MeshRendererComponent* renderer = enemy->AddComponent<MeshRendererComponent>();
	renderer->model_ = ModelManager::Instance().GetModel("./Data/Model/SkeletalMesh/Enemy/enemy_1walk.fbx");
#endif


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


	SphereCollider* collider = enemy->AddCollider<SphereCollider>();
	collider->radius_ = 3.0f;

	// --- タイマーをリセット ---
	obj->timer_ = spawner->spawnSpeed_;
}