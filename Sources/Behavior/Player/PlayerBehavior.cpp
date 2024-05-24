#include "PlayerBehavior.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/Input/InputManager.h"

#include "../../Library/GameObject/BehaviorManager.h"
#include "../../Library/GameObject/EraserManager.h"

#include "../../Library/Graphics/ModelManager.h"

#include "../../Component/Component.h"


// ===== �v���C���[�̊�{�̍s������ ======================================================================================================================================================
void BasePlayerBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:

		obj->state_++;
		break;

	case 1:

	{
		AnimatorComponent* animator = obj->GetComponent<AnimatorComponent>();								// �A�j���[�V�����R���|�[�l���g�̎擾
		PlayerControllerComponent* controller = obj->parent_->GetComponent<PlayerControllerComponent>();	// �e�̃R���g���[���[�R���|�[�l���g�̎擾
		PlayerComponent* player = obj->GetComponent<PlayerComponent>();										// �v���C���[�R���|�[�l���g�̎擾


		// --- ��]���� ---
		Rotate(obj, player, controller, 90.0f);


		// --- �ˌ����� ---
		Shot(obj, player, controller);


		// --- �A�j���[�V�����̍X�V ---
		animator->Update(elapsedTime);
	}

		break;
	}
}


// --- ��]�̏��� ---
void BasePlayerBehavior::Rotate(GameObject* obj, PlayerComponent* player, PlayerControllerComponent* controller, float angleOffset)
{
	float theta = DirectX::XMConvertToRadians(player->angleOffset_ + obj->parent_->transform_->rotation_.y + angleOffset);
	obj->transform_->position_.x = cosf(-theta) * controller->range_;
	obj->transform_->position_.z = sinf(-theta) * controller->range_;

	obj->transform_->rotation_.y = obj->parent_->transform_->rotation_.y + player->angleOffset_;
}


// --- �ˌ����� ---
void BasePlayerBehavior::Shot(GameObject* obj, PlayerComponent* player, PlayerControllerComponent* controller)
{	
	InputManager& input = InputManager::Instance();

	// --- �ˌ��t���O ---
	bool shootable[2] =
	{
		static_cast<bool>(input.down(0) & input::LMB),
		static_cast<bool>(input.down(0) & input::RMB),
	};

	size_t playerIndex = player->playerNum_;

	// --- ���E�̃L�[�������ĂȂ������� ---
	switch (input.state(0) & (input::LEFT | input::RIGHT))
	{
	case input::LEFT:
	case input::RIGHT: break;

	default:

		// --- �ˌ����� ---
		if (shootable[playerIndex] && controller->bullet_[playerIndex] >= controller->bulletCost_)
		{
			// --- �e�ۂ̒ǉ� ---
			AddBullet(obj, 0.02f);

			// --- �e��̌��Z ---
			controller->bullet_[playerIndex] -= controller->bulletCost_;
			controller->bullet_[playerIndex] = (std::max)(controller->bullet_[playerIndex], 0.0f);
		}

	}

}


// --- �e�ۂ̒ǉ� ---
void BasePlayerBehavior::AddBullet(const GameObject* parent, const float scaling)
{
	GameObject* bullet = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		parent->transform_->position_,
		BehaviorManager::Instance().GetBehavior("Bullet")
	);

	bullet->name_ = u8"�e";
	bullet->eraser_ = EraserManager::Instance().GetEraser("Scene");

	bullet->transform_->scaling_ *= scaling;

	RigidBodyComponent* rigidBody = bullet->AddComponent<RigidBodyComponent>();
	rigidBody->velocity_ = Vector3::Zero_ - bullet->transform_->position_;

	InstancedMeshComponent* renderer = bullet->AddComponent<InstancedMeshComponent>();
	renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Bullet.fbx", 1000, nullptr, true);

	SphereCollider* collider = bullet->AddCollider<SphereCollider>();
	collider->radius_ = 1.0f;

}



// ===== �v���C���[�̑��쏈�� ======================================================================================================================================================
void PlayerControllerBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:

		obj->state_++;
		break;

	case 1:

	{
		InputManager& input = InputManager::Instance();

		PlayerControllerComponent* controller = obj->GetComponent<PlayerControllerComponent>();	// �R���g���[���[�R���|�[�l���g�̎擾

		Rotate(obj, controller, elapsedTime);	// ��]����
	}

	break;
	}
}


// --- ��]���� ---
void PlayerControllerBehavior::Rotate(GameObject* obj, PlayerControllerComponent* controller, float elapsedTime) const
{	
	InputManager& input = InputManager::Instance();


	// --- ���S�����_�ɉ�] ---
	float rotateSpeed = controller->rotateSpeed_ * elapsedTime;

	// --- ��]���� ---
	switch (input.state(0) & (input::LEFT | input::RIGHT))
	{
	case input::LEFT:
		obj->transform_->rotation_.y += rotateSpeed;								// ��]
		controller->actionGauge_ -= elapsedTime;									// �A�N�V�����Q�[�W�̌���
		controller->bullet_[0] += controller->addBulletValue_ * elapsedTime;		// �e��̑���
		controller->bullet_[1] += controller->addBulletValue_ * elapsedTime;		// �e��̑���
		break;

	case input::RIGHT:
		obj->transform_->rotation_.y += -rotateSpeed;								// ��]
		controller->actionGauge_ += elapsedTime;									// �A�N�V�����Q�[�W�̑���
		controller->bullet_[0] += controller->addBulletValue_ * elapsedTime;		// �e��̑���
		controller->bullet_[1] += controller->addBulletValue_ * elapsedTime;		// �e��̑���
		break;
	}


	// --- �p�x�� 0 ~ 360 �ɐ��� ---
	if (obj->transform_->rotation_.y > 360.0f) obj->transform_->rotation_.y -= 360.0f;
	if (obj->transform_->rotation_.y < 0.0f) obj->transform_->rotation_.y += 360.0f;

}


// --- �v���C���[�̒e��Q�[�W�̏��� ---
void PlayerBulletGaugeBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
		obj->state_++;
		break;

	case 1:
	{
		PrimitiveRendererComponent* renderer = obj->GetComponent<PrimitiveRendererComponent>();						// �`��R���|�[�l���g�擾
		PlayerControllerComponent*	controller = obj->parent_->parent_->GetComponent<PlayerControllerComponent>();	// �R���g���[���[�R���|�[�l���g�擾
		PlayerComponent*			player = obj->parent_->GetComponent<PlayerComponent>();							// �v���C���[�R���|�[�l���g�擾



		controller->bullet_[player->playerNum_] = (std::min)(controller->bullet_[player->playerNum_], controller->maxBulletValue_);	// �l�̐���

		renderer->size_.x = controller->bullet_[player->playerNum_];

		break;
	}

	}
}
