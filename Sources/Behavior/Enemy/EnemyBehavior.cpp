#include "EnemyBehavior.h"

#include "../../Library/Audio/Audio.h"

#include "../../Library/Scene/SceneManager.h"
#include "../../Library/Scene/SceneOver.h"

#include "../../Library/Library/CameraManager.h"

#include "../../Sources/Component/Component.h"
#include "../../Sources/EventManager.h"
#include "../../Sources/ParameterManager.h"


// --- �G�̊�{���� ---
void BaseEnemyBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:
		obj->state_++;
		break;

	case 1:

	{
		RigidBodyComponent* rigidBody = obj->GetComponent<RigidBodyComponent>();	// ���̃R���|�[�l���g�擾
		SphereCollider* collider = obj->GetComponent<SphereCollider>();		// ���̔���R���|�[�l���g�擾
		EnemyComponent* enemy = obj->GetComponent<EnemyComponent>();		// �G�R���|�[�l���g�̎擾


		// --- �͈͊O�ɍs�������̏��� ---
		HitTown(obj, 100.0f);

		// --- ��]���� ---
		Rotate(obj, rigidBody);


		Update(obj, collider, elapsedTime);

		// --- �ړ����� ---
		obj->transform_->position_ += rigidBody->velocity_ * elapsedTime;

		AnimatorComponent* animator = obj->GetComponent<AnimatorComponent>();
		if (animator)
			animator->Update(elapsedTime);
	}

	break;
	}
}


void BaseEnemyBehavior::HitTown(GameObject* obj, const float range)
{
	Vector3 vec = obj->transform_->position_ - Vector3(0.0f, 0.0f, 0.0f);
	float length = vec.Length();

	if (length > range)
	{
		// --- �G������ ---
		obj->Destroy();

		// --- �p�x����X��n�Ԗڂ̃��C�t�����炷 ---
		float atan = atan2(vec.z, vec.x);
		atan = DirectX::XMConvertToDegrees(atan) + 180.0f;
		atan += 45.0f;	// ��������45�x��]������
		if (atan > 360.0f)
			atan -= 360.0f;

		size_t index = static_cast<size_t>(atan / 90.0f);	// �C���f�b�N�X�̌v�Z
		index = index == 4 ? 0 : index;
		StageComponent* stage = EventManager::Instance().stages_[index]->GetComponent<StageComponent>();
		stage->life_--;


		// --- ���G�t�F�N�g�̍Đ� ---
		ParameterManager::Instance().smokeEffect_->play(obj->transform_->position_, { 10.0f, 10.0f, 10.0f }, Vector3::Zero_);

		CameraManager::Instance().currentCamera_->timer_ = 0.5f;

		AudioManager::instance().playSound(1/*Damage*/);


		if (stage->life_ <= 0)
		{
			EventManager::Instance().TranslateMessage(EventMessage::TO_RESULT_SCENE);
		}

		stage->life_ = (std::max)(stage->life_, 0);
	}

}


void BaseEnemyBehavior::Rotate(GameObject* obj, RigidBodyComponent* rigidBody)
{
	Matrix R;
	R.MakeRotationFromQuaternion(obj->transform_->orientation_);
	Vector3 front = R.v_[2].xyz();

	Vector3 moveVec = Vector3::Normalize(rigidBody->velocity_);
	Vector3 cross = front.Cross(moveVec);
	float dot = front.Dot(moveVec);
	float angle = acosf(dot);
	Quaternion rot;
	rot.SetRotation(cross, angle);
	if (angle > 1e-8f)
		DirectX::XMStoreFloat4(&obj->transform_->orientation_.vec_, DirectX::XMQuaternionSlerp(obj->transform_->orientation_, obj->transform_->orientation_ * rot, 0.3f));
}


void FlyEnemyBehavior::Update(GameObject* obj, SphereCollider* collider, float elapsedTime)
{
	obj->timer_ += elapsedTime;

	collider->offset_.y = 3.0f - obj->transform_->position_.y;

	obj->transform_->position_.y = 5.0f/*Offset*/ + sinf(obj->timer_) * 2.5f;
}
