#include "BulletBehavior.h"


// --- �e�ۂ̏��� ---
void BaseBulletBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
		obj->state_++;
		break;

	case 1:

	{
		RigidBodyComponent* rigidBody = obj->GetComponent<RigidBodyComponent>();

		obj->transform_->position_ += rigidBody->velocity_ * elapsedTime;
	}

		break;
	}
}


void BaseBulletBehavior::Hit(GameObject* src, GameObject* dst, float elapsedTime)
{
	// --- �X�|�i�[�ɓ��������� ---
	if (dst->type_ == ObjectType::SPAWNER)
	{
		// --- �e���폜 ---
		src->Destroy();
	}

	// --- �G�ɓ��������� ---
	else if (dst->type_ == ObjectType::ENEMY)
	{
		src->Destroy();	// �e���폜

		EnemyComponent* enemy = dst->GetComponent<EnemyComponent>();
		enemy->life_ -= 1.0f;

		// --- ���S���� ---
		if (enemy->life_ <= 0.0f)
			dst->Destroy();
	}
}
