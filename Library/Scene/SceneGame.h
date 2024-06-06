#pragma once

#include "SceneManager.h"

#include <string>


class GameObject;

class Vector3;

// ===== ゲーム ======================================================================================================================================================
class SceneGame final : public Scene
{
public:
	// --- 初期化 ---
	void Initialize() override;

	// --- 終了処理 ---
	void Finalize() override;

	// --- 更新処理 ---
	void Update(float elapsedTime) override;

	// --- 描画処理 ---
	void Render(ID3D11DeviceContext* dc) override;

	// --- ブルームの適用 ---
	void ApplyBloom(ID3D11DeviceContext* dc);


	// --- プレイヤーの追加 ---
	GameObject* AddPlayer(std::string name, GameObject* parent, float rotate, float range, int playerNum);

	// --- プレイヤーのコントローラーの追加 ---
	GameObject* AddPlayerController(float rotateSpeed, float range);

	// --- ステージの追加 ---
	void AddStage();

	// --- レールの追加 ---
	void AddRail();

	// --- 弾薬ゲージの追加 ---
	void AddBulletGauge(GameObject* parent, int i);

	// --- スポナーの追加 ---
	void AddEnemySpawner();

	// --- 街のライフ追加 ---
	void AddTownLife(const int life, const float range);

	// --- ライフゲージの追加 ---
	void AddLifeGauge();

	// --- 攻撃ゲージのコントローラー追加 ---
	GameObject* AddAttackGaugeController(GameObject* parent, const Vector3& position);

	// --- 攻撃ゲージの追加 ---
	void AddAttackGauge(GameObject* parent, float width);

	// --- 範囲ゲージの追加 ---
	void AddRangeGauge(GameObject* parent, float width);

	// --- 攻撃ゲージのバーの追加 ---
	void AddAttackGaugeBar(GameObject* parent);
};

