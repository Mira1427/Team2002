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
		ImGui::DragFloat(u8"アクションのゲージ", &actionGauge_, 0.1f);
		ImGui::DragFloat(u8"弾薬のゲージ 左", &bullet_[0], 0.1f);
		ImGui::DragFloat(u8"弾薬のゲージ 右", &bullet_[1], 0.1f);
		ImGui::DragFloat(u8"弾薬の増加量", &addBulletValue_, 0.1f);
		ImGui::DragFloat(u8"弾薬の最大値", &maxBulletValue_, 0.1f);
		ImGui::DragFloat(u8"弾薬のコスト", &bulletCost_, 0.1f);

		ImGui::TreePop();
	}
}


// --- 敵のスポナーのデバッグGui ---
void EnemySpawnerComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"EnemySpawner")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"沸き速度", &spawnSpeed_, 0.1f);

		ImGui::TreePop();
	}
}

void EnemyComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Enemy")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"ライフ", &life_, 0.1f);

		ImGui::TreePop();
	}

}

void StageComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Enemy")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"ライフ", &life_, 0.1f);

		ImGui::TreePop();
	}
}
