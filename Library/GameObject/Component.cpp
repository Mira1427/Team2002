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

		ImGui::TreePop();
	}
}
