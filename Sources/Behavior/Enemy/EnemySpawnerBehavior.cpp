#include "EnemySpawnerBehavior.h"

#include "../../Library/GameObject/BehaviorManager.h"
#include "../../Library/GameObject/EraserManager.h"

#include "../../Library/Scene/SceneGame.h"

#include "../../Library/Graphics/ModelManager.h"

#include "../../Component/Component.h"

#include "../../EventManager.h"



// --- �G�̃X�|�i�[�̏��� ---
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

		// --- �^�C�}�[���[���ɂȂ����� ---
		if (obj->timer_ < 0.0f)
		{
			// --- �G�̒ǉ� ---
			AddEnemy(obj);
		}

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;
	}
}

void EnemySpawnerBehavior::Hit(GameObject* src, GameObject* dst, float elapsedTime)
{
}


// --- �G�̒ǉ� ---
void EnemySpawnerBehavior::AddEnemy(GameObject* obj)
{
	EnemySpawnerComponent* spawner = obj->GetComponent<EnemySpawnerComponent>();	// �X�|�i�[�̃R���|�[�l���g�̎擾

	// --- �I�u�W�F�N�g�̒ǉ� ---
	GameObject* enemy = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("BaseEnemy")
	);

	static int count = 0;
	enemy->name_ = u8"�G##" + std::to_string(count++);
	enemy->type_ = ObjectType::ENEMY;
	enemy->eraser_ = EraserManager::Instance().GetEraser("Scene");

	enemy->transform_->scaling_ *= 0.2f;


	// --- �G�R���|�[�l���g�̒ǉ� ---
	EnemyComponent* enemyComp = enemy->AddComponent<EnemyComponent>();
	enemyComp->life_ = 3.0f;	// ���C�t�̐ݒ�

	size_t colorIndex = rand() % 3;	// �� �� �O���[
	Vector4 colors[3] = { {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.5f, 0.5f, 0.5f, 1.0f} };
	enemyComp->type_ = static_cast<CharacterType>(colorIndex);


	// --- �`��R���|�[�l���g�ǉ� ---
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


	// --- �A�j���[�V�����R���|�[�l���g�ǉ� ---
	AnimatorComponent* animator = enemy->AddComponent<AnimatorComponent>();
	animator->isPlay_ = true;
	animator->isLoop_ = true;
	animator->timeScale_ = 30.0f;


	// --- ���̃R���|�[�l���g�̒ǉ� ---
	RigidBodyComponent* rigidBody = enemy->AddComponent<RigidBodyComponent>();

	// --- ���������� ---
	float theta = static_cast<float>(rand() % 360);
	theta = DirectX::XMConvertToRadians(theta);
	rigidBody->velocity_.x = cosf(theta);
	rigidBody->velocity_.z = sinf(theta);
	rigidBody->velocity_ *= 3.0f;


	SphereCollider* collider = enemy->AddCollider<SphereCollider>();
	collider->radius_ = 3.0f;

	// --- �^�C�}�[�����Z�b�g ---
	obj->timer_ = spawner->spawnSpeed_;
}