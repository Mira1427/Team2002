#include "EnemyBehavior.h"

#include "../../Sources/Component/Component.h"

#include "../../Sources/EventManager.h"


// --- �G�̊�{���� ---
void BaseEnemyBehavior::Execute(GameObject* obj, float elapsedTime)
{
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
		Vector3 vec = obj->transform_->position_ - Vector3(0.0f, 0.0f, 0.0f);
		float length = vec.Length();

		if (length > 100.0f)
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

			stage->life_ = (std::max)(stage->life_, 0.0f);
		}


		// --- �ړ����� ---
		obj->transform_->position_ += rigidBody->velocity_ * elapsedTime;
	}

	break;
	}
}
