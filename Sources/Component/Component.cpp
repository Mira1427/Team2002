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

		ImGui::TreePop();
	}

}

void StageComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Enemy")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"���C�t", &life_, 0.1f);

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
