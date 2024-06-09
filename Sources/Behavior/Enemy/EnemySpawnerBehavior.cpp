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

	if (EventManager::Instance().button_.state_ == ButtonState::TITLE)
		obj->Destroy();

	EnemySpawnerComponent* spawner = obj->GetComponent<EnemySpawnerComponent>();	// �X�|�i�[�̃R���|�[�l���g�̎擾

	switch (obj->state_)
	{
	case 0:	// Wave1_set

		// �f�o�b�O�p�X�e�[�g
		obj->state_ = 2;

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 10;

		// --- �J�b�g�C���̒ǉ� ---
		EventManager::Instance().AddWaveCutIn();

		//obj->state_++;
		break;

	case 1:	// �O���[�̓G�P��

		// --- �^�C�}�[���[���ɂȂ����� ---
		if (obj->timer_ < 0.0f)
		{
			// --- �G�̒ǉ� ---
			Wave1Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 2:	// Wave1_2_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 15;

		// --- �J�b�g�C���̒ǉ� ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 3:	// ���E���E�O���[�̓G�����_���P��

		if (obj->timer_ < 0.0f)
		{
			Wave1_2Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 4:	// Wave2_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 4;

		// --- �J�b�g�C���̒ǉ� ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 5:	// �G�̕��U

		if (obj->timer_ < 0.0f)
		{
			Wave2Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 6:	// Wave3_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 4;

		// --- �J�b�g�C���̒ǉ� ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 7:	// �A�C�e���g�p������P��

		if (obj->timer_ < 0.0f)
		{
			Wave3Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 8:	// Wave4_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 50;

		// --- �J�b�g�C���̒ǉ� ---
		EventManager::Instance().AddWaveCutIn();

		obj->state_++;

		break;

	case 9:	// �r�[���g�p������P��

		if (obj->timer_ < 0.0f)
		{
			Wave4Enemy(obj, spawner);
			spawner->spawnCount_--;
		}

		if (spawner->spawnCount_ <= 0) obj->state_++;

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 10: // Wave5_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 4;

		// --- �J�b�g�C���̒ǉ� ---
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

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 12: // Wave6_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 6;

		// --- �J�b�g�C���̒ǉ� ---
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

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 14: // Wave7_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 100;

		// --- �J�b�g�C���̒ǉ� ---
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

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 16: // Wave8_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 6;

		// --- �J�b�g�C���̒ǉ� ---
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

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 18: // Wave9_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 10;

		// --- �J�b�g�C���̒ǉ� ---
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

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;

	case 20: // Wave10_set

		obj->timer_ = 5.0f;

		// �G�̐�
		spawner->spawnCount_ = 5;

		// --- �J�b�g�C���̒ǉ� ---
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

		// --- �^�C�}�[�̌��Z ---
		obj->timer_ -= elapsedTime;

		break;
	}
}


// Wave1�̓G
void EnemySpawnerBehavior::Wave1Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	Enemy(obj, spawner, 3.0f, 2, rand() % 360);

	// --- �^�C�}�[�����Z�b�g ---
	obj->timer_ = spawner->spawnSpeed_;
}

// Wave1_2�̓G
void EnemySpawnerBehavior::Wave1_2Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	Enemy(obj, spawner, 3.0f, rand() % 3, rand() % 360);

	// --- �^�C�}�[�����Z�b�g ---
	obj->timer_ = spawner->spawnSpeed_;
}

// Wave2�̓G
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

	// --- �^�C�}�[�����Z�b�g ---
	obj->timer_ = spawner->spawnSpeed_;
}

// Wave3�̓G
void EnemySpawnerBehavior::Wave3Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	switch (static_cast<int>(spawner->spawnCount_))
	{
	case 4:	// ���̓G�O���[�v
		Enemy(obj, spawner, 3.0f, 1, 30, 1);
		Enemy(obj, spawner, 3.0f, 1, 60);
		Enemy(obj, spawner, 3.0f, 1, 90);
		Enemy(obj, spawner, 3.0f, 1, 120);
		Enemy(obj, spawner, 3.0f, 1, 150);

		break;

	case 3:	// ���̓G�O���[�v
		Enemy(obj, spawner, 3.0f, 0, 210, 0);
		Enemy(obj, spawner, 3.0f, 0, 240);
		Enemy(obj, spawner, 3.0f, 0, 270);
		Enemy(obj, spawner, 3.0f, 0, 300);
		Enemy(obj, spawner, 3.0f, 0, 330);

		break;

	case 2:	// ���̓G�O���[�v
		Enemy(obj, spawner, 3.0f, 0, 30, 1);
		Enemy(obj, spawner, 3.0f, 0, 60);
		Enemy(obj, spawner, 3.0f, 0, 90);
		Enemy(obj, spawner, 3.0f, 0, 120);
		Enemy(obj, spawner, 3.0f, 0, 150);

		break;

	case 1:	// ���̓G�O���[�v
		Enemy(obj, spawner, 3.0f, 1, 210, 0);
		Enemy(obj, spawner, 3.0f, 1, 240);
		Enemy(obj, spawner, 3.0f, 1, 270);
		Enemy(obj, spawner, 3.0f, 1, 300);
		Enemy(obj, spawner, 3.0f, 1, 330);

		break;

	default:
		break;
	}

	// --- �^�C�}�[�����Z�b�g ---
	obj->timer_ = spawner->spawnSpeed_;
}

// �r�[���g�p������P��
void EnemySpawnerBehavior::Wave4Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	Enemy(obj, spawner, 10.0f, rand() % 3, rand() % 360);

	// --- �^�C�}�[�����Z�b�g ---
	const float reset_Time = 1.0f;
	obj->timer_ = reset_Time;
}

// ���ƍ��̓G�𕪎U�i�O���[��HP�����X�j
void EnemySpawnerBehavior::Wave5Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	switch (static_cast<int>(spawner->spawnCount_))
	{
	case 4:	// ���̓G�O���[�v
		Enemy(obj, spawner, 3.0f, 2, 0, 0);
		Enemy(obj, spawner, 3.0f, 1, 60);
		Enemy(obj, spawner, 3.0f, 1, 120);
		Enemy(obj, spawner, 3.0f, 1, 180);
		Enemy(obj, spawner, 3.0f, 1, 240);
		Enemy(obj, spawner, 3.0f, 1, 300);

		break;

	case 3:	// ���̓G�O���[�v
		Enemy(obj, spawner, 3.0f, 0, 30, 1);
		Enemy(obj, spawner, 3.0f, 2, 90);
		Enemy(obj, spawner, 3.0f, 0, 150);
		Enemy(obj, spawner, 3.0f, 0, 210);
		Enemy(obj, spawner, 3.0f, 0, 270);
		Enemy(obj, spawner, 3.0f, 0, 330);

		break;

	case 2:	// ���̓G�O���[�v
		Enemy(obj, spawner, 3.0f, 1, 0, 0);
		Enemy(obj, spawner, 3.0f, 1, 60);
		Enemy(obj, spawner, 3.0f, 1, 120);
		Enemy(obj, spawner, 3.0f, 2, 180);
		Enemy(obj, spawner, 3.0f, 1, 240);
		Enemy(obj, spawner, 3.0f, 1, 300);

		break;

	case 1:	// ���̓G�O���[�v
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

	// --- �^�C�}�[�����Z�b�g ---
	const float reset_Time = 7.0f;
	obj->timer_ = reset_Time;
}

// ���ƍ��̍�HP�i�O���[�𕪎U�j
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

	// --- �^�C�}�[�����Z�b�g ---
	const float reset_Time = 7.0f;
	obj->timer_ = reset_Time;
}

// �O���[���߂����ᕪ�U
void EnemySpawnerBehavior::Wave7Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	Enemy(obj, spawner, 1.0f, 2, rand() % 360);

	// --- �^�C�}�[�����Z�b�g ---
	const float reset_Time = 0.3f;
	obj->timer_ = reset_Time;
}

// �O���[�̍�HP�𕪎U
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


	// --- �^�C�}�[�����Z�b�g ---
	const float reset_Time = 2.0f;
	obj->timer_ = reset_Time;
}

// ���ƍ��̍�HP�𕪎U
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


	// --- �^�C�}�[�����Z�b�g ---
	const float reset_Time = 7.0f;
	obj->timer_ = reset_Time;
}

// �{�X�o��
void EnemySpawnerBehavior::Wave10Enemy(GameObject* obj, EnemySpawnerComponent* spawner)
{
	// --- �^�C�}�[�����Z�b�g ---
	obj->timer_ = spawner->spawnSpeed_;
}

// �G�ݒ�
void EnemySpawnerBehavior::Enemy(GameObject* obj, EnemySpawnerComponent* spawner, float life, int color, int angle, int item)
{
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
	enemyComp->life_ = life;	// ���C�t�̐ݒ�
	enemyComp->itemType_ = item;

	size_t colorIndex = color;	// �� �� �O���[
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

		collider->radius_ = 7.0f;
	}

	else /*�O���[*/
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
}
