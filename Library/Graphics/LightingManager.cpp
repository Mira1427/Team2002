#include "LightingManager.h"

#include "../../External/ImGui/imgui.h"


LightingManager::LightingManager()
{
	ambientLight_.color_ = { 0.3f, 0.3f, 0.3f };

	//directionLight_.direction_	= { 0.0f, -0.5f, 1.0f };
	directionLight_.direction_	= { -0.113f, -1.0f, 1.0f };
	directionLight_.intensity_	= 3.0f;
	directionLight_.color_		= { 1.0f, 1.0f, 1.0f };

	directionLight_.viewFocus_		= Vector3::Zero_;
	directionLight_.viewDistance_	= 10.0f;
	directionLight_.viewSize_		= 210.0f;
	directionLight_.viewNearZ_		= -90.0f;
	directionLight_.viewFarZ_		= 185.0f;
}


// --- 更新処理 ---
void LightingManager::Update()
{
	pointLights_.clear();
}


// --- デバッグGui更新 ---
void LightingManager::UpdateDebugGui()
{
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("AmbientLight"))
	{
		ImGui::ColorEdit3("Color", &ambientLight_.color_.x);

		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("DirectionLight"))
	{
		ImGui::DragFloat3("Direction", &directionLight_.direction_.x, 0.01f);
		ImGui::ColorEdit3("Color", &directionLight_.color_.x);
		ImGui::DragFloat("Intensity", &directionLight_.intensity_);
		ImGui::DragFloat3(u8"注視点", &directionLight_.viewFocus_.x);
		ImGui::DragFloat(u8"距離", &directionLight_.viewDistance_);
		ImGui::DragFloat(u8"サイズ", &directionLight_.viewSize_);
		ImGui::DragFloat(u8"最近距離", &directionLight_.viewNearZ_);
		ImGui::DragFloat(u8"最遠距離", &directionLight_.viewFarZ_);

		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("PointLight"))
	{
		ImGui::Text("Size : %d", static_cast<int>(pointLights_.size()));

		ImGui::TreePop();
	}

}
