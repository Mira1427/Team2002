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
	void AddPlayer(std::string name, GameObject* parent, float rotate, float range);

	// --- プレイヤーのコントローラーの追加 ---
	GameObject* AddPlayerController(float rotateSpeed, float range);
};

