#include "Component.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/Graphics/EffectManager.h"
#include "../../Library/Library/CameraManager.h"


// --- プレイヤーコンポーネントのデバッグGui ---
void PlayerComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Player")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"回転のオフセット", &angleOffset_, 0.1f);
		ImGui::Text(type_ == CharacterType::BLACK ? u8"黒" : u8"白");

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
		ImGui::Separator();
		ImGui::DragFloat(u8"攻撃ゲージ", &attackGauge_, 0.1f);
		ImGui::DragFloat(u8"攻撃ゲージの増加量", &addAttackGaugeValue_, 0.01f);
		ImGui::Separator();
		ImGui::DragFloat(u8"弾薬のゲージ 左", &bullet_[0], 0.1f);
		ImGui::DragFloat(u8"弾薬のゲージ 右", &bullet_[1], 0.1f);
		ImGui::DragFloat(u8"弾薬の増加量", &addBulletValue_, 0.1f);
		ImGui::DragFloat(u8"弾薬の最大値", &maxBulletValue_, 0.1f);
		ImGui::DragFloat(u8"弾薬のコスト", &bulletCost_, 0.1f);
		ImGui::Separator();
		ImGui::DragFloat(u8"最大攻撃力", &maxAttackAmount_);
		ImGui::DragFloat(u8"最小攻撃力", &minAttackAmount_);
		ImGui::DragFloat(u8"最大範囲", &maxRangeAmount_);
		ImGui::DragFloat(u8"最小範囲", &minRangeAmount_);
		ImGui::DragFloat(u8"攻撃ゲージの高さ", &maxAttackGaugeHeight_);
		ImGui::Separator();
		ImGui::DragFloat(u8"レーザーの攻撃力", &laserAttackAmount_, 0.1f);
		ImGui::DragFloat3(u8"レーザーのサイズ", &laserSize_.x, 0.1f);
		ImGui::Separator();
		ImGui::Checkbox(u8"色交換アイテム", &hasSwapColor_);
		ImGui::Checkbox(u8"ゲージ交換アイテム", &hasSwapGauge_);

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
		ImGui::InputInt(u8"敵の生成数", &spawnCount_);

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
		ImGui::InputInt(u8"アイテムの種類", &itemType_);

		static const char* typeNames[3] = { u8"白", u8"黒", u8"グレー" };
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

		ImGui::InputInt(u8"ライフ", &life_);

		ImGui::TreePop();
	}
}



void BulletComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Bullet")) {
		ImGui::Spacing();

		ImGui::DragFloat(u8"攻撃力", &attack_, 0.1f);

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

		ImGui::InputInt(u8"ユニークID", &uniqueID_);
		ImGui::InputInt(u8"イベントID", &eventID_);
		ImGui::DragFloat3(u8"オフセット", &offset_.x);
		ImGui::DragFloat3(u8"初期位置", &basePosition_.x);

		ImGui::DragFloat(u8"イージングのタイマー", &easeData_.timer_);
		ImGui::DragFloat(u8"タイマーの上限", &easeData_.timerLimit_);
		ImGui::Checkbox(u8"反転するか", &easeData_.isReverse_);
		ImGui::DragFloat(u8"イージングの移動量", &easeScale_);

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

		ImGui::DragFloat3(u8"オフセット", &offset_.x);
		ImGui::DragFloat3(u8"強度", &shakeIntensity_.x);

		ImGui::TreePop();
	}
}



void EffectComponent::Draw(ID3D11DeviceContext* dc)
{
	auto* camera = CameraManager::Instance().currentCamera_->GetComponent<CameraComponent>();
	EffectManager::instance().render(camera->view_, camera->projection_);
}
