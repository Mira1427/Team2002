#include "BulletBehavior.h"

#include "../../Library/Math/Easing.h"

#include "../../Library/Audio/Audio.h"

#include "../../Library/GameObject/BehaviorManager.h"
#include "../../Library/GameObject/EraserManager.h"

#include "../../EventManager.h"
#include "../../ParameterManager.h"


// --- 弾丸の処理 ---
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
	// --- スポナーに当たったら ---
	if (dst->type_ == ObjectType::SPAWNER)
	{
		// --- 弾を削除 ---
		src->Destroy();
	}

	// --- 敵に当たったら ---
	else if (dst->type_ == ObjectType::ENEMY)
	{
		src->Destroy();	// 弾を削除

		BulletComponent* bullet = src->GetComponent<BulletComponent>();
		EnemyComponent* enemy   = dst->GetComponent<EnemyComponent>();

		if (bullet->type_ == enemy->type_ || enemy->type_ == CharacterType::GRAY)
		{
			AddExplosion(src->transform_->position_, bullet->type_, bullet->attack_, bullet->radius_);

			AudioManager::instance().playSound(9/*HitBullet_1*/);
		}

		else
		{
			AudioManager::instance().playSound(10/*HitBullet_2*/);
		}
	}

	else if (dst->type_ == ObjectType::ITEM)
	{
		dst->Destroy();

		auto* controller = EventManager::Instance().controller_->GetComponent<PlayerControllerComponent>();
		
		if (dst->state_ == 0/*Color*/)
			controller->hasSwapColor_ = true;

		else if (dst->state_ == 1/*Gauge*/)
			controller->hasSwapGauge_ = true;

		AudioManager::instance().playSound(11/*GetItem*/);
	}
}


void BaseBulletBehavior::AddExplosion(const Vector3& position, CharacterType type, float attack, float radius)
{
	GameObject* explosion = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		position,
		BehaviorManager::Instance().GetBehavior("BulletExplosion")
	);

	explosion->name_ = u8"弾の爆発";
	explosion->eraser_ = EraserManager::Instance().GetEraser("Scene");

	SphereCollider* collider = explosion->AddCollider<SphereCollider>();
	collider->radius_ = RootsLib::Math::Lerp(1.0f, 30.0f, (radius - 1.0f) / (10.0f - 1.0f)/*MaxRadius*/);

	BulletComponent* bulletComp = explosion->AddComponent<BulletComponent>();
	bulletComp->attack_ = attack;
	bulletComp->type_ = type;


	// --- エフェクトのサイズを計算 ---
	Vector3 scale = Vector3::Unit_;
	auto func = RootsLib::Easing::GetFunction(EaseOutCubic);
	float rate = RootsLib::Math::Lerp(1.0f, 0.2f, func(static_cast<double>((collider->radius_ - 1.0f) / (30.0f - 1.0f)/*MaxRadius*/)));
	scale *= collider->radius_;
	scale *= rate;

	Effekseer::Handle handle = ParameterManager::Instance().explosionEffect_->play(position, scale, Vector3::Zero_);
	ParameterManager::Instance().explosionEffect_->SetFrame(handle, 130.0f);
}



void BulletExplosionBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	obj->state_++;

	if (obj->state_ >= 2/*処理が2回目なら*/ && !GameObjectManager::Instance().showCollision_)
		obj->Destroy();
}

void BulletExplosionBehavior::Hit(GameObject* src, GameObject* dst, float elapsedTime)
{
	if(src->state_ == 1/*処理が1回目なら*/)
	{
		if (dst->type_ == ObjectType::ENEMY)
		{
			BulletComponent* bullet = src->GetComponent<BulletComponent>();
			EnemyComponent* enemy = dst->GetComponent<EnemyComponent>();

			//if (bullet->type_ == enemy->type_ || enemy->type_ == CharacterType::GRAY)
			//{
				enemy->life_ -= bullet->attack_;

				// --- 死亡処理 ---
				if (enemy->life_ <= 0.0f)
				{
					dst->Destroy();

					if (enemy->itemType_ == 0)
						EventManager::Instance().AddColorItem();

					else if (enemy->itemType_ == 1)
						EventManager::Instance().AddGaugeItem();


					AudioManager::instance().playSound(3/*Kill*/);
				}
			//}
		}
	}
}
