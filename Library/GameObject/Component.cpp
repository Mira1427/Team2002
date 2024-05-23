#include "Component.h"

#include "../../External/ImGui/imgui.h"


// --- プレイヤーコンポーネントのデバッグGui ---
void PlayerComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Player")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"回転のオフセット", &angleOffset_, 0.1f);

		ImGui::TreePop();
	}
}


// --- プレイヤー操作コンポーネントのデバッグGui ---
void PlayerControllerComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"PlayerController")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"距離", &range_, 0.1f);
		ImGui::DragFloat(u8"回転速度", &rotateSpeed_, 5.0f);

		ImGui::TreePop();
	}
}
