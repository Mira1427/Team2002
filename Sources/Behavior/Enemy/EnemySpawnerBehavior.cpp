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

	enemy->name_ = u8"�G";
	enemy->type_ = ObjectType::ENEMY;
	enemy->eraser_ = EraserManager::Instance().GetEraser("Scene");

	enemy->transform_->scaling_ *= 0.009f;


	// --- �G�R���|�[�l���g�̒ǉ� ---
	EnemyComponent* enemyComp = enemy->AddComponent<EnemyComponent>();
	enemyComp->life_ = 3.0f;	// ���C�t�̐ݒ�


	// --- �`��R���|�[�l���g�ǉ� ---
	MeshRendererComponent* renderer = enemy->AddComponent<MeshRendererComponent>();
	renderer->model_ = ModelManager::Instance().GetModel("./Data/Model/plantune.fbx");


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