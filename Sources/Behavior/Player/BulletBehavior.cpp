#include "BulletBehavior.h"

#include "../../Library/GameObject/BehaviorManager.h"
#include "../../Library/GameObject/EraserManager.h"

#include "../../EventManager.h"
#include "../../ParameterManager.h"


// --- �e�ۂ̏��� ---
void BaseBulletBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:
		obj->state_++;

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

		BulletComponent* bullet = src->GetComponent<BulletComponent>();
		EnemyComponent* enemy   = dst->GetComponent<EnemyComponent>();

		AddExplosion(src->transform_->position_, bullet->type_, bullet->attack_, bullet->radius_);
	}
}


void BaseBulletBehavior::AddExplosion(const Vector3& position, CharacterType type, float attack, float radius)
{
	GameObject* explosion = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		position,
		BehaviorManager::Instance().GetBehavior("BulletExplosion")
	);

	explosion->name_ = u8"�e�̔���";
	explosion->eraser_ = EraserManager::Instance().GetEraser("Scene");

	SphereCollider* collider = explosion->AddCollider<SphereCollider>();
	collider->radius_ = radius;

	BulletComponent* bulletComp = explosion->AddComponent<BulletComponent>();
	bulletComp->attack_ = attack;
	bulletComp->type_ = type;
}



void BulletExplosionBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	obj->state_++;

	if (obj->state_ >= 2/*������2��ڂȂ�*/ && !GameObjectManager::Instance().showCollision_)
		obj->Destroy();
}

void BulletExplosionBehavior::Hit(GameObject* src, GameObject* dst, float elapsedTime)
{
	if(src->state_ == 1/*������1��ڂȂ�*/)
	{
		if (dst->type_ == ObjectType::ENEMY)
		{
			BulletComponent* bullet = src->GetComponent<BulletComponent>();
			EnemyComponent* enemy = dst->GetComponent<EnemyComponent>();

			if (bullet->type_ == enemy->type_ || enemy->type_ == CharacterType::GRAY)
			{
				enemy->life_ -= bullet->attack_;

				SphereCollider* collider = src->GetComponent<SphereCollider>();

				Effekseer::Handle handle = ParameterManager::Instance().explosionEffect_->play(dst->transform_->position_, Vector3::Unit_, Vector3::Zero_);
				ParameterManager::Instance().explosionEffect_->SetFrame(handle, 130.0f);

				// --- ���S���� ---
				if (enemy->life_ <= 0.0f)
					dst->Destroy();
			}
		}
	}
}
