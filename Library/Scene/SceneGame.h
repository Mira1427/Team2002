#pragma once

#include "SceneManager.h"

#include <string>


class GameObject;

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

	// --- 弾薬ゲージの追加 ---
	void AddBulletGauge(GameObject* parent, int i);

	// --- スポナーの追加 ---
	void AddEnemySpawner();
};

