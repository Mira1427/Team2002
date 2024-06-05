#include "EnemyBehavior.h"

#include "../../Library/Scene/SceneManager.h"
#include "../../Library/Scene/SceneOver.h"

#include "../../Sources/Component/Component.h"

#include "../../Library/Library/CameraManager.h"

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
		StageComponent* stage = EventManager::Instance().stages_[index]->GetComponent<StageComponent>();
		stage->life_ -= 1.0f;


		// --- ���G�t�F�N�g�̍Đ� ---
		ParameterManager::Instance().smokeEffect_->play(obj->transform_->position_, { 10.0f, 10.0f, 10.0f }, Vector3::Zero_);


		if (stage->life_ <= 0.0f)
		{
			EventManager::Instance().TranslateMessage(EventMessage::TO_OVER_SCENE);
			CameraManager::Instance().currentCamera_ = CameraManager::Instance().debugCamera_;
		}

		stage->life_ = (std::max)(stage->life_, 0.0f);
	}

}
