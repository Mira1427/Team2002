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
}
