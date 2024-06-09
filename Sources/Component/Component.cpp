#include "Component.h"

#include "../../External/ImGui/imgui.h"


// --- �v���C���[�R���|�[�l���g�̃f�o�b�OGui ---
void PlayerComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Player")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"��]�̃I�t�Z�b�g", &angleOffset_, 0.1f);
		ImGui::Text(type_ == CharacterType::BLACK ? u8"��" : u8"��");

		ImGui::TreePop();
	}
}


// --- �v���C���[����R���|�[�l���g�̃f�o�b�OGui ---
void PlayerControllerComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"PlayerController")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"����", &range_, 0.1f);
		ImGui::DragFloat(u8"��]���x", &rotateSpeed_, 5.0f);
		ImGui::Separator();
		ImGui::DragFloat(u8"�U���Q�[�W", &attackGauge_, 0.1f);
		ImGui::DragFloat(u8"�U���Q�[�W�̑�����", &addAttackGaugeValue_, 0.01f);
		ImGui::Separator();
		ImGui::DragFloat(u8"�e��̃Q�[�W ��", &bullet_[0], 0.1f);
		ImGui::DragFloat(u8"�e��̃Q�[�W �E", &bullet_[1], 0.1f);
		ImGui::DragFloat(u8"�e��̑�����", &addBulletValue_, 0.1f);
		ImGui::DragFloat(u8"�e��̍ő�l", &maxBulletValue_, 0.1f);
		ImGui::DragFloat(u8"�e��̃R�X�g", &bulletCost_, 0.1f);
		ImGui::Separator();
		ImGui::DragFloat(u8"�ő�U����", &maxAttackAmount_);
		ImGui::DragFloat(u8"�ŏ��U����", &minAttackAmount_);
		ImGui::DragFloat(u8"�ő�͈�", &maxRangeAmount_);
		ImGui::DragFloat(u8"�ŏ��͈�", &minRangeAmount_);
		ImGui::DragFloat(u8"�U���Q�[�W�̍���", &maxAttackGaugeHeight_);
		ImGui::Separator();
		ImGui::DragFloat(u8"���[�U�[�̍U����", &laserAttackAmount_, 0.1f);
		ImGui::DragFloat3(u8"���[�U�[�̃T�C�Y", &laserSize_.x, 0.1f);
		ImGui::Separator();
		ImGui::Checkbox(u8"�F�����A�C�e��", &hasSwapColor_);
		ImGui::Checkbox(u8"�Q�[�W�����A�C�e��", &hasSwapGauge_);

		ImGui::TreePop();
	}
}


// --- �G�̃X�|�i�[�̃f�o�b�OGui ---
void EnemySpawnerComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"EnemySpawner")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"�������x", &spawnSpeed_, 0.1f);
		ImGui::InputInt(u8"�G�̐�����", &spawnCount_);

		ImGui::TreePop();
	}
}

void EnemyComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Enemy")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"���C�t", &life_, 0.1f);
		ImGui::InputInt(u8"�A�C�e���̎��", &itemType_);

		static const char* typeNames[3] = { u8"��", u8"��", u8"�O���[" };
		ImGui::Text(typeNames[static_cast<size_t>(type_)]);

		ImGui::TreePop();
	}

}

void StageComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Stage")) {
		ImGui::Spacing();

		ImGui::InputInt(u8"���C�t", &life_);

		ImGui::TreePop();
	}
}



void BulletComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Bullet")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"�U����", &attack_, 0.1f);

		ImGui::TreePop();
	}
}

void UIComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("UIChild"))
	{
		ImGui::Spacing();

		ImGui::InputInt(u8"���j�[�NID", &uniqueID_);
		ImGui::InputInt(u8"�C�x���gID", &eventID_);
		ImGui::DragFloat3(u8"�I�t�Z�b�g", &offset_.x);
		ImGui::DragFloat3(u8"�����ʒu", &basePosition_.x);

		ImGui::DragFloat(u8"�C�[�W���O�̃^�C�}�[", &easeData_.timer_);
		ImGui::DragFloat(u8"�^�C�}�[�̏��", &easeData_.timerLimit_);
		ImGui::Checkbox(u8"���]���邩", &easeData_.isReverse_);
		ImGui::DragFloat(u8"�C�[�W���O�̈ړ���", &easeScale_);

		ImGui::TreePop();
	}
}


void CameraShakeComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("CameraShake"))
	{
		ImGui::Spacing();

		ImGui::DragFloat3(u8"�I�t�Z�b�g", &offset_.x);
		ImGui::DragFloat3(u8"���x", &shakeIntensity_.x);

		ImGui::TreePop();
	}
}
