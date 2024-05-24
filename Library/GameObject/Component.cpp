#include "Component.h"

#include "../../External/ImGui/imgui.h"


// --- vC[R|[lgÌfobOGui ---
void PlayerComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Player")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"ñ]ÌItZbg", &angleOffset_, 0.1f);

		ImGui::TreePop();
	}
}


// --- vC[ìR|[lgÌfobOGui ---
void PlayerControllerComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"PlayerController")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"£", &range_, 0.1f);
		ImGui::DragFloat(u8"ñ]¬x", &rotateSpeed_, 5.0f);
		ImGui::DragFloat(u8"ANVÌQ[W", &actionGauge_, 0.1f);
		ImGui::DragFloat(u8"eòÌQ[W ¶", &bullet_[0], 0.1f);
		ImGui::DragFloat(u8"eòÌQ[W E", &bullet_[1], 0.1f);
		ImGui::DragFloat(u8"eòÌÁÊ", &addBulletValue_, 0.1f);
		ImGui::DragFloat(u8"eòÌÅål", &maxBulletValue_, 0.1f);
		ImGui::DragFloat(u8"eòÌRXg", &bulletCost_, 0.1f);

		ImGui::TreePop();
	}
}
