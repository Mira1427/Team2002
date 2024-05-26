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
		ImGui::Separator();
		ImGui::DragFloat(u8"UQ[W", &attackGauge_, 0.1f);
		ImGui::DragFloat(u8"UQ[WÌÁÊ", &addAttackGaugeValue_, 0.01f);
		ImGui::Separator();
		ImGui::DragFloat(u8"eòÌQ[W ¶", &bullet_[0], 0.1f);
		ImGui::DragFloat(u8"eòÌQ[W E", &bullet_[1], 0.1f);
		ImGui::DragFloat(u8"eòÌÁÊ", &addBulletValue_, 0.1f);
		ImGui::DragFloat(u8"eòÌÅål", &maxBulletValue_, 0.1f);
		ImGui::DragFloat(u8"eòÌRXg", &bulletCost_, 0.1f);

		ImGui::TreePop();
	}
}


// --- GÌX|i[ÌfobOGui ---
void EnemySpawnerComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"EnemySpawner")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"¦«¬x", &spawnSpeed_, 0.1f);

		ImGui::TreePop();
	}
}

void EnemyComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Enemy")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"Ct", &life_, 0.1f);

		ImGui::TreePop();
	}

}

void StageComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Enemy")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"Ct", &life_, 0.1f);

		ImGui::TreePop();
	}
}
