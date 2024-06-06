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

	EnemySpawnerComponent* spawner = obj->GetComponent<EnemySpawnerComponent>();	// �X�|�i�[�̃R���|�[�l���g�̎擾

	switch (obj->state_)
	{
	case 0:	// Wave1_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 10;

		// --- �J�b�g�C���̒ǉ� ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;
		break;

	case 1:	// Wave1

		// --- �^�C�}�[���[���ɂȂ����� ---
		if (obj->timer_ < 0.0f)
		{
			// --- �G�̒ǉ� ---
			AddEnemy(obj);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0.0f) obj->state_++;

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 2:	// Wave2_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 15;

		// --- �J�b�g�C���̒ǉ� ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 3:	// Wave2

		if (obj->timer_ < 0.0f)
		{
			AddEnemy(obj);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

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


	SphereCollider* collider = enemy->AddCollider<SphereCollider>();
	collider->radius_ = 3.0f;
	collider->offset_.y = 5.0f;


	// --- �G�R���|�[�l���g�̒ǉ� ---
	EnemyComponent* enemyComp = enemy->AddComponent<EnemyComponent>();
	enemyComp->life_ = 3.0f;	// ���C�t�̐ݒ�

	size_t colorIndex = rand() % 3;	// �� �� �O���[
	Vector4 colors[3] = { {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.5f, 0.5f, 0.5f, 1.0f} };
	enemyComp->type_ = static_cast<CharacterType>(colorIndex);


	// --- �`��R���|�[�l���g�ǉ� ---
	// --- ���f���̌��� ---
	if (colorIndex == 0/*��*/)
	{
		enemy->transform_->position_.y = 5.0f;
		collider->offset_.y = 0.0f;
		enemy->behavior_ = BehaviorManager::Instance().GetBehavior("FlyEnemy");

		InstancedMeshComponent* renderer = enemy->AddComponent<InstancedMeshComponent>();
		renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Enemy/enemy4_white.fbx", 100);
	}

	else if (colorIndex == 1/*��*/)
	{
		MeshRendererComponent* renderer = enemy->AddComponent<MeshRendererComponent>();
		renderer->model_ = ModelManager::Instance().GetModel("./Data/Model/SkeletalMesh/Enemy/enemy_1walk.fbx");
	}

	else /*�O���[*/
	{
		size_t index = rand() % 2;
		static const char* fileNames[2] = { "./Data/Model/SkeletalMesh/Enemy/enemy_2walk.fbx", "./Data/Model/InstancedMesh/Enemy/enemy3_gray.fbx" };
		
		if (index == 0)
		{
			MeshRendererComponent* renderer = enemy->AddComponent<MeshRendererComponent>();
			renderer->model_ = ModelManager::Instance().GetModel(fileNames[index]);
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

	// --- �^�C�}�[�����Z�b�g ---
	obj->timer_ = spawner->spawnSpeed_;
}